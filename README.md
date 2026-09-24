# soemdsp-sandbox-audioplugin

**DAW plugin + standalone host** for running [soemdsp-sandbox](https://github.com/soundemote/soemdsp-sandbox) patches in a host (e.g. REAPER).

This is **not** a fork of the sandbox. Authoring stays in `soemdsp-sandbox`. This repo owns the CLAP/VST3/AU (+ standalone) shell around a shared runtime.

Built **clap-first** (BaconPaul / free-audio clap-wrapper): CLAP native, then VST3/AU wrappers, plus a standalone target.

## Roles

| Repo | Role |
|------|------|
| `soemdsp-sandbox` | Patch lab / editor |
| `soemdsp-sandbox-audioplugin` | Plugin + standalone host (**this repo**) |

`soemdsp-sandbox-standalone` is superseded by the standalone target here.

## Product rules

- Fixed **32** host-facing parameters.
- Slots bind to existing sandbox **Knob** modules (exclusive ownership).
- Mouse, performance UI, and DAW automation share that Knob value.

## Status

- [x] Bootstrap clap-first template (renamed from SideQuest starting point)
- [ ] Windows Debug/Release smoke (CLAP + VST3 + standalone)
- [ ] Stereo audio effect ports (sandbox patch player, not a synth)
- [ ] Link sandbox native graph runtime
- [ ] Load a sandbox patch + map 32 Knobs

## Build (Windows)

Requires CMake ≥ 3.28, a C++ toolchain (MSVC or MinGW), and the vendored `libs/` tree (already populated from the template).

```powershell
cd soemdsp-sandbox-audioplugin
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

Or Ninja + clang/MSVC as you prefer. Artifacts land under `build/` (and copy-after-build into the usual clap/vst3 folders when enabled).

## License

Template / SST / clap-wrapper stack: MIT sources with **GPL3** combined work (JUCE dependency). See `LICENSE` / `LICENSE.md`.
