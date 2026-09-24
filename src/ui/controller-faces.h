#pragma once

#include "engine/sandbox_graph.h"
#include <juce_graphics/juce_graphics.h>
#include <algorithm>
#include <cmath>
#include <string>

namespace soemdsp_ap::ui
{

inline juce::Colour faceColour(uint32_t argb)
{
    return juce::Colour(argb);
}

inline juce::Colour lerpColour(juce::Colour a, juce::Colour b, float t)
{
    t = std::clamp(t, 0.f, 1.f);
    return a.interpolatedWith(b, t);
}

inline void fillRoundish(juce::Graphics &g, juce::Rectangle<float> r, float rounding01,
                         const std::string &shape)
{
    rounding01 = std::clamp(rounding01, 0.f, 1.f);
    const float m = std::min(r.getWidth(), r.getHeight());
    const bool pill = (shape == "square" || shape == "round" || shape == "pill");
    const float rad = rounding01 * m * (pill ? 0.5f : 0.38f);
    g.fillRoundedRectangle(r, rad);
}

inline juce::Rectangle<float> pinPoint(juce::Rectangle<float> box, const std::string &align, float pad)
{
    pad = std::clamp(pad, 0.f, 1.f);
    const float xPad = pad * box.getWidth();
    const float yPad = pad * box.getHeight();
    float x = box.getX() + xPad;
    float y = box.getY() + yPad;
    if (align == "top" || align == "mid" || align == "bottom")
        x = box.getCentreX();
    else if (align == "topright" || align == "midright" || align == "bottomright")
        x = box.getRight() - xPad;
    if (align == "midleft" || align == "mid" || align == "midright")
        y = box.getCentreY();
    else if (align == "bottomleft" || align == "bottom" || align == "bottomright")
        y = box.getBottom() - yPad;
    return {x, y, 0.f, 0.f};
}

inline juce::Justification pinJust(const std::string &align)
{
    const bool left = align == "topleft" || align == "midleft" || align == "bottomleft";
    const bool right = align == "topright" || align == "midright" || align == "bottomright";
    const bool top = align == "topleft" || align == "top" || align == "topright";
    const bool bot = align == "bottomleft" || align == "bottom" || align == "bottomright";
    int flags = 0;
    flags |= left ? juce::Justification::left
                  : (right ? juce::Justification::right : juce::Justification::horizontallyCentred);
    flags |= top ? juce::Justification::top
                 : (bot ? juce::Justification::bottom : juce::Justification::verticallyCentred);
    return juce::Justification(flags);
}

inline void drawPinnedText(juce::Graphics &g, juce::Rectangle<float> face, const std::string &text,
                           const std::string &align, float pad, float scale, juce::Colour colour)
{
    if (text.empty())
        return;
    const float minSide = std::max(8.f, std::min(face.getWidth(), face.getHeight()));
    const float fs = std::clamp(scale, 0.f, 1.f) * minSide;
    if (fs < 2.f)
        return;
    g.setColour(colour);
    g.setFont(juce::FontOptions(fs));
    auto origin = pinPoint(face, align, pad);
    const float w = fs * (float)std::max<size_t>(1, text.size()) * 0.7f + fs;
    const float h = fs * 1.2f;
    juce::Rectangle<float> box;
    const auto just = pinJust(align);
    if (just.getFlags() & juce::Justification::right)
        box = {origin.getX() - w, origin.getY() - ((just.getFlags() & juce::Justification::bottom) ? h : 0.f),
               w, h};
    else if (just.getFlags() & juce::Justification::horizontallyCentred)
        box = {origin.getX() - w * 0.5f,
               origin.getY() - ((just.getFlags() & juce::Justification::bottom)
                                    ? h
                                    : ((just.getFlags() & juce::Justification::top) ? 0.f : h * 0.5f)),
               w, h};
    else
        box = {origin.getX(),
               origin.getY() - ((just.getFlags() & juce::Justification::bottom)
                                    ? h
                                    : ((just.getFlags() & juce::Justification::top) ? 0.f : h * 0.5f)),
               w, h};
    if (just.getFlags() & juce::Justification::verticallyCentred)
        box.setY(origin.getY() - h * 0.5f);
    if (just.getFlags() & juce::Justification::top)
        box.setY(origin.getY());
    g.drawFittedText(text, box.toNearestInt(), just, 1);
}

inline std::string formatBias(float value, int decimals)
{
    decimals = std::clamp(decimals, 0, 6);
    return juce::String(value, decimals).toStdString();
}

inline juce::Rectangle<float> pinBoxPx(juce::Rectangle<float> face, float w, float h,
                                       const std::string &align, float pad01)
{
    pad01 = std::clamp(pad01, 0.f, 1.f);
    const float padX = pad01 * face.getWidth();
    const float padY = pad01 * face.getHeight();
    w = std::max(0.f, w);
    h = std::max(0.f, h);
    float x = face.getX() + padX;
    float y = face.getY() + padY;
    if (align == "top" || align == "mid" || align == "bottom")
        x = face.getCentreX() - w * 0.5f;
    else if (align == "topright" || align == "midright" || align == "bottomright")
        x = face.getRight() - padX - w;
    if (align == "midleft" || align == "mid" || align == "midright")
        y = face.getCentreY() - h * 0.5f;
    else if (align == "bottomleft" || align == "bottom" || align == "bottomright")
        y = face.getBottom() - padY - h;
    return {x, y, w, h};
}

inline juce::Rectangle<float> sliderBarRect(juce::Rectangle<float> face, const FaceLook &look)
{
    const float pad = std::clamp(look.sliderPadding, 0.f, 1.f);
    auto inner = face.reduced(pad * face.getWidth(), pad * face.getHeight());
    const float w = std::clamp(look.sliderLength, 0.f, 1.f) * inner.getWidth();
    const float h = std::clamp(look.sliderHeight, 0.f, 1.f) * inner.getHeight();
    float y = inner.getY();
    if (look.sliderAlign == "mid")
        y = inner.getCentreY() - h * 0.5f;
    else if (look.sliderAlign == "bottom")
        y = inner.getBottom() - h;
    return {inner.getX(), y, w, h};
}

inline void drawKnobFace(juce::Graphics &g, juce::Rectangle<float> box, const DrawNode &n, float u)
{
    const auto &L = n.look;
    g.setColour(faceColour(L.bg));
    g.fillRect(box);
    const float minSide = std::min(box.getWidth(), box.getHeight());
    const float dial = std::clamp(L.dialSize, 0.05f, 1.f) * minSide * 0.92f;
    auto c = box.getCentre();
    if (L.labelPos == "above")
        c.y += minSide * 0.06f;
    const float rad = dial * 0.5f;
    const float inner = std::clamp(L.innerRadius, 0.f, 0.95f);
    juce::Path track, fill;
    const float span = juce::degreesToRadians(std::clamp(L.rotationDegrees, 0.f, 1440.f));
    const float start = juce::degreesToRadians(-90.f) - span * 0.5f;
    const float end = start + span;
    const float fillEnd = start + span * std::clamp(u, 0.f, 1.f);
    const float thick = std::max(2.f, (1.f - inner) * rad);
    track.addCentredArc(c.x, c.y, rad, rad, 0.f, start, end, true);
    g.setColour(faceColour(L.arcTrack));
    g.strokePath(track, juce::PathStrokeType(thick, juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
    if (u > 0.001f)
    {
        fill.addCentredArc(c.x, c.y, rad, rad, 0.f, start, fillEnd, true);
        g.setColour(faceColour(L.arcFill));
        g.strokePath(fill, juce::PathStrokeType(thick, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    }
    const std::string title = L.displayName.empty() ? n.label : L.displayName;
    if (L.labelPos != "off" && !title.empty())
    {
        const float fs = std::clamp(L.labelSize, 0.04f, 1.f) * minSide;
        g.setColour(faceColour(L.textColor));
        g.setFont(juce::FontOptions(fs));
        auto tb = box;
        if (L.labelPos == "below")
            tb = box.removeFromBottom(fs * 1.4f);
        else
            tb = box.removeFromTop(fs * 1.4f);
        if (L.labelPos == "mid")
            tb = juce::Rectangle<float>(c.x - rad, c.y - fs, rad * 2.f, fs * 1.2f);
        g.drawFittedText(title, tb.toNearestInt(), juce::Justification::centred, 1);
    }
    if (L.valuePos != "off")
    {
        const float fs = std::clamp(L.valueSize, 0.04f, 1.f) * minSide;
        g.setColour(juce::Colours::white);
        g.setFont(juce::FontOptions(fs));
        auto vb = juce::Rectangle<float>(c.x - rad, c.y - fs * 0.55f, rad * 2.f, fs * 1.1f);
        if (L.valuePos == "above")
            vb.setY(c.y - rad + 2.f);
        else if (L.valuePos == "below")
            vb.setY(c.y + rad - fs * 1.2f);
        g.drawFittedText(formatBias(u, L.decimals), vb.toNearestInt(), juce::Justification::centred, 1);
    }
}

inline void drawSliderFace(juce::Graphics &g, juce::Rectangle<float> box, const DrawNode &n, float u)
{
    const auto &L = n.look;
    g.setColour(faceColour(L.bg));
    g.fillRect(box);
    auto bar = sliderBarRect(box, L);
    if (bar.getWidth() > 1.f && bar.getHeight() > 1.f)
    {
        g.setColour(faceColour(L.arcTrack));
        fillRoundish(g, bar, L.sliderRounding, L.cornerShape);
        auto filled = bar.withWidth(bar.getWidth() * std::clamp(u, 0.f, 1.f));
        if (filled.getWidth() > 0.5f)
        {
            g.setColour(faceColour(L.sliderColor));
            fillRoundish(g, filled, L.sliderRounding, L.cornerShape);
        }
    }
    const std::string title = L.displayName.empty() ? n.label : L.displayName;
    if (L.showLabel)
        drawPinnedText(g, box, title, L.labelAlign, L.labelPad, L.labelScale, faceColour(L.textColor));
    if (L.showNumber)
        drawPinnedText(g, box, formatBias(u, L.decimals), L.numberAlign, L.numberPad, L.numberScale,
                       faceColour(L.numberColor));
    if (L.showUnit && !L.unit.empty())
        drawPinnedText(g, box, L.unit, L.unitAlign, L.unitPad, L.unitScale, faceColour(L.unitColor));
}

inline void drawButtonFace(juce::Graphics &g, juce::Rectangle<float> box, const DrawNode &n, float u,
                           bool hovered)
{
    const auto &L = n.look;
    g.setColour(faceColour(L.bg));
    g.fillRect(box);
    const float scale = std::clamp(L.buttonScale, 0.f, 1.f);
    auto btn = pinBoxPx(box, box.getWidth() * scale, box.getHeight() * scale, L.buttonAlign,
                        std::clamp(L.padding, 0.f, 1.f));
    auto fill = lerpColour(faceColour(L.inactive), faceColour(L.active), std::clamp(u, 0.f, 1.f));
    if (hovered)
        fill = lerpColour(fill, faceColour(L.hover), 0.35f);
    g.setColour(fill);
    g.fillRoundedRectangle(btn, 4.f);
    const float sw = std::clamp(L.strokeScale, 0.f, 1.f) * std::min(btn.getWidth(), btn.getHeight()) * 0.5f;
    if (sw > 0.4f)
    {
        g.setColour(faceColour(L.stroke));
        g.drawRoundedRectangle(btn, 4.f, sw);
    }
    const bool on = u >= 0.5f;
    const std::string text = on ? L.onLabel : L.offLabel;
    if (!text.empty())
    {
        const float fs = std::clamp(L.textScale, 0.04f, 1.f) * std::min(btn.getWidth(), btn.getHeight());
        g.setColour(faceColour(L.textColor ? L.textColor : 0xfff4f7f8u));
        g.setFont(juce::FontOptions(fs));
        g.drawFittedText(text, btn.toNearestInt(), juce::Justification::centred, 1);
    }
    const std::string title = L.displayName;
    if (L.showLabel && !title.empty())
        drawPinnedText(g, box, title, L.labelAlign, L.labelPad, L.labelScale, faceColour(L.textColor));
}

inline void drawControllerFace(juce::Graphics &g, juce::Rectangle<float> box, const DrawNode &n,
                               float unit01, bool hovered)
{
    switch (n.ctrl)
    {
    case CtrlFace::Knob:
        drawKnobFace(g, box, n, unit01);
        break;
    case CtrlFace::Slider:
        drawSliderFace(g, box, n, unit01);
        break;
    case CtrlFace::Toggle:
    case CtrlFace::Momentary:
        drawButtonFace(g, box, n, unit01, hovered);
        break;
    default:
        g.setColour(juce::Colour(0xff1c2a32));
        g.fillRoundedRectangle(box, 4.f);
        g.setColour(juce::Colour(0xff7fc7d9));
        g.drawRoundedRectangle(box, 4.f, 1.f);
        g.setColour(juce::Colours::white.withAlpha(0.9f));
        g.setFont(juce::FontOptions(std::clamp(box.getHeight() * 0.28f, 9.f, 14.f)));
        g.drawFittedText(n.label, box.reduced(4.f).toNearestInt(), juce::Justification::centred, 2);
        break;
    }
}

} // namespace soemdsp_ap::ui
