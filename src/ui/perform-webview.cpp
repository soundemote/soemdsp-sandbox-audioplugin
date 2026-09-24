#include "perform-webview.h"

#include "plugin-editor.h"

#include <cmrc/cmrc.hpp>
#include <cstring>

#if JUCE_WINDOWS
 #include <windows.h>
#endif

CMRC_DECLARE(soemdsp_perform);

namespace soemdsp_ap::ui
{
namespace
{
juce::File moduleDirectory()
{
#if JUCE_WINDOWS
    HMODULE module = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&moduleDirectory), &module))
    {
        wchar_t path[MAX_PATH]{};
        const auto n = GetModuleFileNameW(module, path, MAX_PATH);
        if (n > 0)
            return juce::File(juce::String(path, (size_t)n)).getParentDirectory();
    }
#endif
    return juce::File::getSpecialLocation(juce::File::currentExecutableFile).getParentDirectory();
}

juce::File webView2LoaderDll()
{
    auto dll = moduleDirectory().getChildFile("WebView2Loader.dll");
    return dll.existsAsFile() ? dll : juce::File();
}

juce::File performPackRoot()
{
    auto dir = moduleDirectory();
    for (int i = 0; i < 4 && dir.getFullPathName().isNotEmpty(); ++i)
    {
        auto pack = dir.getChildFile("soemdsp-perform");
        if (pack.getChildFile("perform.html").existsAsFile())
            return pack;
        dir = dir.getParentDirectory();
    }
    return {};
}

juce::String mimeForPath(const juce::String &path)
{
    if (path.endsWithIgnoreCase(".html")) return "text/html";
    if (path.endsWithIgnoreCase(".js")) return "text/javascript";
    if (path.endsWithIgnoreCase(".css")) return "text/css";
    if (path.endsWithIgnoreCase(".json")) return "application/json";
    if (path.endsWithIgnoreCase(".svg")) return "image/svg+xml";
    if (path.endsWithIgnoreCase(".png")) return "image/png";
    if (path.endsWithIgnoreCase(".jpg") || path.endsWithIgnoreCase(".jpeg")) return "image/jpeg";
    if (path.endsWithIgnoreCase(".wasm")) return "application/wasm";
    if (path.endsWithIgnoreCase(".woff2")) return "font/woff2";
    if (path.endsWithIgnoreCase(".woff")) return "font/woff";
    return "application/octet-stream";
}

std::optional<juce::WebBrowserComponent::Resource> resourceFromFile(const juce::File &file)
{
    juce::MemoryBlock block;
    if (!file.existsAsFile() || !file.loadFileAsData(block))
        return std::nullopt;
    juce::WebBrowserComponent::Resource resource;
    resource.data.resize(block.getSize());
    if (block.getSize() > 0)
        std::memcpy(resource.data.data(), block.getData(), block.getSize());
    resource.mimeType = mimeForPath(file.getFileName());
    return resource;
}

std::optional<juce::WebBrowserComponent::Resource> performResource(const juce::String &url)
{
    auto path = url.upToFirstOccurrenceOf("?", false, false)
                    .upToFirstOccurrenceOf("#", false, false);
    if (path.contains("://"))
        path = path.fromFirstOccurrenceOf("://", false, false).fromFirstOccurrenceOf("/", true, false);
    if (path == "/" || path == "/index.html" || path == "/perform.html" || path.isEmpty())
        path = "/perform.html";

    if (auto pack = performPackRoot(); pack.isDirectory())
    {
        auto rel = path.startsWithChar('/') ? path.substring(1) : path;
        if (rel.contains(".."))
            return std::nullopt;
        juce::File file;
        if (rel == "perform.html" || rel == "index.html")
            file = pack.getChildFile(rel);
        else if (rel == "favicon.svg" || rel == "favicon.ico")
            file = pack.getChildFile("public").getChildFile("favicon.svg");
        else
            file = pack.getChildFile(rel);
        auto root = pack.getFullPathName();
        if (!file.getFullPathName().startsWith(root))
            return std::nullopt;
        if (auto resource = resourceFromFile(file))
            return resource;
    }

    if (path == "/perform.html")
        path = "perform.html";
    else if (path.startsWithChar('/'))
        path = path.substring(1);

    try
    {
        auto fs = cmrc::soemdsp_perform::get_filesystem();
        if (!fs.exists(path.toStdString()))
            return std::nullopt;
        auto file = fs.open(path.toStdString());
        juce::WebBrowserComponent::Resource resource;
        resource.data.resize((size_t)std::distance(file.begin(), file.end()));
        std::memcpy(resource.data.data(), file.begin(), resource.data.size());
        if (path.endsWithIgnoreCase(".html"))
            resource.mimeType = "text/html";
        else if (path.endsWithIgnoreCase(".js"))
            resource.mimeType = "text/javascript";
        else if (path.endsWithIgnoreCase(".css"))
            resource.mimeType = "text/css";
        else
            resource.mimeType = "application/octet-stream";
        return resource;
    }
    catch (...)
    {
        return std::nullopt;
    }
}
} // namespace

PerformView::PerformView(PluginEditor &ed) : editor(ed)
{
    auto options = juce::WebBrowserComponent::Options{}
                       .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
                       .withNativeIntegrationEnabled()
                       .withResourceProvider([](const juce::String &url) { return performResource(url); })
                       .withEventListener("soemdsp-perform",
                                          [this](const juce::var &payload)
                                          {
                                              auto *obj = payload.getDynamicObject();
                                              if (obj == nullptr)
                                                  return;
                                              const auto event = obj->getProperty("event").toString();
                                              if (event == "ready")
                                              {
                                                  pageReady = true;
                                                  if (pendingJson.isNotEmpty())
                                                      pushPatchJson(pendingJson);
                                                  return;
                                              }
                                              if (event == "gesture")
                                              {
                                                  const int pluginId = (int)obj->getProperty("pluginId");
                                                  const float value = (float)(double)obj->getProperty("value");
                                                  editor.onPerformGesture(pluginId, value,
                                                                           obj->getProperty("phase").toString());
                                              }
                                              if (event == "notes")
                                              {
                                                  editor.onPerformNotes(
                                                      (double)obj->getProperty("c0"),
                                                      (double)obj->getProperty("c1"),
                                                      (double)obj->getProperty("c2"));
                                              }
                                          });

#if JUCE_WINDOWS
    auto dll = webView2LoaderDll();
    juce::WebBrowserComponent::Options::WinWebView2 win;
    win = win.withUserDataFolder(juce::File::getSpecialLocation(juce::File::tempDirectory)
                                     .getChildFile("soemdsp-webview"));
    if (dll.existsAsFile())
        win = win.withDLLLocation(dll);
    options = options.withWinWebView2Options(win);
#endif

    if (!juce::WebBrowserComponent::areOptionsSupported(options))
        return;

    browser = std::make_unique<juce::WebBrowserComponent>(options);
    addAndMakeVisible(*browser);
    browser->goToURL(juce::WebBrowserComponent::getResourceProviderRoot() + "perform.html");
}

PerformView::~PerformView() = default;

void PerformView::resized()
{
    if (browser)
        browser->setBounds(getLocalBounds());
}

void PerformView::pushPatchJson(const juce::String &json)
{
    pendingJson = json;
    if (!browser || !pageReady || json.isEmpty())
        return;
    auto parsed = juce::JSON::parse(json);
    if (parsed.isVoid())
        return;
    browser->evaluateJavascript("window.soemdspPerform && window.soemdspPerform.loadPatch(" +
                                juce::JSON::toString(parsed) + ")");
}

void PerformView::pushSlot(int pluginId, float value)
{
    if (!browser || !pageReady)
        return;
    browser->evaluateJavascript("window.soemdspPerform && window.soemdspPerform.setSlot(" +
                                juce::String(pluginId) + "," + juce::String(value, 6) + ")");
}
} // namespace soemdsp_ap::ui
