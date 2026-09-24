#include "engine/sandbox_graph.h"
#include "soemdsp_native_ids.h"
#include "native_wire_tables.gen.h"

#include <juce_core/juce_core.h>
#include <unordered_map>
#include <cmath>
#include <algorithm>

namespace soemdsp_ap
{

static uint32_t fnv1a32(const std::string &s)
{
    uint32_t hash = 2166136261u;
    for (unsigned char c : s)
    {
        hash ^= (uint32_t)c;
        hash *= 16777619u;
    }
    return hash;
}

static int mapPort(const juce::String &port, const juce::String &type, bool destination)
{
    const auto p = port.trim().toLowerCase();
    const auto t = type.trim();
    return soemdsp_wire_port(t.toRawUTF8(), p.toRawUTF8(), destination);
}

static int mapParamId(const juce::String &type, const juce::String &key)
{
    return soemdsp_wire_param_id(type.trim().toRawUTF8(), key.trim().toRawUTF8());
}

static uint32_t parseHexColour(const juce::var &v, uint32_t fallback)
{
    auto s = v.toString().trim();
    if (s.startsWithChar('#') && s.length() == 7)
        return (uint32_t)s.substring(1).getHexValue32() | 0xff000000u;
    if (s.startsWithChar('#') && s.length() == 4)
    {
        auto r = s.substring(1, 2);
        auto g = s.substring(2, 3);
        auto b = s.substring(3, 4);
        auto hex = r + r + g + g + b + b;
        return (uint32_t)hex.getHexValue32() | 0xff000000u;
    }
    return fallback;
}

static float clampf(double v, float lo, float hi, float fb)
{
    if (!std::isfinite(v))
        return fb;
    return std::clamp((float)v, lo, hi);
}

static void fillFaceLook(DrawNode &dn, juce::DynamicObject *n)
{
    auto *face = n->getProperty("traceDisplaySettings").getDynamicObject();
    auto get = [&](const char *key) -> juce::var {
        return face ? face->getProperty(key) : juce::var();
    };
    auto *meta = n->getProperty("paramMeta").getDynamicObject();
    juce::DynamicObject *offsetMeta = nullptr;
    if (meta)
    {
        auto om = meta->getProperty("offset");
        offsetMeta = om.getDynamicObject();
    }

    if (dn.ctrl == CtrlFace::Knob)
    {
        FaceLook L;
        L.bg = parseHexColour(get("background"), 0xff000000u);
        L.arcFill = parseHexColour(get("arcFill"), 0xfff1b84bu);
        L.arcTrack = parseHexColour(get("arcTrack"), 0xff3a3428u);
        L.textColor = 0xffcfdde5u;
        L.rotationDegrees = clampf((double)get("rotationDegrees"), 0.f, 1440.f, 270.f);
        L.dialSize = clampf((double)get("dialSize"), 0.f, 1.f, 1.f);
        L.labelSize = clampf((double)get("labelSize"), 0.f, 1.f, 0.2f);
        L.valueSize = clampf((double)get("valueSize"), 0.f, 1.f, 0.2f);
        L.innerRadius = clampf((double)get("innerRadius"), 0.f, 0.95f, 0.7f);
        L.decimals = (int)std::lround(clampf((double)get("decimals"), 0.f, 6.f, 2.f));
        auto lp = get("labelPosition").toString().toStdString();
        auto vp = get("valuePosition").toString().toStdString();
        if (!lp.empty())
            L.labelPos = lp;
        if (!vp.empty())
            L.valuePos = vp;
        L.displayName = get("labelText").toString().toStdString();
        if (L.displayName.empty())
            L.displayName = dn.label;
        dn.look = std::move(L);
    }
    else if (dn.ctrl == CtrlFace::Slider)
    {
        FaceLook L;
        L.bg = parseHexColour(get("background"), 0xff000000u);
        L.arcTrack = parseHexColour(get("arcTrack"), 0xff1a2226u);
        L.sliderColor = parseHexColour(get("sliderColor"), 0xff5491abu);
        L.numberColor = parseHexColour(get("sliderNumberColor"), 0xffffffffu);
        L.textColor = parseHexColour(get("sliderTextColor"), 0xffcad3d8u);
        L.unitColor = parseHexColour(get("sliderUnitColor"), 0xff7fc7d9u);
        L.sliderLength = clampf((double)get("sliderLength"), 0.f, 1.f, 1.f);
        L.sliderHeight = clampf((double)get("sliderHeight"), 0.f, 1.f, 0.22f);
        L.sliderPadding = clampf((double)get("sliderPadding"), 0.f, 1.f, 0.f);
        L.sliderRounding = clampf((double)get("sliderRounding"), 0.f, 1.f, 0.5f);
        L.decimals = (int)std::lround(clampf((double)get("decimals"), 0.f, 6.f, 2.f));
        auto al = get("sliderAlign").toString().toStdString();
        if (!al.empty())
            L.sliderAlign = al;
        auto cs = get("sliderCornerShape").toString().toStdString();
        if (!cs.empty())
            L.cornerShape = cs;
        auto la = get("sliderLabelAlign").toString().toStdString();
        auto na = get("sliderNumberAlign").toString().toStdString();
        auto ua = get("sliderUnitAlign").toString().toStdString();
        if (!la.empty())
            L.labelAlign = la;
        if (!na.empty())
            L.numberAlign = na;
        if (!ua.empty())
            L.unitAlign = ua;
        L.labelPad = clampf((double)get("sliderLabelPadding"), 0.f, 1.f, 0.04f);
        L.labelScale = clampf((double)get("sliderLabelScale"), 0.f, 1.f, 0.22f);
        L.numberPad = clampf((double)get("sliderNumberPadding"), 0.f, 1.f, 0.f);
        L.numberScale = clampf((double)get("sliderNumberScale"), 0.f, 1.f, 0.22f);
        L.unitPad = clampf((double)get("sliderUnitPadding"), 0.f, 1.f, 0.04f);
        L.unitScale = clampf((double)get("sliderUnitScale"), 0.f, 1.f, 0.18f);
        L.showLabel = get("sliderShowLabel").isVoid() || (bool)get("sliderShowLabel");
        L.showNumber = get("sliderShowNumber").isVoid() || (bool)get("sliderShowNumber");
        L.showUnit = get("sliderShowUnit").isVoid() || (bool)get("sliderShowUnit");
        L.displayName = get("labelText").toString().toStdString();
        if (L.displayName.empty())
            L.displayName = dn.label;
        if (offsetMeta && offsetMeta->hasProperty("unit"))
            L.unit = offsetMeta->getProperty("unit").toString().toStdString();
        dn.look = std::move(L);
    }
    else if (dn.ctrl == CtrlFace::Toggle || dn.ctrl == CtrlFace::Momentary)
    {
        FaceLook L;
        L.bg = parseHexColour(get("background"), 0xff101418u);
        L.strokeScale = clampf((double)get("strokeScale"), 0.f, 1.f, 0.06f);
        L.buttonScale = clampf((double)get("buttonScale"), 0.f, 1.f, 1.f);
        L.textScale = clampf((double)get("textScale"), 0.f, 1.f, 0.72f);
        L.padding = clampf((double)get("buttonPadding"), 0.f, 1.f, 0.f);
        auto ba = get("buttonAlign").toString().toStdString();
        if (!ba.empty())
            L.buttonAlign = ba;
        auto la = get("labelAlign").toString().toStdString();
        if (!la.empty())
            L.labelAlign = la;
        L.labelPad = clampf((double)get("labelPadding"), 0.f, 1.f, 0.035f);
        L.labelScale = clampf((double)get("labelScale"), 0.f, 1.f, 0.22f);
        L.showLabel = get("buttonShowLabel").isVoid() || (bool)get("buttonShowLabel");
        L.stroke = parseHexColour(get("strokeColor"), 0xff5c5071u);
        L.inactive = parseHexColour(get("inactiveColor"), 0xff1a2228u);
        L.active = parseHexColour(get("activeColor"), 0xff2f8f86u);
        L.hover = parseHexColour(get("hoverColor"), 0xff89bfc2u);
        L.textColor = parseHexColour(get("textColor"), 0xfff4f7f8u);
        L.displayName = get("labelText").toString().toStdString();
        const bool momentary = dn.ctrl == CtrlFace::Momentary;
        if (get("offText").isVoid())
            L.offLabel = momentary ? "Gate" : "Off";
        else
            L.offLabel = get("offText").toString().toStdString();
        if (get("onText").isVoid())
            L.onLabel = momentary ? "Gate" : "On";
        else
            L.onLabel = get("onText").toString().toStdString();
        dn.look = std::move(L);
    }
}

static juce::var unwrapPatch(const juce::var &root)
{
    if (!root.isObject())
        return root;
    auto *obj = root.getDynamicObject();
    if (!obj)
        return root;
    auto kind = obj->getProperty("kind").toString();
    if (kind == "sandbox_patch")
    {
        auto inner = obj->getProperty("patch_data");
        if (inner.isString())
            return juce::JSON::parse(inner.toString());
        if (inner.isObject())
            return inner;
    }
    return root;
}

GraphPlan parseSandboxPatchJson(const std::string &jsonText, const std::string &displayName)
{
    GraphPlan plan;
    plan.name = displayName;
    auto parsed = juce::JSON::parse(juce::String(jsonText));
    if (parsed.isVoid())
    {
        plan.error = "JSON parse failed";
        return plan;
    }
    auto root = unwrapPatch(parsed);
    auto *obj = root.getDynamicObject();
    if (!obj)
    {
        plan.error = "JSON root is not an object";
        return plan;
    }

    std::unordered_map<std::string, std::string> typeById;
    auto nodes = obj->getProperty("nodes");
    if (auto *arr = nodes.getArray())
    {
        for (auto &nv : *arr)
        {
            auto *n = nv.getDynamicObject();
            if (!n)
                continue;
            auto id = n->getProperty("id").toString().toStdString();
            auto type = n->getProperty("type").toString().toStdString();
            typeById[id] = type;
            uint32_t hash = fnv1a32(id);

            DrawNode dn;
            dn.gx = (float)n->getProperty("gx");
            dn.gy = (float)n->getProperty("gy");
            dn.wGu = n->hasProperty("widthGu") ? (float)n->getProperty("widthGu") : 8.f;
            if (n->hasProperty("heightGu"))
                dn.hGu = (float)n->getProperty("heightGu");
            else if (n->hasProperty("displayHeightGu"))
                dn.hGu = (float)n->getProperty("displayHeightGu") + 2.f;
            else
                dn.hGu = 6.f;
            dn.id = id;
            dn.type = type;
            dn.hash = hash;
            auto alias = n->getProperty("alias").toString();
            dn.label = alias.isNotEmpty() ? alias.toStdString() : type;
            if (type == "knob")
                dn.ctrl = CtrlFace::Knob;
            else if (type == "pluginSlider")
                dn.ctrl = CtrlFace::Slider;
            else if (type == "toggleButton")
                dn.ctrl = CtrlFace::Toggle;
            else if (type == "momentaryButton")
                dn.ctrl = CtrlFace::Momentary;
            dn.isKnob = (dn.ctrl == CtrlFace::Knob || dn.ctrl == CtrlFace::Slider);
            fillFaceLook(dn, n);
            int pluginId = -1;
            if (n->hasProperty("pluginId"))
            {
                pluginId = (int)std::lround((double)n->getProperty("pluginId"));
            }
            const bool bindable = dn.ctrl != CtrlFace::None;
            if (bindable && pluginId >= 0 && pluginId < 32)
            {
                dn.slot = pluginId;
                SlotBind sb;
                sb.slot = pluginId;
                sb.folder = n->getProperty("pluginFolder").toString().toStdString();
                sb.name = n->getProperty("pluginName").toString().toStdString();
                if (sb.name.empty())
                    sb.name = dn.look.displayName;
                if (sb.name.empty())
                    sb.name = dn.label;
                sb.knobHash = hash;
                sb.paramId = soemdsp_native_param_id("offset");
                if (sb.paramId < 0)
                    sb.paramId = 71;
                sb.minV = 0;
                sb.maxV = 1;
                auto *meta = n->getProperty("paramMeta").getDynamicObject();
                if (meta)
                {
                    auto om = meta->getProperty("offset");
                    if (auto *oo = om.getDynamicObject())
                    {
                        if (oo->hasProperty("min"))
                            sb.minV = (double)oo->getProperty("min");
                        if (oo->hasProperty("max"))
                            sb.maxV = (double)oo->getProperty("max");
                    }
                }
                auto *po0 = n->getProperty("params").getDynamicObject();
                if (po0 && po0->hasProperty("offset") && sb.maxV != sb.minV)
                {
                    double off = (double)po0->getProperty("offset");
                    sb.initial01 = (off - sb.minV) / (sb.maxV - sb.minV);
                    if (sb.initial01 < 0)
                        sb.initial01 = 0;
                    if (sb.initial01 > 1)
                        sb.initial01 = 1;
                }
                plan.slots.push_back(sb);
            }
            plan.drawNodes.push_back(dn);

            int typeId = soemdsp_wire_type_id(type.c_str());
            if (typeId <= 0)
            {
                plan.skipped += 1;
                continue;
            }
            GraphNodeSpec spec;
            spec.hash = hash;
            spec.typeId = typeId;
            plan.nodes.push_back(spec);
            plan.added += 1;
            if (type == "audioInput" || type == "input")
                plan.hostInputHashes.push_back(spec.hash);

            if (auto *meta = n->getProperty("paramMeta").getDynamicObject())
            {
                for (auto &prop : meta->getProperties())
                {
                    int pid = mapParamId(type, prop.name.toString());
                    auto *pm = prop.value.getDynamicObject();
                    if (pid < 0 || pm == nullptr)
                        continue;
                    GraphDomainSpec d;
                    d.hash = spec.hash;
                    d.paramId = pid;
                    d.minV = pm->hasProperty("min") ? (float)(double)pm->getProperty("min") : 0.f;
                    d.maxV = pm->hasProperty("max") ? (float)(double)pm->getProperty("max") : 0.f;
                    const bool realMod = (bool)pm->getProperty("outputDomain");
                    const bool modClamp = !pm->hasProperty("modClamp") || (bool)pm->getProperty("modClamp");
                    int flags = 0;
                    if ((bool)pm->getProperty("wraparound"))
                        flags |= 1;
                    if (realMod)
                        flags |= 16;
                    if (!modClamp || realMod)
                        flags |= 8;
                    else
                        flags |= 2;
                    d.flags = flags;
                    plan.domains.push_back(d);
                }
            }

            auto params = n->getProperty("params");
            if (auto *po = params.getDynamicObject())
            {
                for (auto &prop : po->getProperties())
                {
                    int pid = mapParamId(type, prop.name.toString());
                    if (pid < 0)
                        continue;
                    GraphParamSpec ps;
                    ps.hash = spec.hash;
                    ps.paramId = pid;
                    ps.value = (double)prop.value;
                    plan.params.push_back(ps);
                }
            }
        }
    }

    auto conns = obj->getProperty("connections");
    if (auto *arr = conns.getArray())
    {
        for (auto &cv : *arr)
        {
            auto *c = cv.getDynamicObject();
            if (!c)
                continue;
            auto srcId = c->getProperty("sourceNode").toString().toStdString();
            auto dstId = c->getProperty("destinationNode").toString().toStdString();
            GraphConnSpec cs;
            cs.srcHash = fnv1a32(srcId);
            cs.dstHash = fnv1a32(dstId);
            cs.srcPort = mapPort(c->getProperty("sourcePort").toString(), typeById[srcId], false);
            cs.dstPort = mapPort(c->getProperty("destinationPort").toString(), typeById[dstId], true);
            plan.conns.push_back(cs);
            plan.drawWires.push_back({cs.srcHash, cs.dstHash});
        }
    }

    auto mods = obj->getProperty("modulations");
    if (auto *arr = mods.getArray())
    {
        for (auto &mv : *arr)
        {
            auto *m = mv.getDynamicObject();
            if (!m)
                continue;
            auto srcId = m->getProperty("sourceNode").toString().toStdString();
            auto dstId = m->getProperty("destinationNode").toString().toStdString();
            auto dstParam = m->getProperty("destinationParam").toString().toStdString();
            int pid = mapParamId(typeById[dstId], juce::String(dstParam));
            if (pid >= 0 && typeById.count(srcId) && typeById.count(dstId))
            {
                GraphModSpec ms;
                ms.srcHash = fnv1a32(srcId);
                ms.dstHash = fnv1a32(dstId);
                ms.srcPort = mapPort(m->getProperty("sourcePort").toString(), typeById[srcId], false);
                ms.paramId = pid;
                plan.mods.push_back(ms);
            }
            uint32_t srcHash = fnv1a32(srcId);
            for (auto &sb : plan.slots)
            {
                if (sb.knobHash != srcHash)
                    continue;
                sb.dstHash = fnv1a32(dstId);
                int pid = mapParamId(typeById[dstId], juce::String(dstParam));
                if (pid >= 0)
                    sb.paramId = pid;
            }
        }
    }

    if (plan.nodes.empty())
        plan.error = "No native-graph nodes in this JSON (or types not in the engine map)";
    return plan;
}

} // namespace soemdsp_ap
