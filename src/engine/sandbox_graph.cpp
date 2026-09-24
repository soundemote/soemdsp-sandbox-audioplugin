#include "engine/sandbox_graph.h"
#include "soemdsp_graph.h"

#include <cstring>

namespace soemdsp_ap
{

void SandboxGraph::shutdown()
{
    if (handle > 0)
        soemdsp_graph_destroy(handle);
    handle = 0;
    ready = false;
}

bool SandboxGraph::applyPlan(const GraphPlan &plan)
{
    shutdown();
    handle = soemdsp_graph_create();
    if (handle <= 0)
        return false;
    soemdsp_graph_set_sample_rate(handle, sampleRate);
    for (auto &n : plan.nodes)
    {
        if (soemdsp_graph_add_node(handle, n.hash, n.typeId) != 0)
            continue;
    }
    for (auto &c : plan.conns)
        soemdsp_graph_connect(handle, c.srcHash, c.srcPort, c.dstHash, c.dstPort);
    for (auto &d : plan.domains)
        soemdsp_graph_set_param_domain(handle, d.hash, d.paramId, d.minV, d.maxV, d.flags);
    for (auto &m : plan.mods)
        soemdsp_graph_add_param_mod_edge(handle, m.srcHash, m.srcPort, m.dstHash, m.paramId);
    for (auto &p : plan.params)
        soemdsp_graph_set_param(handle, p.hash, p.paramId, p.value);
    // Saved values are the heard values. Do not chase engine defaults (PolyBLEP
    // frequency starts at 220 Hz and amplitude at 1 — that is the DC saw).
    soemdsp_graph_snap_controls(handle);
    if (soemdsp_graph_compile(handle) != 0)
        return false;
    hostInputHashes = plan.hostInputHashes;
    if (hostInputHashes.empty())
        hostInputHashes.push_back(inHash);
    slots = plan.slots;
    displayPlan = std::make_shared<GraphPlan>(plan);
    ready = true;
    status = plan.name + "  (" + std::to_string(plan.added) + " native nodes, " +
             std::to_string(plan.skipped) + " skipped)";
    lastStatus = status;
    return true;
}

void SandboxGraph::setup(float sr)
{
    sampleRate = (sr > 1.f) ? sr : 44100.f;
    GraphPlan def;
    def.name = "default";
    def.nodes.push_back({inHash, kSoemTypeAudioInput});
    def.nodes.push_back({gainHash, kSoemTypeGain});
    def.nodes.push_back({outHash, kSoemTypeOutput});
    def.conns.push_back({inHash, gainHash, kSoemPortLeft, kSoemPortLeft});
    def.conns.push_back({inHash, gainHash, kSoemPortRight, kSoemPortRight});
    def.conns.push_back({gainHash, outHash, kSoemPortLeft, kSoemPortLeft});
    def.conns.push_back({gainHash, outHash, kSoemPortRight, kSoemPortRight});
    def.params.push_back({outHash, kSoemParamVolumeDb, 0.0});
    def.hostInputHashes.push_back(inHash);
    def.added = 3;
    applyPlan(def);
    status = "AudioInput -> Gain (0 dB) -> Output";
    lastStatus = status;
}

void SandboxGraph::queuePlan(std::shared_ptr<GraphPlan> plan)
{
    std::lock_guard<std::mutex> g(pendingMutex);
    pendingPlan = std::move(plan);
    hasPendingPlan.store(true);
}

void SandboxGraph::process(const float *inL, const float *inR, float *destL, float *destR, int n)
{
    if (hasPendingPlan.load())
    {
        std::shared_ptr<GraphPlan> plan;
        {
            std::lock_guard<std::mutex> g(pendingMutex);
            plan = pendingPlan;
            pendingPlan.reset();
            hasPendingPlan.store(false);
        }
        if (plan)
            applyPlan(*plan);
    }

    if (!ready || handle <= 0 || n < 1)
    {
        if (destL)
            std::memset(destL, 0, sizeof(float) * (size_t)n);
        if (destR)
            std::memset(destR, 0, sizeof(float) * (size_t)n);
        return;
    }

    int maxN = soemdsp_graph_max_block_frames();
    if (maxN < 1)
        maxN = 128;
    int offset = 0;
    while (offset < n)
    {
        int chunk = n - offset;
        if (chunk > maxN)
            chunk = maxN;

        auto fillHost = [&](uint32_t hash) {
            double *pL = soemdsp_graph_node_port_ptr(handle, hash, kSoemPortLeft);
            double *pR = soemdsp_graph_node_port_ptr(handle, hash, kSoemPortRight);
            double *pM = soemdsp_graph_node_port_ptr(handle, hash, kSoemPortMono);
            for (int i = 0; i < chunk; ++i)
            {
                const float l = inL ? inL[offset + i] : 0.f;
                const float r = inR ? inR[offset + i] : l;
                if (pL)
                    pL[i] = (double)l;
                if (pR)
                    pR[i] = (double)r;
                if (pM)
                    pM[i] = 0.5 * ((double)l + (double)r);
            }
        };
        if (hostInputHashes.empty())
            fillHost(inHash);
        else
            for (auto h : hostInputHashes)
                fillHost(h);

        soemdsp_graph_process_block(handle, chunk);

        double *oL = soemdsp_graph_block_output_left_ptr(handle);
        double *oR = soemdsp_graph_block_output_right_ptr(handle);
        for (int i = 0; i < chunk; ++i)
        {
            const float ol = oL ? (float)oL[i] : 0.f;
            const float or_ = oR ? (float)oR[i] : ol;
            if (destL)
                destL[offset + i] = ol;
            if (destR)
                destR[offset + i] = or_;
        }
        offset += chunk;
    }
}

void SandboxGraph::setHostTransport(double tempoBpm, double positionSamples, bool lockPosition)
{
    if (handle > 0)
        soemdsp_graph_set_host_transport(handle, tempoBpm, positionSamples, lockPosition ? 1 : 0);
}

void SandboxGraph::setGravityWalkerChunks(double c0, double c1, double c2)
{
    if (handle > 0)
        soemdsp_graph_set_gravity_walker_chunks(handle, c0, c1, c2);
}

void SandboxGraph::setSlot(int slot, double unit01)
{
    if (handle <= 0)
        return;
    for (auto &s : slots)
    {
        if (s.slot != slot)
            continue;
        double u = unit01;
        if (u < 0)
            u = 0;
        if (u > 1)
            u = 1;
        double v = s.minV + (s.maxV - s.minV) * u;
        uint32_t h = s.dstHash ? s.dstHash : s.knobHash;
        if (h)
            soemdsp_graph_set_param(handle, h, s.paramId, v);
        break;
    }
}

} // namespace soemdsp_ap
