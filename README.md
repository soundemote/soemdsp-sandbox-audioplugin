# soemdsp-sandbox-audioplugin

**DAW plugin shell** (VST3 / CLAP) for running soemdsp-sandbox patches inside a host.

This is **not** a fork of [soemdsp-sandbox](https://github.com/soundemote/soemdsp-sandbox). Authoring stays in the sandbox; this repo owns the plugin wrapper (host entry points, state chunks, installers) around a **shared runtime**.

Sibling: [soemdsp-sandbox-standalone](https://github.com/soundemote/soemdsp-sandbox-standalone) (desktop app).

## Roles

| Repo | Role |
|------|------|
| `soemdsp-sandbox` | Patch lab / editor |
| `soemdsp-sandbox-standalone` | Desktop runtime shell |
| `soemdsp-sandbox-audioplugin` | DAW plugin shell (this repo) |

## Product rules (locked in design)

- Fixed **32** plugin parameters visible to the DAW.
- Slots bind to existing **Knob** modules (no separate HostKnob type).
- **Exclusive** slot ownership (one Knob per slot).
- Mouse, performance UI, and DAW automation share that Knob's value (one value, several surfaces).
- Wire Knobs into the graph explicitly; v1 does not pin arbitrary face params into the 32.

## Layout (starter)

```
plugin/       # VST3 / CLAP / wrapper (framework TBD)
runtime/      # shared graph runtime (submodule or package — TBD)
docs/         # host / format notes
```

Do **not** vendor the full sandbox `public/` tree here.

## Status

Empty skeleton. Prefer proving the shared runtime in **standalone** first, then wrap the same runtime here.
