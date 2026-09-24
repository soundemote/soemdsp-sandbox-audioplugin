#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <memory>

namespace soemdsp_ap
{

struct GraphNodeSpec
{
    uint32_t hash{0};
    int typeId{0};
};
struct GraphConnSpec
{
    uint32_t srcHash{0};
    uint32_t dstHash{0};
    int srcPort{0};
    int dstPort{0};
};
struct GraphParamSpec
{
    uint32_t hash{0};
    int paramId{0};
    double value{0};
};
struct GraphModSpec
{
    uint32_t srcHash{0};
    uint32_t dstHash{0};
    int srcPort{0};
    int paramId{0};
};
struct GraphDomainSpec
{
    uint32_t hash{0};
    int paramId{0};
    float minV{0};
    float maxV{0};
    int flags{0};
};
enum class CtrlFace : uint8_t
{
    None = 0,
    Knob,
    Slider,
    Toggle,
    Momentary
};

struct FaceLook
{
    uint32_t bg{0xff000000};
    uint32_t arcFill{0xfff1b84b};
    uint32_t arcTrack{0xff3a3428};
    uint32_t sliderColor{0xff5491ab};
    uint32_t numberColor{0xffffffff};
    uint32_t textColor{0xffcad3d8};
    uint32_t unitColor{0xff7fc7d9};
    uint32_t stroke{0xff5c5071};
    uint32_t inactive{0xff1a2228};
    uint32_t active{0xff2f8f86};
    uint32_t hover{0xff89bfc2};
    float rotationDegrees{270.f};
    float dialSize{1.f};
    float labelSize{0.2f};
    float valueSize{0.2f};
    float innerRadius{0.7f};
    float sliderLength{1.f};
    float sliderHeight{0.22f};
    float sliderPadding{0.f};
    float sliderRounding{0.5f};
    float strokeScale{0.06f};
    float buttonScale{1.f};
    float textScale{0.72f};
    float padding{0.f};
    float labelPad{0.04f};
    float labelScale{0.22f};
    float numberPad{0.f};
    float numberScale{0.22f};
    float unitPad{0.04f};
    float unitScale{0.18f};
    int decimals{2};
    std::string labelPos{"above"};
    std::string valuePos{"mid"};
    std::string sliderAlign{"mid"};
    std::string cornerShape{"squircle"};
    std::string buttonAlign{"mid"};
    std::string labelAlign{"topleft"};
    std::string numberAlign{"mid"};
    std::string unitAlign{"topright"};
    std::string displayName;
    std::string unit;
    std::string offLabel{"Off"};
    std::string onLabel{"On"};
    bool showLabel{true};
    bool showNumber{true};
    bool showUnit{true};
};

struct DrawNode
{
    float gx{0}, gy{0};
    float wGu{8}, hGu{6};
    std::string id, type, label;
    uint32_t hash{0};
    bool isKnob{false};
    CtrlFace ctrl{CtrlFace::None};
    int slot{-1}; // 0..31 if performance control
    FaceLook look;
};
struct DrawWire
{
    uint32_t srcHash{0};
    uint32_t dstHash{0};
};
struct SlotBind
{
    int slot{0};
    uint32_t dstHash{0};
    uint32_t knobHash{0};
    int paramId{0};
    double minV{0}, maxV{1};
    double initial01{0};
    std::string folder;
    std::string name;
};

struct GraphPlan
{
    std::string name;
    std::string error;
    int added{0};
    int skipped{0};
    std::vector<GraphNodeSpec> nodes;
    std::vector<GraphConnSpec> conns;
    std::vector<GraphParamSpec> params;
    std::vector<GraphModSpec> mods;
    std::vector<GraphDomainSpec> domains;
    std::vector<uint32_t> hostInputHashes;
    std::vector<DrawNode> drawNodes;
    std::vector<DrawWire> drawWires;
    std::vector<SlotBind> slots;
};

// Parse sandbox JSON (raw patch or kind:sandbox_patch wrapper). Uses JUCE JSON.
GraphPlan parseSandboxPatchJson(const std::string &jsonText, const std::string &displayName);

struct SandboxGraph
{
    int handle{0};
    bool ready{false};
    float sampleRate{44100.f};
    unsigned int inHash{0xA11D0001u};
    unsigned int gainHash{0xA11D0002u};
    unsigned int outHash{0xA11D0003u};
    std::vector<uint32_t> hostInputHashes;
    std::string status{"AudioInput -> Gain -> Output"};

    std::mutex pendingMutex;
    std::shared_ptr<GraphPlan> pendingPlan;
    std::atomic<bool> hasPendingPlan{false};
    std::string lastStatus;
    std::vector<SlotBind> slots;
    std::shared_ptr<GraphPlan> displayPlan;

    void setup(float sr);
    void shutdown();
    bool applyPlan(const GraphPlan &plan);
    void queuePlan(std::shared_ptr<GraphPlan> plan);
    void process(const float *inL, const float *inR, float *destL, float *destR, int n);
    void setSlot(int slot, double unit01);
    // tempoBpm <= 1 keeps the patch BPM. lockPosition writes the playhead in samples.
    void setHostTransport(double tempoBpm, double positionSamples, bool lockPosition);
    void setGravityWalkerChunks(double c0, double c1, double c2);
};

} // namespace soemdsp_ap
