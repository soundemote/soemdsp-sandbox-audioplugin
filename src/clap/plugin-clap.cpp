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

#include "configuration.h"
#include <clap/clap.h>
#include <clap/fixedpoint.h>
#include <chrono>
#include <cstring>

#include <clap/helpers/plugin.hh>
#include "engine/engine.h"
#include "presets/preset-manager.h"

#include <clap/helpers/plugin.hxx>
#include <clap/helpers/host-proxy.hxx>

#include <memory>
#include "sst/plugininfra/patch-support/patch_base_clap_adapter.h"
#include "sst/plugininfra/cpufeatures.h"

#include "sst/voicemanager/midi1_to_voicemanager.h"
#include "sst/clap_juce_shim/clap_juce_shim.h"

#include "ui/plugin-editor.h"

#include <clapwrapper/vst3.h>

namespace soemdsp_ap
{

extern const clap_plugin_descriptor *getDescriptor();

namespace clapimpl
{

static constexpr clap::helpers::MisbehaviourHandler misLevel =
    clap::helpers::MisbehaviourHandler::Ignore;
static constexpr clap::helpers::CheckingLevel checkLevel = clap::helpers::CheckingLevel::Maximal;

using plugHelper_t = clap::helpers::Plugin<misLevel, checkLevel>;

template <bool multiOut> struct SoemDSP : public plugHelper_t, sst::clap_juce_shim::EditorProvider
{
    SoemDSP(const clap_host *h) : plugHelper_t(getDescriptor(), h)
    {
        engine = std::make_unique<Engine>();

        engine->clapHost = h;

        clapJuceShim = std::make_unique<sst::clap_juce_shim::ClapJuceShim>(this);
        clapJuceShim->setResizable(true);
    }
    virtual ~SoemDSP(){};

    std::unique_ptr<Engine> engine;
    size_t blockPos{0};

  protected:
    bool activate(double sampleRate, uint32_t minFrameCount,
                  uint32_t maxFrameCount) noexcept override
    {
        engine->setSampleRate(sampleRate);
        return true;
    }

    void onMainThread() noexcept override { engine->onMainThread(); }

    bool implementsAudioPorts() const noexcept override { return true; }
    uint32_t audioPortsCount(bool isInput) const noexcept override { return 1; }
    bool audioPortsInfo(uint32_t index, bool isInput,
                        clap_audio_port_info *info) const noexcept override
    {
        // Stereo effect: one main in, one main out (sandbox patch player).
        if (index != 0)
            return false;
        info->id = isInput ? 75240 : 75241;
        // Allow hosts to use the same buffer for main in/out when possible.
        info->in_place_pair = isInput ? 75241 : 75240;
        strncpy(info->name, isInput ? "Main In" : "Main Out", sizeof(info->name));
        info->flags = CLAP_AUDIO_PORT_IS_MAIN;
        info->channel_count = 2;
        info->port_type = CLAP_PORT_STEREO;
        return true;
    }
    bool implementsAudioPortsActivation() const noexcept override { return true; }
    bool audioPortsActivationCanActivateWhileProcessing() const noexcept override { return true; }
    bool audioPortsActivationSetActive(bool is_input, uint32_t port_index, bool is_active,
                                       uint32_t sample_size) noexcept override
    {
        return true;
    }

    bool implementsNotePorts() const noexcept override { return true; }
    uint32_t notePortsCount(bool isInput) const noexcept override { return isInput ? 1 : 0; }
    bool notePortsInfo(uint32_t index, bool isInput,
                       clap_note_port_info *info) const noexcept override
    {
        assert(isInput);
        assert(index == 0);
        if (!isInput || index != 0)
            return false;

        info->id = 17252;
        info->supported_dialects =
            CLAP_NOTE_DIALECT_MIDI | CLAP_NOTE_DIALECT_MIDI_MPE | CLAP_NOTE_DIALECT_CLAP;
        info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
        strncpy(info->name, "Note Input", CLAP_NAME_SIZE - 1);
        return true;
    }

    clap_process_status process(const clap_process *process) noexcept override
    {
        auto fpuguard = sst::plugininfra::cpufeatures::FPUStateGuard();

        auto ev = process->in_events;
        auto outq = process->out_events;
        auto sz = ev->size(ev);

        const clap_event_header_t *nextEvent{nullptr};
        uint32_t nextEventIndex{0};
        if (sz != 0)
        {
            nextEvent = ev->get(ev, nextEventIndex);
        }

        double hostBpm = 0.0;
        double hostPosSamples = 0.0;
        bool hostPosLock = false;
        if (process->transport)
        {
            const auto *tr = process->transport;
            if ((tr->flags & CLAP_TRANSPORT_HAS_TEMPO) && tr->tempo > 1.0)
                hostBpm = tr->tempo;
            // song_pos_* are clap fixed-point (value * 2^31), not raw seconds or beats.
            // Reading them raw multiplies the playhead by 2^31, so the 16th-note gate
            // never lands on the song clock and the walker does not step a melody.
            if (tr->flags & CLAP_TRANSPORT_HAS_SECONDS_TIMELINE)
            {
                const double seconds = (double)tr->song_pos_seconds / (double)CLAP_SECTIME_FACTOR;
                hostPosSamples = seconds * engine->sampleRate;
                hostPosLock = true;
            }
            else if ((tr->flags & CLAP_TRANSPORT_HAS_BEATS_TIMELINE) && hostBpm > 1.0)
            {
                const double beats = (double)tr->song_pos_beats / (double)CLAP_BEATTIME_FACTOR;
                hostPosSamples = beats * (60.0 / hostBpm) * engine->sampleRate;
                hostPosLock = true;
            }
        }

        // Stereo effect: host in → engine.output (meters) → host out. Same buffer.
        if (process->audio_outputs_count < 1 || !process->audio_outputs[0].data32 ||
            !process->audio_outputs[0].data32[0])
        {
            return CLAP_PROCESS_CONTINUE;
        }
        float *outL = process->audio_outputs[0].data32[0];
        float *outR = (process->audio_outputs[0].channel_count > 1 &&
                       process->audio_outputs[0].data32[1])
                          ? process->audio_outputs[0].data32[1]
                          : outL;

        float *inL = nullptr;
        float *inR = nullptr;
        if (process->audio_inputs_count > 0 && process->audio_inputs[0].data32 &&
            process->audio_inputs[0].data32[0])
        {
            inL = process->audio_inputs[0].data32[0];
            inR = (process->audio_inputs[0].channel_count > 1 &&
                   process->audio_inputs[0].data32[1])
                      ? process->audio_inputs[0].data32[1]
                      : inL;
        }

        for (auto s = 0U; s < process->frames_count; ++s)
        {
            if (blockPos == 0)
            {
                while (nextEvent && nextEvent->time <= s)
                {
                    handleEvent(nextEvent);
                    nextEventIndex++;
                    if (nextEventIndex < sz)
                        nextEvent = ev->get(ev, nextEventIndex);
                    else
                        nextEvent = nullptr;
                }

                // Pass contiguous input block (or null) into the engine.
                const float *blockInL = inL ? (inL + s) : nullptr;
                const float *blockInR = inR ? (inR + s) : nullptr;
                engine->sandboxGraph.setHostTransport(
                    hostBpm, hostPosSamples + (double)s, hostPosLock);
                engine->process(outq, blockInL, blockInR);
            }

            outL[s] = engine->output[0][blockPos];
            outR[s] = engine->output[1][blockPos];

            blockPos++;
            if (blockPos == blockSize)
            {
                blockPos = 0;
            }
        }

        while (nextEvent)
        {
            handleEvent(nextEvent);
            nextEventIndex++;
            if (nextEventIndex < sz)
                nextEvent = ev->get(ev, nextEventIndex);
            else
                nextEvent = nullptr;
        }
        return CLAP_PROCESS_CONTINUE;
    }

    void reset() noexcept override { engine->voiceManager->allSoundsOff(); }

    bool handleEvent(const clap_event_header_t *nextEvent)
    {
        if (nextEvent->space_id == CLAP_CORE_EVENT_SPACE_ID)
        {
            switch (nextEvent->type)
            {
            case CLAP_EVENT_NOTE_ON:
            {
                auto ne = reinterpret_cast<const clap_event_note *>(nextEvent);
                engine->noteOn(ne->key);
            }
            break;
            case CLAP_EVENT_NOTE_OFF:
            {
                auto ne = reinterpret_cast<const clap_event_note *>(nextEvent);
                engine->noteOff(ne->key);
            }
            break;
            case CLAP_EVENT_MIDI:
            case CLAP_EVENT_NOTE_EXPRESSION:
                break;
            case CLAP_EVENT_PARAM_VALUE:
            {
                auto pevt = reinterpret_cast<const clap_event_param_value *>(nextEvent);
                auto par =
                    sst::plugininfra::patch_support::paramFromClapEvent<Param>(pevt, engine->patch);
                if (par)
                {
                    engine->handleParamValue(par, pevt->param_id, pevt->value);
                }
            }
            break;

            default:
            {
                SQLOG("Unknown inbound event of type " << nextEvent->type);
            }
            break;
            }
        }
        return true;
    }

    bool implementsState() const noexcept override { return true; }
    bool stateSave(const clap_ostream *ostream) noexcept override
    {
        engine->mainToAudio.push({Engine::MainToAudioMsg::SEND_PREP_FOR_STREAM});
        if (_host.canUseParams())
            _host.paramsRequestFlush();

        // best efforts on that message for now
        static constexpr int maxIts{5};
        int i{0};
        for (i = 0; i < maxIts && !engine->readyForStream; ++i)
        {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(4ms);
        }
        if (i == maxIts && !engine->readyForStream)
        {
            // sigh. something is wonky
            engine->prepForStream();
        }

        auto res = sst::plugininfra::patch_support::patchToOutStream(engine->patch, ostream);
        engine->readyForStream = false;

        return res;
    }

    bool stateLoad(const clap_istream *istream) noexcept override
    {
        Patch patchCopy;
        if (!sst::plugininfra::patch_support::inStreamToPatch(istream, patchCopy))
            return false;

        presets::PresetManager::sendEntirePatchToAudio(patchCopy, engine->mainToAudio,
                                                       patchCopy.name, _host.host());
        if (_host.canUseParams())
        {
            _host.paramsRescan(CLAP_PARAM_RESCAN_VALUES);
            _host.paramsRequestFlush();
        }
        return true;
    }

    bool implementsParams() const noexcept override { return true; }
    uint32_t paramsCount() const noexcept override { return engine->patch.params.size(); }
    bool paramsInfo(uint32_t paramIndex, clap_param_info *info) const noexcept override
    {
        if (!sst::plugininfra::patch_support::patchParamsInfo(paramIndex, info, engine->patch))
            return false;
        if (!info)
            return true;
        auto *param = engine->patch.params[paramIndex];
        if (!param)
            return true;
        const uint32_t pid = param->meta.id;
        if (pid < Patch::SoemDSPNode::idBase ||
            pid >= Patch::SoemDSPNode::idBase + (uint32_t)Patch::SoemDSPNode::slotCount)
            return true;
        const int slot = (int)(pid - Patch::SoemDSPNode::idBase);
        for (const auto &sb : engine->sandboxGraph.slots)
        {
            if (sb.slot != slot)
                continue;
            if (!sb.name.empty())
            {
                std::strncpy(info->name, sb.name.c_str(), CLAP_NAME_SIZE - 1);
                info->name[CLAP_NAME_SIZE - 1] = 0;
            }
            if (!sb.folder.empty())
            {
                std::strncpy(info->module, sb.folder.c_str(), CLAP_NAME_SIZE - 1);
                info->module[CLAP_NAME_SIZE - 1] = 0;
            }
            break;
        }
        return true;
    }
    bool paramsValue(clap_id paramId, double *value) noexcept override
    {
        return sst::plugininfra::patch_support::patchParamsValue(paramId, value, engine->patch);
    }
    bool paramsValueToText(clap_id paramId, double value, char *display,
                           uint32_t size) noexcept override
    {
        return sst::plugininfra::patch_support::patchParamsValueToText(paramId, value, display,
                                                                       size, engine->patch);
    }
    bool paramsTextToValue(clap_id paramId, const char *display, double *value) noexcept override
    {
        return sst::plugininfra::patch_support::patchParamsTextToValue(paramId, display, value,
                                                                       engine->patch);
    }
    void paramsFlush(const clap_input_events *in, const clap_output_events *out) noexcept override
    {
        auto sz = in->size(in);

        for (int i = 0; i < sz; ++i)
        {
            const clap_event_header_t *nextEvent{nullptr};
            nextEvent = in->get(in, i);
            handleEvent(nextEvent);
        }

        engine->processUIQueue(out);
    }

  public:
    bool implementsGui() const noexcept override { return clapJuceShim != nullptr; }
    std::unique_ptr<sst::clap_juce_shim::ClapJuceShim> clapJuceShim;
    ADD_SHIM_IMPLEMENTATION(clapJuceShim)
    ADD_SHIM_LINUX_TIMER(clapJuceShim)
    std::unique_ptr<juce::Component> createEditor() override
    {
        auto res = std::make_unique<soemdsp_ap::ui::PluginEditor>(
            engine->audioToUi, engine->mainToAudio, _host.host());
        res->performNotesHandler = [eng = engine.get()](double c0, double c1, double c2) {
            eng->publishNoteChunks(c0, c1, c2);
        };

        res->onZoomChanged = [this](auto f)
        {
            if (_host.canUseGui() && clapJuceShim->isEditorAttached())
            {
                // SQLOG("onZoomChanged " << f);
                auto s = f * clapJuceShim->getGuiScale();
                guiSetSize(soemdsp_ap::ui::PluginEditor::edWidth * s,
                           soemdsp_ap::ui::PluginEditor::edHeight * s);
                _host.guiRequestResize(soemdsp_ap::ui::PluginEditor::edWidth * s,
                                       soemdsp_ap::ui::PluginEditor::edHeight * s);
            }
        };

        onShow = [e = res.get()]()
        {
            // SQLOG("onShow with zoom factor " << e->zoomFactor);
            e->setZoomFactor(e->zoomFactor);
            return true;
        };
        // res->sneakyStartupGrabFrom(engine->patch);
        res->repaint();

        return res;
    }

    bool registerOrUnregisterTimer(clap_id &id, int ms, bool reg) override
    {
        if (!_host.canUseTimerSupport())
            return false;
        if (reg)
        {
            _host.timerSupportRegister(ms, &id);
        }
        else
        {
            _host.timerSupportUnregister(id);
        }
        return true;
    }

    bool registerOrUnregisterPosixFd(int fd, clap_posix_fd_flags_t flags, bool reg) override
    {
        if (!_host.canUsePosixFdSupport())
            return false;

        if (reg)
        {
            return _host.posixFdSupportRegister(fd, flags);
        }

        return _host.posixFdSupportUnregister(fd);
    }

    static uint32_t vst3_getNumMIDIChannels(const clap_plugin *plugin, uint32_t note_port)
    {
        return 16;
    }
    static uint32_t vst3_supportedNoteExpressions(const clap_plugin *plugin)
    {
        return clap_supported_note_expressions::AS_VST3_NOTE_EXPRESSION_TUNING |
               clap_supported_note_expressions::AS_VST3_NOTE_EXPRESSION_PAN;
    }

    const void *extension(const char *id) noexcept override
    {
        if (strcmp(id, CLAP_PLUGIN_AS_VST3) == 0)
        {
            static clap_plugin_as_vst3 v3p{vst3_getNumMIDIChannels, vst3_supportedNoteExpressions};
            return &v3p;
        }

        return nullptr;
    }
};

} // namespace clapimpl

const clap_plugin *makePlugin(const clap_host *h, bool multiOut)
{
    if (multiOut)
    {
        auto res = new soemdsp_ap::clapimpl::SoemDSP<true>(h);
        return res->clapPlugin();
    }
    else
    {
        auto res = new soemdsp_ap::clapimpl::SoemDSP<false>(h);
        return res->clapPlugin();
    }
}
} // namespace soemdsp_ap

namespace chlp = clap::helpers;
namespace bpss = soemdsp_ap::clapimpl;

template class chlp::Plugin<bpss::misLevel, bpss::checkLevel>;
template class chlp::HostProxy<bpss::misLevel, bpss::checkLevel>;
