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

#include "plugin-editor.h"
#include "perform-webview.h"

#include "sst/plugininfra/version_information.h"
#include "sst/clap_juce_shim/menu_helper.h"
#include "sst/plugininfra/misc_platform.h"

#include "sst/jucegui/components/Label.h"

#include "presets/preset-manager.h"
#include "preset-data-binding.h"
#include "patch-data-bindings.h"
#include "main-panel.h"
#include "controller-faces.h"
#include <algorithm>
#include <unordered_map>

namespace soemdsp_ap::ui
{
struct IdleTimer : juce::Timer
{
    PluginEditor &editor;
    IdleTimer(PluginEditor &e) : editor(e) {}
    void timerCallback() override { editor.idle(); }
};

namespace jstl = sst::jucegui::style;
using sheet_t = jstl::StyleSheet;
static constexpr sheet_t::Class PatchMenu("SoemDSP.patch-menu");

PluginEditor::PluginEditor(Engine::audioToUIQueue_t &atou, Engine::mainToAudioQueue_T &utoa,
                           const clap_host_t *h)
    : jcmp::WindowPanel(true), audioToUI(atou), mainToAudio(utoa), clapHost(h)
{
    setTitle("soemdsp");
    setAccessible(true);
    sst::jucegui::style::StyleSheet::initializeStyleSheets([]() {});

    sheet_t::addClass(PatchMenu).withBaseClass(jcmp::JogUpDownButton::Styles::styleClass);

    setStyle(sst::jucegui::style::StyleSheet::getBuiltInStyleSheet(
        sst::jucegui::style::StyleSheet::DARK));

    style()->setFont(
        PatchMenu, jcmp::MenuButton::Styles::labelfont,
        style()
            ->getFont(jcmp::MenuButton::Styles::styleClass, jcmp::MenuButton::Styles::labelfont)
            .withHeight(18));

    mainPanel = std::make_unique<MainPanel>(*this);
    mainPanel->hasHamburger = false;
    // Synth knobs hidden — v1 is an audio effect (sandbox Gain through).
    addChildComponent(*mainPanel);
    mainPanel->setVisible(false);

    auto startMsg = Engine::MainToAudioMsg{Engine::MainToAudioMsg::REQUEST_REFRESH};
    mainToAudio.push(startMsg);
    requestParamsFlush();

    idleTimer = std::make_unique<IdleTimer>(*this);
    idleTimer->startTimer(1000. / 60.);

    toolTip = std::make_unique<jcmp::ToolTip>();
    addChildComponent(*toolTip);

    presetManager = std::make_unique<presets::PresetManager>(clapHost);
    presetManager->onPresetLoaded = [this](auto s)
    {
        this->postPatchChange(s);
        repaint();
    };

    presetDataBinding = std::make_unique<PresetDataBinding>(*presetManager, patchCopy, mainToAudio);
    presetDataBinding->setStateForDisplayName(patchCopy.name);

    presetButton = std::make_unique<jcmp::JogUpDownButton>();
    presetButton->setCustomClass(PatchMenu);
    presetButton->setSource(presetDataBinding.get());
    presetButton->onPopupMenu = [this]() { doLoadSandboxJson(); };
    addAndMakeVisible(*presetButton);
    setPatchNameDisplay();
    sst::jucegui::component_adapters::setTraversalId(presetButton.get(), 174);

    // this needs a cleanup
    defaultsProvider = std::make_unique<defaultsProvder_t>(
        presetManager->userPath, "SoemDSP", defaultName,
        [](auto e, auto b) { SQLOG("[ERROR]" << e << " " << b); });
    setSkinFromDefaults();

    lnf = std::make_unique<sst::jucegui::style::LookAndFeelManager>(this);
    lnf->setStyle(style());

    vuMeter = std::make_unique<jcmp::VUMeter>(jcmp::VUMeter::HORIZONTAL);
    addAndMakeVisible(*vuMeter);

    struct PatchCanvas : public juce::Component
    {
        PluginEditor &ed;
        explicit PatchCanvas(PluginEditor &e) : ed(e) {}
        void paint(juce::Graphics &g) override
        {
            g.fillAll(juce::Colour(0xff101418));
            auto plan = ed.canvasPlan;
            if (!plan || plan->drawNodes.empty())
            {
                g.setColour(juce::Colours::white.withAlpha(0.5f));
                g.drawFittedText("Load sandbox JSON... from the preset menu\n"
                                 "to see this patch's canvas (read-only).\n"
                                 "Knobs on the canvas map to DAW params 1-32.",
                                 getLocalBounds().reduced(16), juce::Justification::centred, 6);
                return;
            }
            float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
            const float gu = 28.f;
            for (auto &n : plan->drawNodes)
            {
                minx = std::min(minx, n.gx);
                miny = std::min(miny, n.gy);
                maxx = std::max(maxx, n.gx + n.wGu);
                maxy = std::max(maxy, n.gy + n.hGu);
            }
            float dw = std::max(8.f, maxx - minx);
            float dh = std::max(8.f, maxy - miny);
            auto r = getLocalBounds().toFloat().reduced(12.f);
            float sx = r.getWidth() / (dw * gu);
            float sy = r.getHeight() / (dh * gu);
            float s = std::min(sx, sy);
            auto toPx = [&](float gx, float gy) {
                return juce::Point<float>(r.getX() + (gx - minx) * gu * s,
                                          r.getY() + (gy - miny) * gu * s);
            };
            std::unordered_map<uint32_t, juce::Point<float>> center;
            for (auto &n : plan->drawNodes)
            {
                auto p = toPx(n.gx, n.gy);
                float w = n.wGu * gu * s;
                float h = n.hGu * gu * s;
                center[n.hash] = {p.x + w * 0.5f, p.y + h * 0.5f};
            }
            g.setColour(juce::Colour(0xff3a6a7a));
            for (auto &w : plan->drawWires)
            {
                auto a = center.find(w.srcHash);
                auto b = center.find(w.dstHash);
                if (a == center.end() || b == center.end())
                    continue;
                g.drawLine(a->second.x, a->second.y, b->second.x, b->second.y, 1.5f);
            }
            for (auto &n : plan->drawNodes)
            {
                auto p = toPx(n.gx, n.gy);
                float w = n.wGu * gu * s;
                float h = n.hGu * gu * s;
                auto box = juce::Rectangle<float>(p.x, p.y, w, h);
                float u = 0.f;
                if (n.slot >= 0)
                {
                    auto it = ed.patchCopy.paramMap.find((uint32_t)(1000 + n.slot));
                    if (it != ed.patchCopy.paramMap.end() && it->second)
                        u = it->second->value;
                }
                const bool hovered = (hoverSlot == n.slot && n.slot >= 0);
                drawControllerFace(g, box, n, u, hovered);
            }
        }
        void mouseMove(const juce::MouseEvent &e) override
        {
            int next = hitTestSlot(e.position, ed.canvasPlan.get());
            if (next != hoverSlot)
            {
                hoverSlot = next;
                repaint();
            }
        }
        void mouseDown(const juce::MouseEvent &e) override
        {
            auto plan = ed.canvasPlan;
            if (!plan)
                return;
            hitNode = hitTestNode(e.position, plan.get());
            lastY = e.position.y;
            lastX = e.position.x;
            dragValue = 0.f;
            if (hitNode < 0)
                return;
            const auto &n = plan->drawNodes[(size_t)hitNode];
            if (n.slot < 0)
                return;
            hitSlot = n.slot;
            hitCtrl = n.ctrl;
            auto it = ed.patchCopy.paramMap.find((uint32_t)(1000 + hitSlot));
            if (it != ed.patchCopy.paramMap.end() && it->second)
                dragValue = it->second->value;
            ed.gesturingParamId = (int32_t)(1000 + hitSlot);
            ed.mainToAudio.push({Engine::MainToAudioMsg::Action::BEGIN_EDIT,
                                 (uint32_t)(1000 + hitSlot)});
            if (hitCtrl == CtrlFace::Momentary)
            {
                dragValue = 1.f;
                ed.setAndSendParamValue((uint32_t)(1000 + hitSlot), 1.f, true, false);
                repaint();
            }
            else if (hitCtrl == CtrlFace::Slider)
            {
                auto box = nodeBox(n, plan.get());
                auto bar = sliderBarRect(box, n.look);
                if (bar.getWidth() > 4.f)
                    dragValue = std::clamp((e.position.x - bar.getX()) / bar.getWidth(), 0.f, 1.f);
                ed.setAndSendParamValue((uint32_t)(1000 + hitSlot), dragValue, true, false);
                repaint();
            }
        }
        void mouseDrag(const juce::MouseEvent &e) override
        {
            auto plan = ed.canvasPlan;
            if (!plan || hitSlot < 0)
                return;
            if (hitCtrl == CtrlFace::Toggle || hitCtrl == CtrlFace::Momentary)
                return;
            if (hitCtrl == CtrlFace::Slider && hitNode >= 0)
            {
                const auto &n = plan->drawNodes[(size_t)hitNode];
                auto box = nodeBox(n, plan.get());
                auto bar = sliderBarRect(box, n.look);
                if (bar.getWidth() > 4.f)
                    dragValue = std::clamp((e.position.x - bar.getX()) / bar.getWidth(), 0.f, 1.f);
            }
            else
            {
                float du = (lastY - e.position.y) / 140.f;
                lastY = e.position.y;
                dragValue = std::clamp(dragValue + du, 0.f, 1.f);
            }
            ed.setAndSendParamValue((uint32_t)(1000 + hitSlot), dragValue, true, false);
            repaint();
        }
        void mouseUp(const juce::MouseEvent &) override
        {
            if (hitSlot >= 0)
            {
                if (hitCtrl == CtrlFace::Toggle)
                {
                    float next = dragValue >= 0.5f ? 0.f : 1.f;
                    ed.setAndSendParamValue((uint32_t)(1000 + hitSlot), next, true, false);
                }
                else if (hitCtrl == CtrlFace::Momentary)
                {
                    ed.setAndSendParamValue((uint32_t)(1000 + hitSlot), 0.f, true, false);
                }
                ed.mainToAudio.push({Engine::MainToAudioMsg::Action::END_EDIT,
                                     (uint32_t)(1000 + hitSlot)});
                ed.requestParamsFlush();
            }
            ed.gesturingParamId = -1;
            hitSlot = -1;
            hitNode = -1;
            hitCtrl = CtrlFace::None;
            repaint();
        }
        int hitSlot{-1};
        int hitNode{-1};
        int hoverSlot{-1};
        CtrlFace hitCtrl{CtrlFace::None};
        float lastY{0.f};
        float lastX{0.f};
        float dragValue{0.f};
        juce::Rectangle<float> nodeBox(const DrawNode &n, GraphPlan *plan)
        {
            float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
            const float gu = 28.f;
            for (auto &m : plan->drawNodes)
            {
                minx = std::min(minx, m.gx);
                miny = std::min(miny, m.gy);
                maxx = std::max(maxx, m.gx + m.wGu);
                maxy = std::max(maxy, m.gy + m.hGu);
            }
            auto r = getLocalBounds().toFloat().reduced(12.f);
            float s = std::min(r.getWidth() / (std::max(8.f, maxx - minx) * gu),
                               r.getHeight() / (std::max(8.f, maxy - miny) * gu));
            auto p = juce::Point<float>(r.getX() + (n.gx - minx) * gu * s,
                                        r.getY() + (n.gy - miny) * gu * s);
            return {p.x, p.y, n.wGu * gu * s, n.hGu * gu * s};
        }
        int hitTestNode(juce::Point<float> pos, GraphPlan *plan)
        {
            if (!plan)
                return -1;
            for (int i = (int)plan->drawNodes.size() - 1; i >= 0; --i)
            {
                const auto &n = plan->drawNodes[(size_t)i];
                if (n.slot < 0)
                    continue;
                if (nodeBox(n, plan).contains(pos))
                    return i;
            }
            return -1;
        }
        int hitTestSlot(juce::Point<float> pos, GraphPlan *plan)
        {
            int i = hitTestNode(pos, plan);
            if (i < 0)
                return -1;
            return plan->drawNodes[(size_t)i].slot;
        }
    };
    patchCanvas = std::make_unique<PatchCanvas>(*this);
    addAndMakeVisible(*patchCanvas);
    performView = std::make_unique<PerformView>(*this);
    if (performView->isLive())
    {
        addAndMakeVisible(*performView);
        patchCanvas->setVisible(false);
    }
    else
    {
        performView.reset();
    }

    mainToAudio.push({Engine::MainToAudioMsg::EDITOR_ATTACH_DETATCH, true});
    mainToAudio.push({Engine::MainToAudioMsg::REQUEST_REFRESH, true});
    requestParamsFlush();

    auto pzf = defaultsProvider->getUserDefaultValue(Defaults::zoomLevel, 100);
    zoomFactor = pzf * 0.01;
    setTransform(juce::AffineTransform().scaled(zoomFactor));

    setSize(edWidth, edHeight);
#define DEBUG_FOCUS 0
#if DEBUG_FOCUS
    focusDebugger = std::make_unique<sst::jucegui::accessibility::FocusDebugger>(*this);
    focusDebugger->setDoFocusDebug(true);
#endif
}
PluginEditor::~PluginEditor()
{
    mainToAudio.push({Engine::MainToAudioMsg::EDITOR_ATTACH_DETATCH, false});
    idleTimer->stopTimer();
    setLookAndFeel(nullptr);
}

void PluginEditor::idle()
{
    auto aum = audioToUI.pop();
    while (aum.has_value())
    {
        if (aum->action == Engine::AudioToUIMsg::UPDATE_PARAM)
        {
            if (gesturingParamId >= 0 && aum->paramId == (uint32_t)gesturingParamId)
            {
                aum = audioToUI.pop();
                continue;
            }
            setAndSendParamValue(aum->paramId, aum->value, false);
            if (performView && aum->paramId >= 1000 && aum->paramId < 1032)
                performView->pushSlot((int)aum->paramId - 1000, aum->value);
            if (patchCanvas)
                patchCanvas->repaint();
        }
        else if (aum->action == Engine::AudioToUIMsg::UPDATE_VU)
        {
            vuMeter->setLevels(aum->value, aum->value2);
        }
        else if (aum->action == Engine::AudioToUIMsg::UPDATE_VOICE_COUNT)
        {
            SQLOG_ONCE("Implement update voice count");
        }
        else if (aum->action == Engine::AudioToUIMsg::SET_PATCH_NAME)
        {
            memset(patchCopy.name, 0, sizeof(patchCopy.name));
            strncpy(patchCopy.name, aum->patchNamePointer, 255);
            setPatchNameDisplay();
        }
        else if (aum->action == Engine::AudioToUIMsg::SET_PATCH_DIRTY_STATE)
        {
            patchCopy.dirty = (bool)aum->paramId;
            presetDataBinding->setDirtyState(patchCopy.dirty);
            presetButton->repaint();
        }
        else if (aum->action == Engine::AudioToUIMsg::DO_PARAM_RESCAN)
        {
            if (!clapParamsExtension)
                clapParamsExtension = static_cast<const clap_host_params_t *>(
                    clapHost->get_extension(clapHost, CLAP_EXT_PARAMS));
            if (clapParamsExtension)
            {
                clapParamsExtension->rescan(clapHost, CLAP_PARAM_RESCAN_VALUES |
                                                          CLAP_PARAM_RESCAN_TEXT |
                                                          CLAP_PARAM_RESCAN_INFO);
                clapParamsExtension->request_flush(clapHost);
            }
        }
        else if (aum->action == Engine::AudioToUIMsg::SEND_SAMPLE_RATE)
        {
            sampleRate = aum->value;
            repaint();
        }
        else
        {
            SQLOG("Ignored patch message " << aum->action);
        }
        aum = audioToUI.pop();
    }
}

void PluginEditor::paint(juce::Graphics &g)
{
    jcmp::WindowPanel::paint(g);
    auto ft = style()->getFont(jcmp::Label::Styles::styleClass, jcmp::Label::Styles::labelfont);

    bool isLight = defaultsProvider->getUserDefaultValue(Defaults::useLightSkin, false);

    g.setColour(juce::Colours::white.withAlpha(0.9f));
    auto q = ft.withHeight(30);
    g.setFont(q);
    auto xp = 3;
    auto ht = 30;

    int np{110};

    if (isLight)
        g.setColour(juce::Colours::navy);
    else
        g.setColour(juce::Colours::white.withAlpha(0.5f));
    q = ft.withHeight(12);
    g.setFont(q);

    g.drawText(PRODUCT_NAME, getLocalBounds().reduced(3, 3), juce::Justification::bottomLeft);

    std::string os = "";
#if JUCE_MAC
    os = "macOS";
#endif
#if JUCE_WINDOWS
    os = "Windows";
#endif
#if JUCE_LINUX
    os = "Linux";
#endif

    auto bi = os + " " + sst::plugininfra::VersionInformation::git_commit_hash;
    bi += fmt::format(" @ {:.1f}k", sampleRate / 1000.0);
    g.drawText(bi, getLocalBounds().reduced(3, 3), juce::Justification::bottomRight);

    g.drawText(sst::plugininfra::VersionInformation::git_implied_display_version,
               getLocalBounds().reduced(3, 3), juce::Justification::centredBottom);

    g.setColour(juce::Colours::white);
    g.setFont(juce::FontOptions(25));
    auto dr = juce::Rectangle<int>(0, 0, np, ht);
    g.drawText(PRODUCT_NAME, dr.reduced(2), juce::Justification::centredLeft);

    auto body = getLocalBounds().withTrimmedTop(40).withTrimmedBottom(24).reduced(16);
    g.setFont(juce::FontOptions(16));
    g.setColour(juce::Colours::white.withAlpha(0.85f));
    g.drawFittedText("Soundemote  soemdsp\n\n"
                     "Audio effect - sandbox graph player\n"
                     "Use the preset menu: Load sandbox JSON...\n\n" +
                         juce::String(sandboxStatusText),
                     body, juce::Justification::centredLeft, 12);

#if !defined(NDEBUG) || !NDEBUG
    g.setFont(juce::FontOptions(30));

    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("DEBUG", dr.translated(1, 1), juce::Justification::centred);
    g.setColour(juce::Colours::red.withAlpha(0.6f));
    g.drawText("DEBUG", dr, juce::Justification::centred);
#endif
}

void PluginEditor::resized()
{
    int presetHeight{33}, footerHeight{15};

    auto lb = getLocalBounds();
    auto presetArea = lb.withHeight(presetHeight);
    auto panelArea = lb.withTrimmedTop(presetHeight).withTrimmedBottom(footerHeight);

    auto panelMargin{3};
    auto uicMargin{4};
    // Preset button
    auto but = presetArea.reduced(110, 0).withTrimmedTop(uicMargin);
    presetButton->setBounds(but);
    but = but.withLeft(presetButton->getRight() + uicMargin).withRight(getWidth() - uicMargin);

    vuMeter->setBounds(but);

    auto canvasArea = panelArea.reduced(panelMargin);
    if (patchCanvas)
        patchCanvas->setBounds(canvasArea);
    if (performView)
        performView->setBounds(canvasArea);
    if (mainPanel)
        mainPanel->setBounds(canvasArea);
}

void PluginEditor::showTooltipOn(juce::Component *c)
{
    int x = 0;
    int y = 0;
    juce::Component *component = c;
    while (component != this)
    {
        auto bounds = component->getBoundsInParent();
        x += bounds.getX();
        y += bounds.getY();

        component = component->getParentComponent();
    }
    y += c->getHeight();
    toolTip->resetSizeFromData();
    if (y + toolTip->getHeight() > getHeight() - 40)
    {
        y -= c->getHeight() + 3 + toolTip->getHeight();
    }

    if (x + toolTip->getWidth() > getWidth())
    {
        x -= toolTip->getWidth();
        x += c->getWidth() - 3;
    }

    toolTip->setTopLeftPosition(x, y);
    toolTip->setVisible(true);
}
void PluginEditor::updateTooltip(jdat::Continuous *c)
{
    toolTip->setTooltipTitleAndData(c->getLabel(), {c->getValueAsString()});
    toolTip->resetSizeFromData();
}
void PluginEditor::updateTooltip(jdat::Discrete *d)
{
    toolTip->setTooltipTitleAndData(d->getLabel(), {d->getValueAsString()});
    toolTip->resetSizeFromData();
}
void PluginEditor::hideTooltip() { toolTip->setVisible(false); }

struct MenuValueTypein : HasEditor, juce::PopupMenu::CustomComponent, juce::TextEditor::Listener
{
    std::unique_ptr<juce::TextEditor> textEditor;
    juce::Component::SafePointer<jcmp::ContinuousParamEditor> underComp;

    MenuValueTypein(PluginEditor &editor,
                    juce::Component::SafePointer<jcmp::ContinuousParamEditor> under)
        : juce::PopupMenu::CustomComponent(false), HasEditor(editor), underComp(under)
    {
        textEditor = std::make_unique<juce::TextEditor>();
        textEditor->setWantsKeyboardFocus(true);
        textEditor->addListener(this);
        textEditor->setIndents(2, 0);

        addAndMakeVisible(*textEditor);
    }

    void getIdealSize(int &w, int &h) override
    {
        w = 180;
        h = 22;
    }
    void resized() override { textEditor->setBounds(getLocalBounds().reduced(3, 1)); }

    void visibilityChanged() override
    {
        juce::Timer::callAfterDelay(
            2,
            [this]()
            {
                if (textEditor->isVisible())
                {
                    textEditor->setText(getInitialText(),
                                        juce::NotificationType::dontSendNotification);
                    auto valCol = juce::Colour(0xFF, 0x90, 0x00);
                    textEditor->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                                          valCol.withAlpha(0.1f));
                    textEditor->setColour(juce::TextEditor::ColourIds::highlightColourId,
                                          valCol.withAlpha(0.15f));
                    textEditor->setJustification(juce::Justification::centredLeft);
                    textEditor->setColour(juce::TextEditor::ColourIds::outlineColourId,
                                          juce::Colours::black.withAlpha(0.f));
                    textEditor->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                                          juce::Colours::black.withAlpha(0.f));
                    textEditor->setBorder(juce::BorderSize<int>(3));
                    textEditor->applyColourToAllText(valCol, true);
                    textEditor->grabKeyboardFocus();
                    textEditor->selectAll();
                }
            });
    }

    std::string getInitialText() const { return underComp->continuous()->getValueAsString(); }

    void setValueString(const std::string &s)
    {
        if (underComp && underComp->continuous())
        {
            underComp->onBeginEdit();

            if (s.empty())
            {
                underComp->continuous()->setValueFromGUI(
                    underComp->continuous()->getDefaultValue());
            }
            else
            {
                underComp->continuous()->setValueAsString(s);
            }
            underComp->onEndEdit();
            underComp->repaint();
            underComp->grabKeyboardFocus();
            underComp->notifyAccessibleChange();
        }
    }

    void textEditorReturnKeyPressed(juce::TextEditor &ed) override
    {
        auto s = ed.getText().toStdString();
        setValueString(s);
        triggerMenuItem();
    }
    void textEditorEscapeKeyPressed(juce::TextEditor &) override { triggerMenuItem(); }
};

void PluginEditor::popupMenuForContinuous(jcmp::ContinuousParamEditor *e)
{
    auto data = e->continuous();
    if (!data)
    {
        return;
    }

    if (!e->isEnabled())
    {
        return;
    }

    auto p = juce::PopupMenu();
    p.addSectionHeader(data->getLabel());
    p.addSeparator();
    p.addCustomItem(-1, std::make_unique<MenuValueTypein>(*this, juce::Component::SafePointer(e)));
    p.addSeparator();
    p.addItem("Set to Default",
              [w = juce::Component::SafePointer(e)]()
              {
                  if (!w)
                      return;
                  w->continuous()->setValueFromGUI(w->continuous()->getDefaultValue());
                  w->repaint();
              });

    // I could also stick the param id onto the component properties I guess
    auto pid = sst::jucegui::component_adapters::getClapParamId(e);
    if (pid.has_value())
    {
        sst::clap_juce_shim::populateMenuForClapParam(p, *pid, clapHost);
    }

    p.showMenuAsync(juce::PopupMenu::Options().withParentComponent(this));
}

void PluginEditor::showPresetPopup()
{
    auto p = juce::PopupMenu();
    p.addSectionHeader("Main Menu");

    auto f = juce::PopupMenu();
    for (auto &[c, ent] : presetManager->factoryPatchNames)
    {
        auto em = juce::PopupMenu();
        for (auto &e : ent)
        {
            auto noExt = e;
            auto ps = noExt.find(PATCH_EXTENSION);
            if (ps != std::string::npos)
            {
                noExt = noExt.substr(0, ps);
            }
            em.addItem(noExt,
                       [cat = c, pat = e, this]() {
                           this->presetManager->loadFactoryPreset(patchCopy, mainToAudio, cat, pat);
                       });
        }
        f.addSubMenu(c, em);
    }

    auto u = juce::PopupMenu();
    auto cat = fs::path();
    auto s = juce::PopupMenu();
    for (const auto &up : presetManager->userPatches)
    {
        auto pp = up.parent_path();
        auto dn = up.filename().replace_extension("").u8string();
        if (pp.empty())
        {
            u.addItem(dn,
                      [this, pth = up, dn]()
                      {
                          presetManager->loadUserPresetDirect(patchCopy, mainToAudio,
                                                              presetManager->userPatchesPath / pth);
                      });
        }
        else
        {
            if (pp != cat)
            {
                if (cat.empty())
                {
                    u.addSeparator();
                }
                if (s.getNumItems() > 0)
                {
                    u.addSubMenu(cat.u8string(), s);
                    s = juce::PopupMenu();
                }
                cat = pp;
            }
            s.addItem(dn,
                      [this, pth = up, dn]()
                      {
                          presetManager->loadUserPresetDirect(patchCopy, mainToAudio,
                                                              presetManager->userPatchesPath / pth);
                      });
        }
    }
    if (s.getNumItems() > 0 && !cat.empty())
    {
        u.addSubMenu(cat.u8string(), s);
    }
    p.addSeparator();
    p.addSubMenu("Factory Presets", f);
    p.addSubMenu("User Presets", u);
    p.addSeparator();
    p.addItem("Load Patch",
              [w = juce::Component::SafePointer(this)]()
              {
                  if (w)
                      w->doLoadPatch();
              });
    p.addItem("Load sandbox JSON...",
              [w = juce::Component::SafePointer(this)]()
              {
                  if (w)
                      w->doLoadSandboxJson();
              });
    p.addItem("Save Patch",
              [w = juce::Component::SafePointer(this)]()
              {
                  if (w)
                      w->doSavePatch();
              });
    p.addSeparator();
    p.addItem("Reset to Init",
              [w = juce::Component::SafePointer(this)]()
              {
                  if (w)
                  {
                      w->resetToDefault();
                  }
              });
    p.addSeparator();

    auto uim = juce::PopupMenu();
    auto isLight = defaultsProvider->getUserDefaultValue(Defaults::useLightSkin, 0);

    for (auto scale : {75, 90, 100, 110, 125, 150})
    {
        uim.addItem("Zoom " + std::to_string(scale) + "%", true,
                    std::fabs(zoomFactor * 100 - scale) < 2,
                    [s = scale, w = juce::Component::SafePointer(this)]()
                    {
                        if (!w)
                            return;
                        w->setZoomFactor(s * 0.01);
                    });
    }

    uim.addSeparator();
    uim.addItem("Dark Mode", true, !isLight,
                [w = juce::Component::SafePointer(this)]()
                {
                    if (!w)
                        return;
                    w->defaultsProvider->updateUserDefaultValue(Defaults::useLightSkin, false);
                    w->setSkinFromDefaults();
                });

    uim.addItem("Light Mode", true, isLight,
                [w = juce::Component::SafePointer(this)]()
                {
                    if (!w)
                        return;
                    w->defaultsProvider->updateUserDefaultValue(Defaults::useLightSkin, true);
                    w->setSkinFromDefaults();
                });
    uim.addSeparator();

    uim.addItem("Activate Debug Log", true, debugLevel > 0,
                [w = juce::Component::SafePointer(this)]()
                {
                    if (w)
                        w->toggleDebug();
                });

#if JUCE_WINDOWS
    auto swr = defaultsProvider->getUserDefaultValue(Defaults::useSoftwareRenderer, false);

    uim.addItem(
        "Use Software Renderer", true, swr,
        [w = juce::Component::SafePointer(this), swr]()
        {
            if (!w)
                return;
            w->defaultsProvider->updateUserDefaultValue(Defaults::useSoftwareRenderer, !swr);
            juce::AlertWindow::showMessageBoxAsync(
                juce::AlertWindow::WarningIcon, "Software Renderer Change",
                "A software renderer change is only active once you restart/reload the plugin.");
        });
#endif

    p.addSubMenu("User Interface", uim);

    p.addSeparator();
    p.addItem("Read the Manual", false, false, []() {});
    p.addItem("Get the Source",
              []() {
                  juce::URL("https://github.com/baconpaul/SoemDSP-startingpoint/")
                      .launchInDefaultBrowser();
              });
    p.addItem("Acknowledgements", false, false, []() {});
    p.showMenuAsync(juce::PopupMenu::Options().withParentComponent(this));
}

void PluginEditor::doSavePatch()
{
    auto svP = presetManager->userPatchesPath;
    if (strcmp(patchCopy.name, "Init") != 0)
    {
        svP = (svP / patchCopy.name).replace_extension(PATCH_EXTENSION);
    }
    fileChooser = std::make_unique<juce::FileChooser>("Save Patch", juce::File(svP.u8string()),
                                                      juce::String("*") + PATCH_EXTENSION);
    fileChooser->launchAsync(juce::FileBrowserComponent::canSelectFiles |
                                 juce::FileBrowserComponent::saveMode |
                                 juce::FileBrowserComponent::warnAboutOverwriting,
                             [w = juce::Component::SafePointer(this)](const juce::FileChooser &c)
                             {
                                 if (!w)
                                     return;
                                 auto result = c.getResults();
                                 if (result.isEmpty() || result.size() > 1)
                                 {
                                     return;
                                 }
                                 auto pn = fs::path{result[0].getFullPathName().toStdString()};
                                 w->setPatchNameTo(pn.filename().replace_extension("").u8string());

#if USE_WCHAR_PRESET
                                 w->presetManager->saveUserPresetDirect(
                                     w->patchCopy, result[0].getFullPathName().toUTF16());
#else
                                 w->presetManager->saveUserPresetDirect(w->patchCopy, pn);
#endif

                                 w->presetDataBinding->setDirtyState(false);
                                 w->repaint();
                             });
}

void PluginEditor::setPatchNameTo(const std::string &s)
{
    memset(patchCopy.name, 0, sizeof(patchCopy.name));
    strncpy(patchCopy.name, s.c_str(), 255);
    mainToAudio.push({Engine::MainToAudioMsg::SEND_PATCH_NAME, 0, 0, patchCopy.name});
    setPatchNameDisplay();
}

void PluginEditor::doLoadSandboxJson()
{
    auto startDir = juce::File("C:/Users/argit/Documents/_PROGRAMMING/soemdsp-sandbox/patches");
    if (!startDir.isDirectory())
        startDir = juce::File(presetManager->userPatchesPath.u8string());
    fileChooser = std::make_unique<juce::FileChooser>("Load sandbox JSON", startDir, "*.json");
    fileChooser->launchAsync(
        juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode,
        [w = juce::Component::SafePointer(this)](const juce::FileChooser &c)
        {
            if (!w)
                return;
            auto result = c.getResults();
            if (result.isEmpty() || result.size() > 1)
                return;
            auto text = result[0].loadFileAsString();
            w->lastSandboxJson = text;
            if (w->performView)
                w->performView->pushPatchJson(text);
            auto plan = parseSandboxPatchJson(text.toStdString(),
                                              result[0].getFileNameWithoutExtension().toStdString());
            if (!plan.error.empty() && plan.nodes.empty())
            {
                w->sandboxStatusText = "Load failed: " + plan.error;
                w->repaint();
                return;
            }
            w->sandboxStatusText = plan.name + "  (" + std::to_string(plan.added) +
                                   " native nodes, " + std::to_string(plan.skipped) + " skipped)";
            if (w->presetDataBinding)
                w->presetDataBinding->setStateForDisplayName(
                    result[0].getFileNameWithoutExtension().toStdString());
            if (w->presetButton)
                w->presetButton->repaint();
            w->canvasPlan = std::make_shared<GraphPlan>(plan);
            for (auto &sb : plan.slots)
            {
                w->setAndSendParamValue((uint32_t)(1000 + sb.slot), (float)sb.initial01, true, false);
            }
            auto *heapPlan = new GraphPlan(std::move(plan));
            w->mainToAudio.push({Engine::MainToAudioMsg::LOAD_SANDBOX_JSON, 0, 0,
                                 reinterpret_cast<const char *>(heapPlan)});
            w->mainToAudio.push({Engine::MainToAudioMsg::SEND_REQUEST_RESCAN, 0, 0, nullptr});
            if (w->patchCanvas)
                w->patchCanvas->repaint();
            w->repaint();
        });
}

void PluginEditor::doLoadPatch()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Load Patch", juce::File(presetManager->userPatchesPath.u8string()),
        juce::String("*") + PATCH_EXTENSION);
    fileChooser->launchAsync(
        juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::openMode,
        [w = juce::Component::SafePointer(this)](const juce::FileChooser &c)
        {
            if (!w)
                return;
            auto result = c.getResults();
            if (result.isEmpty() || result.size() > 1)
            {
                return;
            }
            auto loadPath = fs::path{result[0].getFullPathName().toStdString()};
            w->presetManager->loadUserPresetDirect(w->patchCopy, w->mainToAudio, loadPath);
        });
}

void PluginEditor::resetToDefault() { presetManager->loadInit(patchCopy, mainToAudio); }

void PluginEditor::onPerformNotes(double c0, double c1, double c2)
{
    if (performNotesHandler)
        performNotesHandler(c0, c1, c2);
}

void PluginEditor::onPerformGesture(int pluginId, float value, const juce::String &phase)
{
    if (pluginId < 0 || pluginId > 31)
        return;
    value = juce::jlimit(0.f, 1.f, value);
    const uint32_t paramId = (uint32_t)(1000 + pluginId);
    if (!patchCopy.paramMap.count(paramId))
        return;
    patchCopy.paramMap[paramId]->value = value;
    if (phase == "begin")
    {
        gesturingParamId = (int32_t)paramId;
        mainToAudio.push({Engine::MainToAudioMsg::Action::BEGIN_EDIT, paramId});
    }
    mainToAudio.push({Engine::MainToAudioMsg::Action::SET_PARAM, paramId, value});
    if (phase == "end")
    {
        mainToAudio.push({Engine::MainToAudioMsg::Action::END_EDIT, paramId});
        if (gesturingParamId == (int32_t)paramId)
            gesturingParamId = -1;
    }
    requestParamsFlush();
    if (patchCanvas)
        patchCanvas->repaint();
}

void PluginEditor::setAndSendParamValue(uint32_t paramId, float value, bool notifyAudio,
                                        bool sendBeginEnd)
{
    patchCopy.paramMap[paramId]->value = value;

    auto rit = componentRefreshByID.find(paramId);
    if (rit != componentRefreshByID.end())
    {
        rit->second();
    }

    auto pit = componentByID.find(paramId);
    if (pit != componentByID.end() && pit->second)
        pit->second->repaint();

    if (notifyAudio)
    {
        if (sendBeginEnd)
            mainToAudio.push({Engine::MainToAudioMsg::Action::BEGIN_EDIT, paramId});
        mainToAudio.push({Engine::MainToAudioMsg::Action::SET_PARAM, paramId, value});
        if (sendBeginEnd)
            mainToAudio.push({Engine::MainToAudioMsg::Action::END_EDIT, paramId});
        requestParamsFlush();
    }
}

void PluginEditor::setPatchNameDisplay()
{
    if (!presetButton)
        return;
    presetDataBinding->setStateForDisplayName(patchCopy.name);
    presetButton->repaint();
}

void PluginEditor::postPatchChange(const std::string &s)
{
    presetDataBinding->setStateForDisplayName(s);
    for (auto [id, f] : componentRefreshByID)
        f();

    repaint();
}

bool PluginEditor::keyPressed(const juce::KeyPress &key) { return false; }

void PluginEditor::visibilityChanged()
{
    if (isVisible() && isShowing())
    {
        presetButton->setWantsKeyboardFocus(true);
        presetButton->grabKeyboardFocus();
    }
}

void PluginEditor::parentHierarchyChanged()
{
    if (isVisible() && isShowing())
    {
        presetButton->setWantsKeyboardFocus(true);
        presetButton->grabKeyboardFocus();
    }

#if JUCE_WINDOWS
    auto swr = defaultsProvider->getUserDefaultValue(Defaults::useSoftwareRenderer, false);

    if (swr)
    {
        if (auto peer = getPeer())
        {
            SQLOG("Enabling software rendering engine");
            peer->setCurrentRenderingEngine(0); // 0 for software mode, 1 for Direct2D mode
        }
    }
#endif
}

void PluginEditor::setSkinFromDefaults()
{
    auto b = defaultsProvider->getUserDefaultValue(Defaults::useLightSkin, 0);
    if (b)
    {
        setStyle(sst::jucegui::style::StyleSheet::getBuiltInStyleSheet(
            sst::jucegui::style::StyleSheet::LIGHT));
        style()->setColour(PatchMenu, jcmp::MenuButton::Styles::fill,
                           style()
                               ->getColour(jcmp::base_styles::Base::styleClass,
                                           jcmp::base_styles::Base::background)
                               .darker(0.3f));
    }
    else
    {
        setStyle(sst::jucegui::style::StyleSheet::getBuiltInStyleSheet(
            sst::jucegui::style::StyleSheet::DARK));
    }

    style()->setFont(
        PatchMenu, jcmp::MenuButton::Styles::labelfont,
        style()
            ->getFont(jcmp::MenuButton::Styles::styleClass, jcmp::MenuButton::Styles::labelfont)
            .withHeight(18));
}

void PluginEditor::setZoomFactor(float zf)
{
    // SCLOG("Setting zoom factor to " << zf);
    zoomFactor = zf;
    setTransform(juce::AffineTransform().scaled(zoomFactor));
    defaultsProvider->updateUserDefaultValue(Defaults::zoomLevel, zoomFactor * 100);
    if (onZoomChanged)
        onZoomChanged(zoomFactor);
}

void PluginEditor::doSinglePanelHamburger()
{
    juce::Component *vis;
    for (auto c : mainPanel->getChildren())
    {
        if (c->isVisible())
        {
            vis = c;
        }
    }
    if (!vis)
        return;
}

void PluginEditor::activateHamburger(bool b)
{
    mainPanel->hasHamburger = b;
    mainPanel->repaint();
}

void PluginEditor::requestParamsFlush()
{
    if (!clapParamsExtension)
        clapParamsExtension = static_cast<const clap_host_params_t *>(
            clapHost->get_extension(clapHost, CLAP_EXT_PARAMS));
    if (clapParamsExtension)
    {
        clapParamsExtension->request_flush(clapHost);
    }
}

void PluginEditor::sneakyStartupGrabFrom(Patch &other)
{
    for (auto &p : other.params)
    {
        patchCopy.paramMap.at(p->meta.id)->value = p->value;
    }
    strncpy(patchCopy.name, other.name, 255);
    postPatchChange(other.name);
}

bool PluginEditor::toggleDebug()
{
    if (debugLevel == 0)
    {
        sst::plugininfra::misc_platform::allocateConsole();
    }
    if (debugLevel <= 0)
        debugLevel = 1;
    else
        debugLevel = -1;
    SQLOG("Started debug session");
    SQLOG("If you are on windows and you close this window it may end your entire session");
    return debugLevel > 0;
}

void PluginEditor::onStyleChanged()
{
    jcmp::WindowPanel::onStyleChanged();
    if (lnf)
        lnf->setStyle(style());
}

} // namespace soemdsp_ap::ui