# Audio plugin shell architecture

## Boundary

- **In this repo:** CLAP/VST3 entry points, host audio/MIDI callbacks, plugin state save/load, DAW parameter surface (32), packaging.
- **Not in this repo:** modular editor and full sandbox chrome (lives in `soemdsp-sandbox`).
- **Shared (to be linked):** graph engine / native modules / patch load / Knob to slot mapping (same as standalone).

## Parameter surface

- Always **32** slots toward the host (empty slots remain reserved so automation maps stay stable).
- Assignment is exclusive: binding slot N to Knob B clears it from Knob A.
- Host automation writes the Knob value; patch wires decide what that CV drives.

## Relationship to standalone

Same runtime contract. Standalone is the easier first host (no third-party DAW). This repo adds format SDKs and host politics on top.
