/*
 * SoemDSP Starting Point
 *
 * Basically lets paul bootstrap his projects.
 *
 * Copyright 2024-2025, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work will be
 * released under GPL3.
 *
 * The source code and license are at https://github.com/baconpaul/SoemDSP-startingpoint
 */

#include "engine/engine.h"

#include <cmath>
#include <cstring>
#include "sst/cpputils/constructors.h"
#include "sst/basic-blocks/mechanics/block-ops.h"
#include "sst/basic-blocks/dsp/PanLaws.h"

#include "libMTSClient.h"

namespace soemdsp_ap
{

int debugLevel{0};

namespace mech = sst::basic_blocks::mechanics;
namespace sdsp = sst::basic_blocks::dsp;

Engine::Engine()
    : responder(*this), monoResponder(*this),
      voices(sst::cpputils::make_array<Voice, VMConfig::maxVoiceCount>(patch))
{
    voiceManager = std::make_unique<voiceManager_t>(responder, monoResponder);
}

Engine::~Engine() {}

void Engine::setSampleRate(double sr)
{
    sampleRate = sr;
    sampleRateInv = 1.0 / sr;
    midiCCLagCollection.setRateInMilliseconds(1000.0 * 128.0 / 48000.0, sampleRate,
                                              1.0 / blockSize);
    midiCCLagCollection.snapAllActiveToTarget();

    vuPeak.setSampleRate(sampleRate);
    sandboxGraph.setup((float)sampleRate);

    audioToUi.push({AudioToUIMsg::SEND_SAMPLE_RATE, 0, (float)sampleRate});
}

void Engine::publishNoteChunks(double c0, double c1, double c2)
{
    noteChunkSeq.fetch_add(1, std::memory_order_release);
    noteC0 = c0;
    noteC1 = c1;
    noteC2 = c2;
    noteChunkSeq.fetch_add(1, std::memory_order_release);
}

void Engine::noteOn(int key)
{
    if (key < 0 || key > 127)
        return;
    heldNotes[key] = 1;
    notesDirty = true;
}

void Engine::noteOff(int key)
{
    if (key < 0 || key > 127)
        return;
    heldNotes[key] = 0;
    notesDirty = true;
}

static void unpackNoteChunks(uint8_t *held, double c0, double c1, double c2)
{
    memset(held, 0, 128);
    auto take = [&](double bits, int start, int count) {
        double place = 1.0;
        double n = bits;
        if (!(n == n) || n < 0.0)
            n = 0.0;
        for (int b = 0; b < count; ++b)
        {
            const int midi = start + b;
            if (midi >= 0 && midi < 128 && std::fmod(std::floor(n / place), 2.0) >= 1.0)
                held[midi] = 1;
            place *= 2.0;
        }
    };
    take(c0, 0, 49);
    take(c1, 49, 49);
    take(c2, 98, 30);
}

static void packNoteChunks(const uint8_t *held, double &c0, double &c1, double &c2)
{
    auto range = [&](int start, int end) {
        double bits = 0.0;
        double place = 1.0;
        for (int m = start; m <= end; ++m)
        {
            if (held[m])
                bits += place;
            place *= 2.0;
        }
        return bits;
    };
    c0 = range(0, 48);
    c1 = range(49, 97);
    c2 = range(98, 127);
}

void Engine::flushNoteChunks()
{
    const uint32_t s1 = noteChunkSeq.load(std::memory_order_acquire);
    if ((s1 & 1u) == 0 && s1 != noteChunkApplied)
    {
        const double a = noteC0;
        const double b = noteC1;
        const double c = noteC2;
        const uint32_t s2 = noteChunkSeq.load(std::memory_order_acquire);
        if (s1 == s2)
        {
            unpackNoteChunks(heldNotes, a, b, c);
            noteChunkApplied = s1;
            notesDirty = true;
        }
    }
    if (!notesDirty)
        return;
    double a = 0, b = 0, c = 0;
    packNoteChunks(heldNotes, a, b, c);
    sandboxGraph.setGravityWalkerChunks(a, b, c);
    notesDirty = false;
}

void Engine::process(const clap_output_events_t *outq, const float *inL, const float *inR)
{
    processUIQueue(outq);
    flushNoteChunks();

    if (!audioRunning)
    {
        memset(output, 0, sizeof(output));
        return;
    }

    memset(output, 0, sizeof(output));
    sandboxGraph.process(inL, inR, output[0], output[1], (int)blockSize);

    if (isEditorAttached)
    {
        for (int i = 0; i < blockSize; ++i)
        {
            vuPeak.process(output[0][i], output[1][i]);
        }

        if (lastVuUpdate >= updateVuEvery)
        {
            AudioToUIMsg msg{AudioToUIMsg::UPDATE_VU, 0, vuPeak.vu_peak[0], vuPeak.vu_peak[1]};
            audioToUi.push(msg);

            AudioToUIMsg msg2{AudioToUIMsg::UPDATE_VOICE_COUNT, (uint32_t)voiceCount};
            audioToUi.push(msg2);
            lastVuUpdate = 0;
        }
        else
        {
            lastVuUpdate++;
        }
    }
}

void Engine::addToVoiceList(Voice *v)
{
    v->prior = nullptr;
    v->next = head;
    if (v->next)
    {
        v->next->prior = v;
    }
    head = v;
    voiceCount++;
}

Voice *Engine::removeFromVoiceList(Voice *cvoice)
{
    if (cvoice->prior)
    {
        cvoice->prior->next = cvoice->next;
    }
    if (cvoice->next)
    {
        cvoice->next->prior = cvoice->prior;
    }
    if (cvoice == head)
    {
        head = cvoice->next;
    }
    auto nv = cvoice->next;
    cvoice->cleanup();
    cvoice->next = nullptr;
    cvoice->prior = nullptr;
    voiceCount--;

    return nv;
}

void Engine::dumpVoiceList()
{
    SQLOG("DUMP VOICE LIST : head=" << std::hex << head << std::dec);
    auto c = head;
    while (c)
    {
        // SQLOG("   c=" << std::hex << c << std::dec << " key=" << c->voiceValues.key
        //                 << " u=" << c->used);
        c = c->next;
    }
}

void Engine::processUIQueue(const clap_output_events_t *outq)
{
    bool didRefresh{false};
    if (doFullRefresh)
    {
        pushFullUIRefresh();
        doFullRefresh = false;
        didRefresh = true;
    }
    auto uiM = mainToAudio.pop();
    while (uiM.has_value())
    {
        switch (uiM->action)
        {
        case MainToAudioMsg::REQUEST_REFRESH:
        {
            if (!didRefresh)
            {
                // don't do it twice in one process obvs
                pushFullUIRefresh();
            }
        }
        break;
        case MainToAudioMsg::SET_PARAM_WITHOUT_NOTIFYING:
        case MainToAudioMsg::SET_PARAM:
        {
            bool notify = uiM->action == MainToAudioMsg::SET_PARAM;

            auto dest = patch.paramMap.at(uiM->paramId);
            dest->value = uiM->value;
            if (uiM->paramId >= 1000 && uiM->paramId < 1032)
                sandboxGraph.setSlot((int)(uiM->paramId - 1000), uiM->value);
            if (notify)
            {
                clap_event_param_value_t p;
                p.header.size = sizeof(clap_event_param_value_t);
                p.header.time = 0;
                p.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
                p.header.type = CLAP_EVENT_PARAM_VALUE;
                p.header.flags = 0;
                p.param_id = uiM->paramId;
                p.cookie = dest;

                p.note_id = -1;
                p.port_index = -1;
                p.channel = -1;
                p.key = -1;

                p.value = uiM->value;

                outq->try_push(outq, &p.header);
            }

            // Side Effects and Ad Hoc Features go here

            auto d = patch.dirty;
            if (!d)
            {
                patch.dirty = true;
                audioToUi.push({AudioToUIMsg::SET_PATCH_DIRTY_STATE, patch.dirty});
            }
        }
        break;
        case MainToAudioMsg::BEGIN_EDIT:
        case MainToAudioMsg::END_EDIT:
        {
            if (uiM->action == MainToAudioMsg::BEGIN_EDIT)
            {
                beginEndParamGestureCount++;
                gesturingParamId = (int32_t)uiM->paramId;
            }
            else
            {
                beginEndParamGestureCount--;
                if (gesturingParamId == (int32_t)uiM->paramId)
                    gesturingParamId = -1;
            }
            clap_event_param_gesture_t p;
            p.header.size = sizeof(clap_event_param_gesture_t);
            p.header.time = 0;
            p.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
            p.header.type = uiM->action == MainToAudioMsg::BEGIN_EDIT
                                ? CLAP_EVENT_PARAM_GESTURE_BEGIN
                                : CLAP_EVENT_PARAM_GESTURE_END;
            p.header.flags = 0;
            p.param_id = uiM->paramId;

            outq->try_push(outq, &p.header);
        }
        break;
        case MainToAudioMsg::STOP_AUDIO:
        {
            if (voiceManager)
                voiceManager->allSoundsOff();
            audioRunning = false;
        }
        break;
        case MainToAudioMsg::START_AUDIO:
        {
            audioRunning = true;
        }
        break;
        case MainToAudioMsg::SEND_PATCH_NAME:
        {
            memset(patch.name, 0, sizeof(patch.name));
            strncpy(patch.name, uiM->uiManagedPointer, 255);
            audioToUi.push({AudioToUIMsg::SET_PATCH_NAME, 0, 0, 0, patch.name});
        }
        break;
        case MainToAudioMsg::SEND_PATCH_IS_CLEAN:
        {
            patch.dirty = false;
            audioToUi.push({AudioToUIMsg::SET_PATCH_DIRTY_STATE, patch.dirty});
        }
        break;
        case MainToAudioMsg::SEND_POST_LOAD:
        {
            postLoad();
        }
        break;
        case MainToAudioMsg::SEND_PREP_FOR_STREAM:
        {
            prepForStream();
        }
        break;
        case MainToAudioMsg::SEND_REQUEST_RESCAN:
        {
            onMainRescanParams = true;
            audioToUi.push({AudioToUIMsg::DO_PARAM_RESCAN});
            clapHost->request_callback(clapHost);
        }
        break;
        case MainToAudioMsg::EDITOR_ATTACH_DETATCH:
        {
            isEditorAttached = uiM->paramId;
        }
        break;
        case MainToAudioMsg::LOAD_SANDBOX_JSON:
        {
            auto *plan = reinterpret_cast<GraphPlan *>((void *)uiM->uiManagedPointer);
            if (plan)
            {
                sandboxGraph.applyPlan(*plan);
                delete plan;
            }
        }
        break;
        case MainToAudioMsg::PANIC_STOP_VOICES:
        {
            voiceManager->allSoundsOff();
        }
        break;
        }
        uiM = mainToAudio.pop();
    }
}

void Engine::handleParamValue(Param *p, uint32_t pid, float value)
{
    if (!p)
    {
        p = patch.paramMap.at(pid);
    }

    p->value = value;
    if (pid >= 1000 && pid < 1032)
        sandboxGraph.setSlot((int)(pid - 1000), value);

    if (gesturingParamId == (int32_t)pid)
        return;

    AudioToUIMsg au = {AudioToUIMsg::UPDATE_PARAM, pid, value};
    audioToUi.push(au);
}

void Engine::pushFullUIRefresh()
{
    for (const auto *p : patch.params)
    {
        AudioToUIMsg au = {AudioToUIMsg::UPDATE_PARAM, p->meta.id, p->value};
        audioToUi.push(au);
    }
    audioToUi.push({AudioToUIMsg::SET_PATCH_NAME, 0, 0, 0, patch.name});
    audioToUi.push({AudioToUIMsg::SET_PATCH_DIRTY_STATE, patch.dirty});
    audioToUi.push({AudioToUIMsg::SEND_SAMPLE_RATE, 0, (float)sampleRate});
}

void Engine::onMainThread()
{
    bool ex{true}, re{false};
    if (onMainRescanParams.compare_exchange_strong(ex, re))
    {
        auto pe = static_cast<const clap_host_params_t *>(
            clapHost->get_extension(clapHost, CLAP_EXT_PARAMS));
        if (pe)
        {
            pe->rescan(clapHost, CLAP_PARAM_RESCAN_VALUES | CLAP_PARAM_RESCAN_TEXT);
        }
    }
}

} // namespace soemdsp_ap
