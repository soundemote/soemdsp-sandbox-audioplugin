# PLAN FOR PLUGIN

Slow path. Two repos. A coding agent can implement both sides against this doc.

- **soemdsp-sandbox** — workshop (online) + **perform mode** (picture the plugin loads).
- **soemdsp-sandbox-audioplugin** — this repo: native audio, 32 CLAP slots, JUCE WebView shell.

Online = **make** patches. DAW = **use** them. One audio engine (`graph_engine`). One painter (sandbox JS faces). The plugin is a shell, not a second sandbox.

Do not grow C++ `paint()` faces. Freeze them until the WebView shows the same JSON.

---

## Product split

| Place | Job |
|-------|-----|
| **soemdsp-sandbox** | Shop, cables, Display Settings, save JSON. Browser preview audio. Also ships a perform page. |
| **soemdsp-sandbox-audioplugin** | Load that JSON, run native audio, show perform page, bind 32 slots. No shop. No rewiring. |

Authoring never moves into JUCE. The DAW never becomes the editor.

---

## Two layers (do not mix)

**Audio (native, this repo)**

- `graph_engine` is the only DSP.
- Load sandbox JSON → compile the graph.
- Always **32** CLAP slots (`pluginId` 0–31). Empty slots stay reserved.
- Host automation, mouse, and the picture share those slot values.
- Process() stays in C++. The perform page does **not** run AudioWorklet or a second graph.

**Picture (sandbox JS)**

- Knob, Slider, Toggle, Momentary, filter faces already draw in the sandbox.
- That code is the only face renderer.
- Plugin editor hosts it in a JUCE `WebBrowserComponent` (WebView2 on Windows).
- `paint()` may keep a thin native strip: load patch, preset, status. Not modules.

---

## Frozen contract (both sides)

Do not invent a second patch format. Perform mode reads what Save already writes: nodes, `pluginId` 0–31, Folder/Control, `traceDisplaySettings`.

### Entry

Sandbox: `perform.html` at repo root next to `index.html` (or `public/perform.html` if boot requires it). Query `?mode=perform` on `index.html` is an acceptable alias that must behave the same.

Plugin packs that page + the JS/CSS it needs. Never navigate to soundemote.io.

### Perform page is the phone / F fullscreen (not the modular graph)

The workshop already has this. Phone button + **F** cycle: `off → perform → edit → off` (`public/node-graph-layout-canvas.js`).

- **perform** — pinned faces only, freeform `x/y/w/h` from `patch.view.canvases`. No tile grips. Fullscreen, no workshop chrome.
- **edit** — same tiles, user can move/resize. **Workshop only.** The plugin never opens edit.
- Pins live on the patch (`patch.view.canvases.root` / `byMetamodule`). Author those on the site (phone/F edit), save JSON; the DAW shows that layout.

The plugin perform page is **that perform state**, not a copy of the modular wiring view and not a new layout system. If nothing is pinned, the page is empty (or a short “pin displays on the site” note) — do not auto-dump every module onto the stage.

### Perform page rules

- Open layout-canvas **perform** from JSON. Same face modules as the workshop.
- No shop, no cable edit, no add/remove, no cameras, no code screen, no boot menu, no F-edit grips.
- **No AudioWorklet. No live graph. No wasm audio.**
- Hit-test only controller faces that have `pluginId` 0–31: `knob`, `pluginSlider`, `toggleButton`, `momentaryButton`.
- Other pinned modules are display-only (no gesture into CLAP until a later milestone).
- Gestures do **not** write sandbox `params` / Bias as the workshop does. They emit the bridge message. Native audio is truth.

### Message protocol (`v: 1`)

All messages are JSON objects with `type: "soemdsp-perform"` and `v: 1`.

**Page → host**

| `event` | Fields | When |
|---------|--------|------|
| `ready` | — | Perform page booted, waiting for a patch |
| `gesture` | `pluginId` (int 0–31), `value` (float 0–1), `phase` (`begin` \| `set` \| `end`) | Pointer on a controller |

**Host → page**

| `event` | Fields | When |
|---------|--------|------|
| `loadPatch` | `patch` (full sandbox JSON object) | After native load |
| `setSlot` | `pluginId` (int 0–31), `value` (float 0–1) | Automation / host echo. Ignore `setSlot` for a `pluginId` that currently has an open `begin`…`end` gesture |

Also expose on the window for `evaluateJavascript`:

```js
window.soemdspPerform = {
  loadPatch(patch),          // same as loadPatch event
  setSlot(pluginId, value),  // same as setSlot event
};
```

Page posts with `window.parent.postMessage(...)` and, if present, JUCE native integration (`window.__JUCE__` / `emitEvent` — match whatever the editor enables). Host must accept **either** `postMessage` or the JUCE native callback so browser-only testing works without JUCE.

`value` is 0–1 across the parameter min/max (same as CLAP slot), not raw Bias units.

### Browser-only test (sandbox, no plugin)

Open `perform.html`. Devtools:

```js
soemdspPerform.loadPatch(/* paste JSON */);
window.addEventListener("message", (e) => console.log(e.data));
```

Drag a Knob with `pluginId`. Console must show `gesture` begin/set/end. Call `soemdspPerform.setSlot(n, 0.5)` and the face must move.

---

## Sandbox work (online repo)

Workshop stays. Users still make patches in `index.html`.

### Add

1. **`perform.html`** — minimal shell. After load, open **layout-canvas perform** (`nodeGraphLayoutCanvasOpen("perform")`) — the same stage as the phone button / F, not the modular graph.
2. **`public/perform-boot.js`** — skip worklet and shop, `loadNodeGraphPatchFromObject`, then layout-canvas perform. Wire controller gestures to the protocol above.
3. Hide leftover workshop chrome (body class e.g. `node-perform-mode`). Do not invent a second tiling system; reuse `node-graph-layout-canvas.js` and `patch.view.canvases`.

### Change (only if required for perform)

- Face paint must run without a live worklet. If a face currently no-ops until audio is up, drive it from stored Bias / param values on the node.
- Controller pointer handlers in perform mode: if `pluginId` is 0–31, emit `gesture` and do not start a live-graph write. Workshop mode unchanged.
- `pluginId` uniqueness already exists for Knob. Perform should honor `pluginId` on Slider / Toggle / Momentary the same way the plugin JSON loader does.

### Do not

- Strip the workshop down to perform mode.
- Add a second patch JSON schema.
- Start AudioWorklet from perform.html.
- Put DAW/CLAP types in sandbox JS beyond `pluginId` + 0–1.

### Likely files (sandbox)

- New: `perform.html`, `public/perform-boot.js`
- Touch as needed: `public/index.html` (optional `?mode=perform` alias), `public/boot-loading.js` (skip workshop boot), `public/node-graph-patch-core.js` (load JSON without live start), `public/modules/knob/knob-face.js`, `public/modules/slider/slider-face.js`, `public/modules/plugin/plugin-controls-ui.js`, `public/styles.css` (hide chrome)
- Cache-bust new scripts in `perform.html` only; do not churn every workshop query unless the shared file actually changed.

### Sandbox acceptance

- Workshop `index.html` still authors and saves as today.
- `perform.html` + `soemdspPerform.loadPatch(json)` draws the patch with no audio.
- Gesture on `pluginId` logs protocol messages.
- `setSlot` moves the face.
- No worklet in the network/console for that page.

---

## Plugin work (this repo)

Native audio / JSON load / 32 slots stay. Picture becomes the packed perform page.

### Add / change

1. **Pack** `perform.html` + its JS/CSS (copy or cmrc/zip at plugin build). Pin a sandbox revision. Do not fetch the live site.
2. **Editor** — `WebBrowserComponent` with WebView2 on Windows, resource provider serving the pack (see JUCE `WebViewPluginDemo.h` for the pattern: `withResourceProvider`, `withNativeIntegrationEnabled`, `withBackend(webview2)`).
3. **Thin chrome** — existing Load sandbox JSON; keep it native.
4. **Bridge** — on load, `loadPatch` into the page; on `gesture`, map to CLAP slot begin/set/end; on host `UPDATE_PARAM`, `setSlot` into the page (skip while that slot is gesturing).
5. **Stopgap** — current C++ module canvas stays until step 4 of the order below; then stop painting those types when WebView is up.

### Do not

- Point WebView at https://soundemote.io.
- Rely on CLAP draft `clap.webview` as the only window (REAPER may not implement it).
- Run sandbox audio in the page.
- Add new C++ face types.
- Vendor the entire sandbox `public/` if a smaller face-pack works; start with whatever `perform.html` actually requests.

### Likely files (plugin)

- `src/ui/plugin-editor.cpp` / `plugin-editor.h` — host WebView, keep load-JSON chrome
- Resource pack CMake (cmrc or zip next to clap/vst3)
- Existing `sandbox_patch_json.cpp` / engine load path — reuse; do not parse a new format
- `controller-faces.h` — freeze, then dead once WebView is the picture

### Plugin acceptance

- Standalone or REAPER: editor shows the perform page (even empty).
- Load JSON: native graph runs **and** page shows the patch.
- Drag in page: CLAP slot + audio follow.
- Automate the slot in the DAW: face moves; no gesture loop (ignore echo during drag).

---

## Slow order (both sides)

Each step shippable. Do not start the next until the current one works.

| Step | Who | Done when |
|------|-----|-----------|
| **0 Freeze stopgap** | plugin | No new C++ faces. Audio/JSON/slots may still change. |
| **1 Perform mode** | sandbox | Browser: load JSON, draw, log one Knob gesture. No DSP. |
| **2 WebView shell** | plugin | Editor loads packed perform page. Load JSON still native-only. |
| **3 One control** | both | One `pluginId` Knob: drag ↔ CLAP ↔ audio. |
| **4 Slider / Toggle / Momentary** | both | Same bridge. Stop C++ draw for those types when page is up. |
| **5 Rest of canvas** | sandbox first | Display-only faces (filters, etc.). Live scope samples later; no JS audio. |
| **6 Drop C++ canvas** | plugin | Remove duplicate face draw. Keep chrome + engine. |

Parallel: sandbox can finish **1** (and most of **5**) without a plugin build. Plugin can do **0** and **2** (hello WebView) without perform mode complete, then wire **3** when `soemdspPerform` exists.

---

## Stop doing

- Recreating sandbox faces in C++ (`controller-faces.h`).
- Porting shop / wiring / cameras into JUCE.
- Running sandbox live audio inside the plugin.
- A fifth chrome layout or a second patch JSON for “plugin only.”
- BaconPaul / template DSP or gesture lag as a second control engine.

---

## Decision log

- **2026-09-23** — Online = make; DAW = use. One painter (sandbox JS perform mode in a JUCE WebView). Native `graph_engine` + 32 CLAP slots unchanged. Do not treat JUCE `paint()` as a second sandbox.
- **2026-09-23** — Dual-repo contract: `perform.html` + `soemdsp-perform` v1 messages + `window.soemdspPerform`. Agents may implement sandbox perform mode and plugin WebView in parallel against this file.
- **2026-09-23** — Perform view = existing phone / F **layout-canvas perform** (`patch.view.canvases`), not the modular wiring graph and not F-edit. Pin displays on the site; the DAW shows that layout.
