#pragma once
// C API from soemdsp-sandbox native_modules/graph_engine (subset used by the plugin host).

#ifdef __cplusplus
extern "C" {
#endif

int soemdsp_graph_create();
void soemdsp_graph_destroy(int handle);
void soemdsp_graph_clear(int handle);
void soemdsp_graph_set_sample_rate(int handle, float sampleRate);
// tempoBpm <= 1 keeps the patch BPM. lockPosition snaps masterSamples to the project playhead.
void soemdsp_graph_set_host_transport(int handle, double tempoBpm, double positionSamples, int lockPosition);
void soemdsp_graph_set_gravity_walker_chunks(int handle, double c0, double c1, double c2);
int soemdsp_graph_add_param_mod_edge(int handle, unsigned int srcHash, int srcPort,
                                     unsigned int dstHash, int paramId);
int soemdsp_graph_set_param_domain(int handle, unsigned int nodeHash, int paramId,
                                   float minV, float maxV, int flags);
int soemdsp_graph_add_node(int handle, unsigned int nodeIdHash, int typeId);
int soemdsp_graph_connect(int handle, unsigned int srcHash, int srcPort, unsigned int dstHash,
                          int dstPort);
int soemdsp_graph_set_param(int handle, unsigned int nodeHash, int paramId, double value);
int soemdsp_graph_snap_controls(int handle);
int soemdsp_graph_compile(int handle);
int soemdsp_graph_process_block(int handle, int n);
double *soemdsp_graph_block_output_left_ptr(int handle);
double *soemdsp_graph_block_output_right_ptr(int handle);
double *soemdsp_graph_node_port_ptr(int handle, unsigned int nodeHash, int port);
int soemdsp_graph_max_block_frames();

double soemdsp_gain_sample(double channel, double mono, double left, double right, double masterDb,
                           double leftDb, double rightDb, double monoSum, double offset);

#ifdef __cplusplus
}
#endif

// Keep in sync with graph_engine.cpp / NATIVE_GRAPH_TYPE_IDS
enum
{
    kSoemTypeOutput = 6,
    kSoemTypeGain = 13,
    kSoemTypeAudioInput = 132,
    kSoemPortMono = 0,
    kSoemPortLeft = 1,
    kSoemPortRight = 2,
    kSoemParamVolumeDb = 0,
};
