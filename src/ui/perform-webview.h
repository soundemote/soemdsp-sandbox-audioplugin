#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <memory>
#include <string>

namespace soemdsp_ap::ui
{
struct PluginEditor;

// Packed perform page. Sandbox perform.html replaces resources/perform/perform.html
// when that file exists at configure time. Not a second face painter.
struct PerformView : juce::Component
{
    explicit PerformView(PluginEditor &editor);
    ~PerformView() override;

    void resized() override;
    bool isLive() const { return browser != nullptr; }

    void pushPatchJson(const juce::String &json);
    void pushSlot(int pluginId, float value);

    PluginEditor &editor;
    std::unique_ptr<juce::WebBrowserComponent> browser;
    bool pageReady{false};
    juce::String pendingJson;
};
} // namespace soemdsp_ap::ui
