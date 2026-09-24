// Generate plugin wire tables from the sandbox worklet.
// Source of truth: public/node-live-audio-worklet-native-graph.js
// Run: node tools/gen-native-wire-tables.mjs
import fs from "fs";
import path from "path";
import vm from "vm";
import { fileURLToPath } from "url";

const here = path.dirname(fileURLToPath(import.meta.url));
const pluginRoot = path.resolve(here, "..");
const sandboxRoot = path.resolve(pluginRoot, "..", "soemdsp-sandbox");
const workletPath = path.join(sandboxRoot, "public", "node-live-audio-worklet-native-graph.js");
const outPath = path.join(pluginRoot, "runtime", "native_wire_tables.gen.h");

const src = fs.readFileSync(workletPath, "utf8");

function NodeLiveAudioProcessor() {}
const context = { NodeLiveAudioProcessor, console };
vm.createContext(context);
vm.runInContext(src, context, { filename: workletPath });
const P = context.NodeLiveAudioProcessor;
const mapper = new P();

const typeIds = P.NATIVE_GRAPH_TYPE_IDS;
const types = Object.keys(typeIds).sort();

const portNames = new Set();
const portFn = src.slice(
  src.indexOf("prototype.mapNativeGraphSrcPortId"),
  src.indexOf("prototype.pushNativeGraphParam"),
);
for (const m of portFn.matchAll(/=== "([^"]+)"/g)) portNames.add(m[1].toLowerCase());
const patchDir = path.join(sandboxRoot, "patches");
if (fs.existsSync(patchDir)) {
  for (const name of fs.readdirSync(patchDir)) {
    if (!name.endsWith(".json")) continue;
    let text;
    try { text = fs.readFileSync(path.join(patchDir, name), "utf8"); } catch { continue; }
    for (const m of text.matchAll(/"(?:source|destination)Port"\s*:\s*"([^"]*)"/g)) {
      const p = m[1].trim().toLowerCase();
      if (p) portNames.add(p);
    }
  }
}

function portId(kind, type, port) {
  const fn = kind === "src" ? mapper.mapNativeGraphSrcPortId : mapper.mapNativeGraphDstPortId;
  const id = fn.call(mapper, port, type);
  return Number.isFinite(id) ? id | 0 : 0;
}

const generic = new Map();
for (const port of portNames) {
  generic.set(port, { src: portId("src", "", port), dst: portId("dst", "", port) });
}
const portRows = [];
for (const type of types) {
  for (const port of portNames) {
    const srcId = portId("src", type, port);
    const dstId = portId("dst", type, port);
    const g = generic.get(port);
    if (srcId !== g.src || dstId !== g.dst) {
      portRows.push({ type, port, src: srcId, dst: dstId });
    }
  }
}
portRows.sort((a, b) => a.type.localeCompare(b.type) || a.port.localeCompare(b.port));
const genericRows = [...generic.entries()].sort((a, b) => a[0].localeCompare(b[0]));

function extractFunction(name) {
  const marker = `prototype.${name} = function`;
  const start = src.indexOf(marker);
  if (start < 0) throw new Error(`missing ${name}`);
  const brace = src.indexOf("{", start);
  let depth = 0;
  for (let i = brace; i < src.length; i += 1) {
    if (src[i] === "{") depth += 1;
    else if (src[i] === "}") {
      depth -= 1;
      if (depth === 0) return src.slice(brace + 1, i);
    }
  }
  throw new Error(`unclosed ${name}`);
}

const paramBody = extractFunction("syncNativeGraphParams");
const paramRows = [];
const lines = paramBody.split(/\r?\n/);
let cond = "";
let collectingCond = false;
let blockTypes = [];
let depth = 0;
let blockDepth = -1;

function typesIn(text) {
  return [...text.matchAll(/type === "([^"]+)"/g)].map((m) => m[1]);
}

function pushLiteral(text, typesForPush) {
  const re = /push\(\s*"([^"]+)"\s*,\s*P\.([A-Z0-9_]+)/g;
  let m;
  while ((m = re.exec(text))) {
    const id = P[m[2]];
    if (!Number.isFinite(id)) throw new Error(`unknown param ${m[2]}`);
    for (const type of typesForPush) paramRows.push({ type, key: m[1], id: id | 0 });
  }
}

for (const line of lines) {
  if (collectingCond) {
    cond += `\n${line}`;
    if (line.includes("{")) {
      collectingCond = false;
      blockTypes = typesIn(cond);
      blockDepth = depth;
      depth += (line.match(/\{/g) || []).length;
      depth -= (line.match(/\}/g) || []).length;
      if (blockTypes.length) pushLiteral(line, blockTypes);
      if (depth <= blockDepth) {
        blockTypes = [];
        blockDepth = -1;
      }
      continue;
    }
  }
  const opens = (line.match(/\{/g) || []).length;
  const closes = (line.match(/\}/g) || []).length;
  if (/if\s*\(/.test(line) && /type ===/.test(line)) {
    cond = line;
    if (!line.includes("{")) {
      collectingCond = true;
      continue;
    }
    blockTypes = typesIn(line);
    blockDepth = depth;
  }
  if (blockTypes.length && blockDepth >= 0 && depth >= blockDepth) {
    pushLiteral(line, blockTypes);
    if (blockTypes.includes("graphicEq") && line.includes("band0 + i")) {
      const band0 = P.NATIVE_GRAPH_PARAM_GRAPHIC_EQ_BAND0 | 0;
      for (let i = 0; i < 30; i += 1) {
        paramRows.push({ type: "graphicEq", key: `band${i}`, id: band0 + i });
      }
    }
  }
  depth += opens - closes;
  if (blockDepth >= 0 && depth <= blockDepth) {
    blockTypes = [];
    blockDepth = -1;
  }
}

const paramSeen = new Set();
const params = [];
for (const row of paramRows) {
  const k = `${row.type}\0${row.key}`;
  if (paramSeen.has(k)) continue;
  paramSeen.add(k);
  params.push(row);
}
params.sort((a, b) => a.type.localeCompare(b.type) || a.key.localeCompare(b.key));

const required = [
  ["polyBlep", "frequency"],
  ["polyBlep", "amplitude"],
  ["gravityWalker", "gravity"],
  ["gravityWalker", "steps"],
  ["pitchQuantizer", "scaleMask"],
  ["pitchQuantizer", "octaves"],
  ["pitchQuantizer", "octaveOffset"],
  ["pluckEnvelope3", "attack"],
  ["pluckEnvelope3", "decay"],
  ["passiveFilter", "highFrequency"],
  ["passiveFilter", "sweep"],
  ["transport", "bpm"],
  ["transport", "timeNumerator"],
  ["ampCurve", "mode"],
  ["output", "volume"],
];
for (const [type, key] of required) {
  if (!params.some((r) => r.type === type && r.key === key)) {
    throw new Error(`generator missed ${type}.${key}`);
  }
}

function cString(s) {
  return `"${s.replace(/\\/g, "\\\\").replace(/"/g, '\\"')}"`;
}

let h = "";
h += "// Generated by tools/gen-native-wire-tables.mjs from the sandbox worklet.\n";
h += "// Do not edit. Re-run the script after the worklet port or parameter tables change.\n";
h += "#pragma once\n#include <cstring>\n\n";
h += "struct SoemWireType { const char* name; int id; };\n";
h += "struct SoemWirePort { const char* type; const char* port; int src; int dst; };\n";
h += "struct SoemWireParam { const char* type; const char* key; int id; };\n\n";

h += `static const SoemWireType kSoemWireTypes[] = {\n`;
for (const type of types) h += `  {${cString(type)}, ${typeIds[type] | 0}},\n`;
h += "};\n\n";

h += "static const SoemWirePort kSoemWirePortsGeneric[] = {\n";
for (const [port, ids] of genericRows) {
  h += `  {"", ${cString(port)}, ${ids.src}, ${ids.dst}},\n`;
}
h += "};\n\n";

h += "static const SoemWirePort kSoemWirePortsOverride[] = {\n";
for (const row of portRows) {
  h += `  {${cString(row.type)}, ${cString(row.port)}, ${row.src}, ${row.dst}},\n`;
}
h += "};\n\n";

h += "static const SoemWireParam kSoemWireParams[] = {\n";
for (const row of params) {
  h += `  {${cString(row.type)}, ${cString(row.key)}, ${row.id}},\n`;
}
h += "};\n\n";

h += `static int soemWireCmp2(const char* a, const char* b, const char* c, const char* d) {
  const int t = std::strcmp(a, c);
  if (t != 0) return t;
  return std::strcmp(b, d);
}
static int soemdsp_wire_type_id(const char* type) {
  if (!type) return 0;
  int lo = 0;
  int hi = (int)(sizeof(kSoemWireTypes) / sizeof(kSoemWireTypes[0])) - 1;
  while (lo <= hi) {
    const int mid = (lo + hi) >> 1;
    const int c = std::strcmp(type, kSoemWireTypes[mid].name);
    if (c == 0) return kSoemWireTypes[mid].id;
    if (c < 0) hi = mid - 1;
    else lo = mid + 1;
  }
  return 0;
}
static const SoemWirePort* soemWireFindPort(const SoemWirePort* rows, int n, const char* type, const char* port) {
  int lo = 0;
  int hi = n - 1;
  while (lo <= hi) {
    const int mid = (lo + hi) >> 1;
    const int c = soemWireCmp2(type, port, rows[mid].type, rows[mid].port);
    if (c == 0) return &rows[mid];
    if (c < 0) hi = mid - 1;
    else lo = mid + 1;
  }
  return nullptr;
}
static int soemdsp_wire_port(const char* type, const char* port, bool destination) {
  if (!port || !port[0]) return 0;
  const int nOver = (int)(sizeof(kSoemWirePortsOverride) / sizeof(kSoemWirePortsOverride[0]));
  const SoemWirePort* over = soemWireFindPort(kSoemWirePortsOverride, nOver, type ? type : "", port);
  if (over) return destination ? over->dst : over->src;
  const int nGen = (int)(sizeof(kSoemWirePortsGeneric) / sizeof(kSoemWirePortsGeneric[0]));
  const SoemWirePort* gen = soemWireFindPort(kSoemWirePortsGeneric, nGen, "", port);
  if (gen) return destination ? gen->dst : gen->src;
  return 0;
}
static int soemdsp_wire_param_id(const char* type, const char* key) {
  if (!type || !key) return -1;
  int lo = 0;
  int hi = (int)(sizeof(kSoemWireParams) / sizeof(kSoemWireParams[0])) - 1;
  while (lo <= hi) {
    const int mid = (lo + hi) >> 1;
    const int c = soemWireCmp2(type, key, kSoemWireParams[mid].type, kSoemWireParams[mid].key);
    if (c == 0) return kSoemWireParams[mid].id;
    if (c < 0) hi = mid - 1;
    else lo = mid + 1;
  }
  return -1;
}
`;

fs.writeFileSync(outPath, h);
console.log(`types ${types.length} genericPorts ${genericRows.length} overrides ${portRows.length} params ${params.length}`);
console.log(`wrote ${outPath}`);

const dreamPath = path.join(patchDir, "analogdream.json");
if (fs.existsSync(dreamPath)) {
  const dream = JSON.parse(fs.readFileSync(dreamPath, "utf8"));
  const typeById = new Map(dream.nodes.map((n) => [n.id, n.type]));
  const paramOf = (type, key) => params.find((r) => r.type === type && r.key === key);
  console.log("--- analogdream connections ---");
  for (const c of dream.connections || []) {
    const st = typeById.get(c.sourceNode) || "";
    const dt = typeById.get(c.destinationNode) || "";
    const s = portId("src", st, c.sourcePort);
    const d = portId("dst", dt, c.destinationPort);
    console.log(`${c.sourceNode}:${c.sourcePort} (${st}) p${s} -> ${c.destinationNode}:${c.destinationPort} (${dt}) p${d}`);
  }
  console.log("--- analogdream modulations ---");
  for (const m of dream.modulations || []) {
    const st = typeById.get(m.sourceNode) || "";
    const dt = typeById.get(m.destinationNode) || "";
    const s = portId("src", st, m.sourcePort);
    const hit = paramOf(dt, m.destinationParam);
    console.log(`${m.sourceNode}:${m.sourcePort} (${st}) p${s} -> ${m.destinationNode}.${m.destinationParam} (${dt}) id ${hit ? hit.id : "MISSING"}`);
  }
  for (const n of dream.nodes) {
    const keys = Object.keys(n.params || {});
    const missing = keys.filter((k) => !paramOf(n.type, k));
    if (missing.length && typeIds[n.type]) console.log(`params not pushed ${n.id} (${n.type}): ${missing.join(", ")}`);
  }
}
