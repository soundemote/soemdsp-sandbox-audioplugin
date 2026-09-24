# Audio plugin shell architecture

## Boundary

- **In this repo:** CLAP/VST3/AU entry points, standalone target, host audio/MIDI callbacks, plugin state, DAW parameter surface (32), packaging, UI shell.
- **Not in this repo:** modular editor and full sandbox chrome (lives in `soemdsp-sandbox`).
- **Shared (to be linked):** graph engine / native modules / patch load / Knob→slot mapping.

## Formats

Clap-first (BaconPaul / free-audio **clap-wrapper**):

- **CLAP** — native
- **VST3 / AUv2** — wrappers
- **Standalone** — same plugin ID, app target from `STANDALONE_CONFIGURATIONS`

## Parameter surface

- Always **32** slots toward the host (empty slots stay reserved so automation maps stay stable).
- Assignment is exclusive: binding slot N to Knob B clears it from any other slot.
- Host automation writes the Knob value; patch wires decide what that CV drives.

## Audio role

v1 is an **audio effect** (stereo in / stereo out) that will run a loaded sandbox patch.  
MIDI note ports may remain for later instrument-style patches.

## Milestone path

1. Toolchain smoke — build CLAP/VST3/standalone (passthrough audio)
2. Native sandbox **Gain** module in the audio callback
3. Full `graph_engine` via clang-cl (current): AudioInput -> Gain -> Output
4. Load one sandbox patch JSON
5. Map 32 Knob slots to DAW params
6. Minimal performance UI
