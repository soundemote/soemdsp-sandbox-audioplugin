// Auto-generated stubs for graph_engine symbols not compiled in v1.
#include <cstdint>
#include <cstring>
extern "C" {
int soemdsp_ladder_filter_create() { return 0; }

void soemdsp_ladder_filter_destroy(int handle) {}

void soemdsp_ladder_filter_set_params(
  int handle, double frequency, double resonance, int mode, int stages, double sampleRate
) {}

double soemdsp_ladder_filter_sample(
  int handle, double input, double frequency, double resonance,
  int mode, int stages, double sampleRate
) { return 0.0; }

void soemdsp_ladder_filter_process_block(int handle, int frameCount) {}

int soemdsp_ladder_filter_block_input_ptr(int handle) { return 0; }

int soemdsp_ladder_filter_block_output_ptr(int handle) { return 0; }

int soemdsp_soft_clipper_create() { return 0; }

void soemdsp_soft_clipper_destroy(int handle) {}

void soemdsp_soft_clipper_set_params(
  int handle, double center, double width, double antialias, int oversampleMode
) {}

void soemdsp_soft_clipper_process_block(int handle, int channel, int frameCount) {}

int soemdsp_soft_clipper_block_input_ptr(int handle, int channel) { return 0; }

int soemdsp_soft_clipper_block_output_ptr(int handle, int channel) { return 0; }

int soemdsp_ping_pong_delay_create() { return 0; }

void soemdsp_ping_pong_delay_destroy(int handle) {}

void soemdsp_ping_pong_delay_reset(int handle) {}

void soemdsp_ping_pong_delay_set_params(
  int handle,
  double feedback, double mix, double amplitude,
  double timeNumerator, double timeDenominator, double timingMode,
  double offsetMs, double lfoAmpMs, double lfoStyle, double lfoRate, double lfoVariation,
  double saturate, double lpfFrequency, double hpfFrequency,
  double tempoBpm, double sampleRate
) {}

double soemdsp_ping_pong_delay_sample(
  int handle, double inputL, double inputR,
  double feedback, double mix, double amplitude,
  double timeNumerator, double timeDenominator, double timingMode,
  double offsetMs, double lfoAmpMs, double lfoStyle, double lfoRate, double lfoVariation,
  double saturate, double lpfFrequency, double hpfFrequency,
  double tempoBpm, double sampleRate
) { return 0.0; }

double soemdsp_ping_pong_delay_right(int handle) { return 0.0; }

double soemdsp_ping_pong_delay_mod_left(int handle) { return 0.0; }

double soemdsp_ping_pong_delay_mod_right(int handle) { return 0.0; }

void soemdsp_ping_pong_delay_process_block(int handle, int frameCount) {}

int soemdsp_ping_pong_delay_block_input_ptr(int handle) { return 0; }

int soemdsp_ping_pong_delay_block_output_left_ptr(int handle) { return 0; }

int soemdsp_ping_pong_delay_block_output_right_ptr(int handle) { return 0; }

int soemdsp_ping_pong_delay_block_output_mod_left_ptr(int handle) { return 0; }

int soemdsp_ping_pong_delay_block_output_mod_right_ptr(int handle) { return 0; }

int soemdsp_attenuverter_create() { return 0; }

void soemdsp_attenuverter_destroy(int handle) {}

void soemdsp_attenuverter_set_params(int handle, double amplitude, double offset) {}

void soemdsp_attenuverter_process_block(int handle, int frameCount) {}

int soemdsp_attenuverter_block_input_ptr(int handle) { return 0; }

int soemdsp_attenuverter_block_output_ptr(int handle) { return 0; }

int soemdsp_raster_rgb_create() { return 0; }

void soemdsp_raster_rgb_destroy(int handle) {}

double soemdsp_raster_rgb_sample(
  int handle,
  double r, double g, double b,
  double invert, double contrast, double brightness, double hue
) { return 0.0; }

double soemdsp_raster_rgb_r(int handle) { return 0.0; }

double soemdsp_raster_rgb_g(int handle) { return 0.0; }

double soemdsp_raster_rgb_b(int handle) { return 0.0; }

double soemdsp_raster_rgb_rgba(int handle) { return 0.0; }

int soemdsp_range_create() { return 0; }

void soemdsp_range_destroy(int handle) {}

void soemdsp_range_set_params(
  int handle, double inLow, double inHigh, double outLow, double outHigh
) {}

void soemdsp_range_process_block(int handle, int frameCount) {}

int soemdsp_range_block_input_ptr(int handle) { return 0; }

int soemdsp_range_block_output_ptr(int handle) { return 0; }

int soemdsp_noise_generator_create() { return 0; }

void soemdsp_noise_generator_destroy(int handle) {}

void soemdsp_noise_generator_process_block(
  int handle,
  double seedValue,
  int mode,
  double mean,
  double deviation,
  double shape,
  double level,
  int frameCount,
  int useSimd
) {}

int soemdsp_noise_generator_block_output_left_ptr(int handle) { return 0; }

int soemdsp_noise_generator_block_output_right_ptr(int handle) { return 0; }

int soemdsp_robin_sinusoid_create() { return 0; }

void soemdsp_robin_sinusoid_destroy(int handle) {}

void soemdsp_robin_sinusoid_reset(int handle) {}

double soemdsp_robin_sinusoid_sample(
  int handle, double frequencyHz, double amplitude, double sampleRate,
  double startPhaseRadians, double reset
) { return 0.0; }

void soemdsp_robin_sinusoid_process_block(
  int handle, double frequencyHz, double amplitude, double sampleRate,
  double startPhaseRadians, double reset, int frameCount
) {}

int soemdsp_robin_sinusoid_block_output_ptr(int handle) { return 0; }

int soemdsp_robin_supersaw_create() { return 0; }

void soemdsp_robin_supersaw_destroy(int handle) {}

void soemdsp_robin_supersaw_reset(int handle) {}

void soemdsp_robin_supersaw_process_block(
  int handle, double frequencyHz, double sampleRate, double detuneCents,
  double voicesExact, double level, double phaseSpread, double stereoMode,
  double detuneAlgorithm, double portaTimeMin, double portaTimeMax,
  double portamentoStyle,
  double jitterSpeed, double jitterDepth, double jitterFilter, double jitterSteps,
  double detuneTilt, double maxVoiceHz, double resetGate, int frameCount
) {}

int soemdsp_robin_supersaw_block_output_left_ptr(int handle) { return 0; }

int soemdsp_robin_supersaw_block_output_right_ptr(int handle) { return 0; }

int soemdsp_robin_supersaw_block_output_mono_ptr(int handle) { return 0; }

int soemdsp_robin_supersaw_voice_count(int handle) { return 0; }

double soemdsp_robin_supersaw_voice_x(int handle, int index) { return 0.0; }

double soemdsp_robin_supersaw_voice_pan(int handle, int index) { return 0.0; }

double soemdsp_robin_supersaw_voice_amp(int handle, int index) { return 0.0; }

int soemdsp_slew_limiter_create() { return 0; }

void soemdsp_slew_limiter_destroy(int handle) {}

void soemdsp_slew_limiter_process_block(
  int handle, double upTime, double downTime, double upShape, double downShape, double bias,
  double sampleRate, int frameCount
) {}

int soemdsp_slew_limiter_block_input_ptr(int handle) { return 0; }

int soemdsp_slew_limiter_block_output_ptr(int handle) { return 0; }

int soemdsp_comparator_create() { return 0; }

void soemdsp_comparator_destroy(int handle) {}

double soemdsp_comparator_sample(int handle, double signalIn) { return 0.0; }

double soemdsp_comparator_up(int handle) { return 0.0; }

double soemdsp_comparator_down(int handle) { return 0.0; }

double soemdsp_comparator_change(int handle) { return 0.0; }

double soemdsp_comparator_steady(int handle) { return 0.0; }

double soemdsp_comparator_sign(int handle) { return 0.0; }

double soemdsp_comparator_thru(int handle) { return 0.0; }

int soemdsp_sample_delay_create() { return 0; }

void soemdsp_sample_delay_destroy(int handle) {}

double soemdsp_sample_delay_sample(
  int handle, double input, double timeSeconds, double samplesParam, double sampleRate
) { return 0.0; }

int soemdsp_min_max_create() { return 0; }

void soemdsp_min_max_destroy(int handle) {}

double soemdsp_min_max_sample(
  int handle, double in1, double in2, double in3, double in4, int connectedMask
) { return 0.0; }

double soemdsp_min_max_min(int handle) { return 0.0; }

double soemdsp_mix_sample(
  double channel,
  double in1, double in2, double in3, double in4,
  double volume1, double volume2, double volume3, double volume4,
  double bias1, double bias2, double bias3, double bias4,
  double bleed2to1, double bleed3to1, double bleed4to1
) { return 0.0; }

double soemdsp_mix_stereo_sample(
  double channel,
  double l1, double r1, double l2, double r2, double l3, double r3, double l4, double r4,
  double mono,
  double vol1, double pan1, double vol2, double pan2, double vol3, double pan3, double vol4, double pan4,
  double amplitude
) { return 0.0; }

int soemdsp_clipper_limiter_create() { return 0; }

void soemdsp_clipper_limiter_destroy(int handle) {}

double soemdsp_clipper_limiter_sample(
  int handle, int channel, double input, double minDb, double maxDb, double gainDb, double antialias
) { return 0.0; }

double soemdsp_mid_side_encode_sample(
  double channel, double left, double right, double midGainDb, double sideGainDb
) { return 0.0; }

double soemdsp_vectorscope_transform_sample(
  double channel, double left, double right, double rotateDeg
) { return 0.0; }

double soemdsp_rotate_3d_to_2d_sample(
  double channel, double x, double y, double z,
  double rotateXCycles, double rotateYCycles, double rotateZCycles
) { return 0.0; }

int soemdsp_clock_create() { return 0; }

void soemdsp_clock_destroy(int handle) {}

double soemdsp_clock_sample(
  int handle, double reset, double phaseOffset, double rate, double duty,
  double level, double sampleRate
) { return 0.0; }

double soemdsp_clock_analog_out(int handle) { return 0.0; }

double soemdsp_clock_pulse(int handle) { return 0.0; }

int soemdsp_trigger_divider_create() { return 0; }

void soemdsp_trigger_divider_destroy(int handle) {}

double soemdsp_trigger_divider_sample_clock(
  int handle, double clock, double reset, double threshold, double division,
  double duty, double level, double sampleRate
) { return 0.0; }

double soemdsp_trigger_divider_sample(
  int handle, double trigger, double reset, double threshold, double division,
  double pulseTime, double level, double sampleRate
) { return 0.0; }

int soemdsp_delayed_trigger_create() { return 0; }

void soemdsp_delayed_trigger_destroy(int handle) {}

double soemdsp_delayed_trigger_sample(
  int handle, double trigger, double reset, double threshold, double delay,
  double pulseTime, double level, double sampleRate
) { return 0.0; }

int soemdsp_random_clock_create() { return 0; }

void soemdsp_random_clock_destroy(int handle) {}

double soemdsp_random_clock_sample(
  int handle, double reset, double threshold, double minSeconds, double maxSeconds,
  double duty, double triggerTime, double level, double sampleRate, int seedKey
) { return 0.0; }

double soemdsp_random_clock_gate(int handle) { return 0.0; }

int soemdsp_trigger_counter_create() { return 0; }

void soemdsp_trigger_counter_destroy(int handle) {}

double soemdsp_trigger_counter_sample(
  int handle, double trigger, double reset, double threshold, double countMax,
  double increment, double pulseTime, double level, double sampleRate
) { return 0.0; }

double soemdsp_trigger_counter_count(int handle) { return 0.0; }

double soemdsp_metallic_ratio_sample(double index) { return 0.0; }

double soemdsp_harmonic_series_sample(double baseHz, double harmonic, double offset) { return 0.0; }

int soemdsp_lut_cell_create() { return 0; }

void soemdsp_lut_cell_destroy(int handle) {}

int soemdsp_lut_cell_sample(
  int handle, double a, double b, double c, double d, double clock, double truthTable
) { return 0; }

int soemdsp_lut_cell_q(int handle) { return 0; }

int soemdsp_lookahead_limiter_create() { return 0; }

void soemdsp_lookahead_limiter_destroy(int handle) {}

double soemdsp_lookahead_limiter_sample(
  int handle, double left, double right,
  double ceilingDb, double lookaheadMs, double lookaheadSamples,
  double attackMs, double releaseMs, double sampleRate,
  double lookaheadEnabled, double gainCompensation, double dipGain
) { return 0.0; }

double soemdsp_lookahead_limiter_left(int handle) { return 0.0; }

double soemdsp_lookahead_limiter_right(int handle) { return 0.0; }

double soemdsp_lookahead_limiter_gain(int handle) { return 0.0; }

int soemdsp_pumping_limiter_create() { return 0; }

void soemdsp_pumping_limiter_destroy(int handle) {}

double soemdsp_pumping_limiter_sample(
  int handle, double left, double right, double sidechain, int hasSidechain,
  double inputGainDb, double thresholdDb, double ratio,
  double lookaheadMs, double lookaheadSamples,
  double attackMs, double releaseMs, double sampleRate,
  double lookaheadEnabled, double amplitude
) { return 0.0; }

double soemdsp_pumping_limiter_left(int handle) { return 0.0; }

double soemdsp_pumping_limiter_right(int handle) { return 0.0; }

double soemdsp_pumping_limiter_gain(int handle) { return 0.0; }

double soemdsp_pumping_limiter_env(int handle) { return 0.0; }

int soemdsp_audio_player_create() { return 0; }

void soemdsp_audio_player_destroy(int handle) {}

double soemdsp_audio_player_sample(
  int handle,
  double reset, double speedCv, double phaseCv, int hasPhase,
  double transportMode, double speedParam, double start, double end,
  double amplitude, double phaseOffset, double phaseSkip, double playlistScrub,
  double antialias, double engineSampleRate
) { return 0.0; }

double soemdsp_audio_player_left(int handle) { return 0.0; }

double soemdsp_audio_player_right(int handle) { return 0.0; }

double soemdsp_audio_player_phase(int handle) { return 0.0; }

double soemdsp_audio_player_trigger(int handle) { return 0.0; }

int soemdsp_alias_sine_create() { return 0; }

void soemdsp_alias_sine_destroy(int handle) {}

double soemdsp_alias_sine_sample(
  int handle, double normFreq, double level, double sampleRate
) { return 0.0; }

int soemdsp_phone_tone_create() { return 0; }

void soemdsp_phone_tone_destroy(int handle) {}

double soemdsp_phone_tone_sample(
  int handle,
  double sampleRate,
  double amplitude,
  double pitchOffsetOctaves,
  double freqOffsetHz,
  double pitchCv,
  double hasPitchCv,
  double analog,
  double hasAnalog,
  double digital,
  double hasDigital,
  double gate,
  double hasGate,
  double referenceVoltage
) { return 0.0; }

double soemdsp_phone_tone_tone(int handle) { return 0.0; }

double soemdsp_phone_tone_tone_l(int handle) { return 0.0; }

double soemdsp_phone_tone_tone_r(int handle) { return 0.0; }

double soemdsp_phone_tone_f1(int handle) { return 0.0; }

double soemdsp_phone_tone_f2(int handle) { return 0.0; }

double soemdsp_phone_tone_analog_thru(int handle) { return 0.0; }

double soemdsp_phone_tone_digital_thru(int handle) { return 0.0; }

int soemdsp_blit_create() { return 0; }

void soemdsp_blit_destroy(int handle) {}

void soemdsp_blit_reset(int handle) {}

void soemdsp_blit_sample(
  int handle, double phase, double phaseIncrement, int waveform, double level
) {}

double soemdsp_blit_out(int handle) { return 0.0; }

double soemdsp_blit_saw(int handle) { return 0.0; }

double soemdsp_blit_ramp(int handle) { return 0.0; }

double soemdsp_blit_square(int handle) { return 0.0; }

double soemdsp_blit_tri(int handle) { return 0.0; }

double soemdsp_blit_sine(int handle) { return 0.0; }

int soemdsp_sine_wavetable_create() { return 0; }

void soemdsp_sine_wavetable_destroy(int handle) {}

void soemdsp_sine_wavetable_reset(int handle) {}

void soemdsp_sine_wavetable_set_method(int handle, int method) {}

void soemdsp_sine_wavetable_sample(
  int handle, double phaseOffsetRadians, double frequency, double amplitude, double sampleRate
) {}

double soemdsp_sine_wavetable_sin(int handle) { return 0.0; }

double soemdsp_sine_wavetable_cos(int handle) { return 0.0; }

double soemdsp_sine_wavetable_phase(int handle) { return 0.0; }

int soemdsp_antisaw_create() { return 0; }

void soemdsp_antisaw_destroy(int handle) {}

double soemdsp_antisaw_sample(
  int handle, double fundamental, double reflections, double tilt, double level, double sampleRate
) { return 0.0; }

int soemdsp_archimedes_create() { return 0; }

void soemdsp_archimedes_destroy(int handle) {}

void soemdsp_archimedes_reset(int handle) {}

void soemdsp_archimedes_reset_counters(int handle) {}

void soemdsp_archimedes_set_profile(int handle, int dtShift) {}

void soemdsp_archimedes_set_frequency(int handle, int freqHz) {}

double soemdsp_archimedes_step(int handle, int ditherBits) { return 0.0; }

double soemdsp_archimedes_sine(int handle) { return 0.0; }

double soemdsp_archimedes_cosine(int handle) { return 0.0; }

double soemdsp_archimedes_extract_pi(int handle) { return 0.0; }

double soemdsp_archimedes_noise_below(int handle) { return 0.0; }

double soemdsp_archimedes_noise_above(int handle) { return 0.0; }

double soemdsp_additive_osc_sample(
  double phase,
  double frequency,
  double harmonics,
  double waveform,
  double morph,
  double harmonicPhaseAdd,
  double harmonicPhaseMultiply,
  double level,
  double dampingFilterFrequency,
  double sampleRate
) { return 0.0; }

int soemdsp_surge_oscillator_create() { return 0; }

void soemdsp_surge_oscillator_destroy(int handle) {}

void soemdsp_surge_oscillator_reset(int handle) {}

void soemdsp_surge_oscillator_sample(
  int handle,
  double frequencyHz,
  double sampleRate,
  double syncIn,
  int hasExternalSync,
  double syncFrequencyHz,
  int waveform,
  double level
) {}

double soemdsp_surge_oscillator_out(int handle) { return 0.0; }

double soemdsp_surge_oscillator_saw(int handle) { return 0.0; }

double soemdsp_surge_oscillator_square(int handle) { return 0.0; }

double soemdsp_surge_oscillator_tri(int handle) { return 0.0; }

double soemdsp_surge_oscillator_sine(int handle) { return 0.0; }

double soemdsp_surge_oscillator_synced(int handle) { return 0.0; }

double soemdsp_surge_oscillator_internal_sync(int handle) { return 0.0; }

int soemdsp_softwave_create() { return 0; }

void soemdsp_softwave_destroy(int handle) {}

void soemdsp_softwave_reset(int handle) {}

double soemdsp_softwave_sample(
  int handle,
  double frequencyHz,
  double sampleRate,
  double waveform,
  double morph,
  double phaseOffset,
  double level,
  double antialias
) { return 0.0; }

int soemdsp_dsf_oscillator_create() { return 0; }

void soemdsp_dsf_oscillator_destroy(int handle) {}

void soemdsp_dsf_oscillator_reset(int handle) {}

void soemdsp_dsf_oscillator_sample(
  int handle,
  double frequencyHz,
  double sampleRate,
  int waveform,
  double morph,
  double pulseWidth,
  double blend,
  double phase,
  double level
) {}

double soemdsp_dsf_oscillator_out(int handle) { return 0.0; }

int soemdsp_hypersaw2_create() { return 0; }

void soemdsp_hypersaw2_destroy(int handle) {}

void soemdsp_hypersaw2_reset(int handle) {}

void soemdsp_hypersaw2_sample(
  int handle,
  double frequencyHz,
  double sampleRate,
  double phaseGlobal,
  double numVoicesExact,
  double distributePhase,
  double randomizePhase,
  double vibratoDistance,
  double vibratoSpeedHz,
  double vibratoPhaseVary,
  double jitterDistance,
  double jitterSpeed,
  double jitterDistanceSource,
  double jitterSteps,
  double jitterFilterHz,
  double vibratoDistanceSource,
  double centerSide,
  double waveform,
  double morph,
  double level,
  double seedParam
) {}

double soemdsp_hypersaw2_left(int handle) { return 0.0; }

double soemdsp_hypersaw2_right(int handle) { return 0.0; }

double soemdsp_hypersaw2_voice_phase(int handle, int voiceIndex) { return 0.0; }

int soemdsp_hypersaw2_voice_count(int handle) { return 0; }

double soemdsp_hypersaw2_voice_last_frac(int handle) { return 0.0; }

int soemdsp_hypersaw2_max_voices() { return 0; }

int soemdsp_hypersaw2_version() { return 0; }

int soemdsp_wow_and_flutter_create() { return 0; }

void soemdsp_wow_and_flutter_destroy(int handle) {}

void soemdsp_wow_and_flutter_reset(int handle, double phaseOffset) {}

double soemdsp_wow_and_flutter_sample(
  int handle,
  double wowSpeedHz,
  double sampleRate,
  double wowPhaseOffset,
  double wowAmp,
  double flutterFrequency,
  double flutterJitter,
  double flutterAmp,
  double seedParam,
  double level
) { return 0.0; }

double soemdsp_wow_and_flutter_out(int handle) { return 0.0; }

int soemdsp_sinc_create() { return 0; }

void soemdsp_sinc_destroy(int handle) {}

double soemdsp_sinc_sample(
  int handle,
  double freq,
  double phaseShift,
  double lobes,
  double bandLimit,
  double sampleRate
) { return 0.0; }

int soemdsp_bradley_2a_create() { return 0; }

void soemdsp_bradley_2a_destroy(int handle) {}

double soemdsp_bradley_2a_sample(
  int handle,
  double carrierFreq,
  double freqOffset,
  double jitterDepth,
  double jitterRate,
  double ampDepth,
  double ampRate,
  double interfLevel,
  double interfFreq,
  double harm2,
  double harm3,
  double hitRate,
  double hitDuration,
  double hitGain,
  double hitPhase,
  double impulseLevel,
  double level,
  double sampleRate
) { return 0.0; }

double soemdsp_ellipsoid_sine_to_square_aa(
  double phaseCycles,
  double shape,
  double frequencyHz,
  double sampleRate,
  int antialias
) { return 0.0; }

int soemdsp_snowflake_create() { return 0; }

void soemdsp_snowflake_destroy(int handle) {}

void soemdsp_snowflake_sample(
  int handle,
  double frequencyHz,
  double pattern,
  double iterations,
  double angleDeg,
  double sizeArg,
  double directionArg,
  double spin,
  double level,
  double reset,
  double phaseArg,
  double sampleRate
) {}

double soemdsp_snowflake_x(int handle) { return 0.0; }

double soemdsp_snowflake_y(int handle) { return 0.0; }

int soemdsp_butterworth_create() { return 0; }

void soemdsp_butterworth_destroy(int handle) {}

double soemdsp_butterworth_sample(
  int handle, double input, int mode, double frequencyHz, int order,
  double bandwidthOct, double rippleDb, double sampleRate
) { return 0.0; }

int soemdsp_linkwitz_riley_create() { return 0; }

void soemdsp_linkwitz_riley_destroy(int handle) {}

double soemdsp_linkwitz_riley_sample(
  int handle, double input, int mode, double frequencyHz, int order,
  double bandwidthOct, double rippleDb, double sampleRate
) { return 0.0; }

int soemdsp_bessel_create() { return 0; }

void soemdsp_bessel_destroy(int handle) {}

double soemdsp_bessel_sample(
  int handle, double input, int mode, double frequencyHz, int order,
  double bandwidthOct, double rippleDb, double sampleRate
) { return 0.0; }

int soemdsp_chebyshev_create() { return 0; }

void soemdsp_chebyshev_destroy(int handle) {}

double soemdsp_chebyshev_sample(
  int handle, double input, int mode, double frequencyHz, int order,
  double bandwidthOct, double rippleDb, double sampleRate
) { return 0.0; }

int soemdsp_elliptic_create() { return 0; }

void soemdsp_elliptic_destroy(int handle) {}

double soemdsp_elliptic_sample(
  int handle, double input, int mode, double frequencyHz, int order,
  double bandwidthOct, double rippleDb, double sampleRate
) { return 0.0; }

int soemdsp_eq_filter_create() { return 0; }

void soemdsp_eq_filter_destroy(int handle) {}

double soemdsp_eq_filter_sample(
  int handle, double input, double mode, double frequency, double q,
  double gainDb, double sampleRate
) { return 0.0; }

int soemdsp_graphic_eq_create() { return 0; }

void soemdsp_graphic_eq_destroy(int handle) {}

void soemdsp_graphic_eq_set_band(int handle, int index, double unitGain) {}

double soemdsp_graphic_eq_sample(
  int handle, double in, double rangeChoice, double mix, double amplitude,
  double sampleRate
) { return 0.0; }

int soemdsp_active_filter_create() { return 0; }

void soemdsp_active_filter_destroy(int handle) {}

double soemdsp_active_filter_sample(
  int handle, double input, double lowFrequencyHz, double highFrequencyHz,
  int hpSlope, int lpSlope, double resonance, int feedbackCircuit,
  int gainCompensation, double sampleRate
) { return 0.0; }

int soemdsp_tb303_filter_create() { return 0; }

void soemdsp_tb303_filter_destroy(int handle) {}

double soemdsp_tb303_filter_sample(
  int handle, double input, double cutoff, double resonance, int mode,
  double drive, double sampleRate
) { return 0.0; }

int soemdsp_flower_child_filter_create() { return 0; }

void soemdsp_flower_child_filter_destroy(int handle) {}

double soemdsp_flower_child_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, int mode, double sampleRate
) { return 0.0; }

int soemdsp_yellowjacket_filter_create() { return 0; }

void soemdsp_yellowjacket_filter_destroy(int handle) {}

double soemdsp_yellowjacket_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, double sampleRate
) { return 0.0; }

int soemdsp_superlove_filter_create() { return 0; }

void soemdsp_superlove_filter_destroy(int handle) {}

int soemdsp_superlove_rev2_create() { return 0; }

void soemdsp_superlove_rev2_destroy(int handle) {}

double soemdsp_superlove_rev2_sample(
  int handle, double input, double frequency, double resonance,
  double morphAmount, double noiseAmount, double phaseBias, int mode, double sampleRate
) { return 0.0; }

double soemdsp_superlove_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, int mode, double sampleRate
) { return 0.0; }

int soemdsp_human_filter_create() { return 0; }

void soemdsp_human_filter_destroy(int handle) {}

double soemdsp_human_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, int mode, double sampleRate
) { return 0.0; }

int soemdsp_resonator_filter_create() { return 0; }

void soemdsp_resonator_filter_destroy(int handle) {}

double soemdsp_resonator_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, int mode, double sampleRate
) { return 0.0; }

int soemdsp_comb_resonator_create() { return 0; }

void soemdsp_comb_resonator_destroy(int handle) {}

double soemdsp_comb_resonator_sample(
  int handle, double input, double frequencyHz, double decaySec, int hold,
  double damping, int topology, int invert, double depth, double amplitude,
  double sampleRate
) { return 0.0; }

int soemdsp_mode_resonator_create() { return 0; }

void soemdsp_mode_resonator_destroy(int handle) {}

double soemdsp_mode_resonator_sample(
  int handle, double input, double frequencyHz, double decaySec, int hold,
  double amplitude, double sampleRate
) { return 0.0; }

int soemdsp_chaotic_phase_locking_filter_create() { return 0; }

void soemdsp_chaotic_phase_locking_filter_destroy(int handle) {}

double soemdsp_chaotic_phase_locking_filter_sample(
  int handle, double input, double frequency, double resonance,
  double chaosAmount, double phaseBias, double sampleRate
) { return 0.0; }

int soemdsp_inertial_filter_create() { return 0; }

void soemdsp_inertial_filter_destroy(int handle) {}

double soemdsp_inertial_filter_sample(
  int handle, double input, double attackHz, double releaseHz,
  double smoothAttack, double sampleRate
) { return 0.0; }

int soemdsp_exp_adsr_create() { return 0; }

void soemdsp_exp_adsr_destroy(int handle) {}

double soemdsp_exp_adsr_sample(
  int handle, double gate, double delay, double attack, double attackShape,
  double decay, double sustain, double release, double releaseShape,
  double loop, double level, double updateOnTrigger, double sampleRate
) { return 0.0; }

int soemdsp_exp_adsr_is_idle(int handle) { return 0; }

int soemdsp_attack_decay_create() { return 0; }

void soemdsp_attack_decay_destroy(int handle) {}

double soemdsp_attack_decay_sample(
  int handle, double gate, double attack, double decay, double curve,
  double amplitude, double inputMode, double cycle, double sampleRate
) { return 0.0; }

int soemdsp_linear_attack_release_create() { return 0; }

void soemdsp_linear_attack_release_destroy(int handle) {}

double soemdsp_linear_attack_release_sample(
  int handle, double gate, double attack, double release,
  double amplitude, double inputMode, double sampleRate
) { return 0.0; }

int soemdsp_linear_attack_release_version() { return 0; }

int soemdsp_curve_attack_release_create() { return 0; }

void soemdsp_curve_attack_release_destroy(int handle) {}

double soemdsp_curve_attack_release_sample(
  int handle, double gate, double attack, double attackShape,
  double release, double releaseShape, double amplitude,
  double inputMode, double updateOnTrigger, double sampleRate
) { return 0.0; }

int soemdsp_curve_attack_release_version() { return 0; }

int soemdsp_thump_envelope_create() { return 0; }

void soemdsp_thump_envelope_destroy(int handle) {}

double soemdsp_thump_envelope_sample(
  int handle, double gate, double attack, double release,
  double decaySnap, double decayBody, double fallCurve, double loop,
  double amplitude, double updateOnTrigger, double sampleRate
) { return 0.0; }

int soemdsp_thump_envelope_version() { return 0; }

int soemdsp_basic_shape_create() { return 0; }

void soemdsp_basic_shape_destroy(int handle) {}

double soemdsp_basic_shape_sample(
  int handle, double frequencyHz, double sampleRate, double waveform,
  double motion, double phaseOffset, double morph, double amplitude,
  double polarity, double increment, double reset
) { return 0.0; }

double soemdsp_basic_shape_out(int handle) { return 0.0; }

double soemdsp_basic_shape_sine(int handle) { return 0.0; }

double soemdsp_basic_shape_tri(int handle) { return 0.0; }

double soemdsp_basic_shape_saw(int handle) { return 0.0; }

double soemdsp_basic_shape_ramp(int handle) { return 0.0; }

double soemdsp_basic_shape_square(int handle) { return 0.0; }

double soemdsp_basic_shape_trisaw(int handle) { return 0.0; }

double soemdsp_basic_shape_center_square(int handle) { return 0.0; }

double soemdsp_basic_shape_phase(int handle) { return 0.0; }

int soemdsp_linear_envelope_create() { return 0; }

void soemdsp_linear_envelope_destroy(int handle) {}

double soemdsp_linear_envelope_sample(
  int handle, double gate, double delay, double attack, double decay,
  double sustain, double release, double loop, double level, double sampleRate
) { return 0.0; }

int soemdsp_linear_envelope_is_idle(int handle) { return 0; }

int soemdsp_wavetable_adsr_create() { return 0; }

void soemdsp_wavetable_adsr_destroy(int handle) {}

double soemdsp_wavetable_adsr_sample(
  int handle, double gate, double reset, double shapeParam,
  double attack, double decay, double sustain, double release,
  double level, double sampleRate
) { return 0.0; }

int soemdsp_wavetable_adsr_is_idle(int handle) { return 0; }

int soemdsp_pluck_envelope_create() { return 0; }

void soemdsp_pluck_envelope_destroy(int handle) {}

double soemdsp_pluck_envelope_sample(
  int handle, double trigger, double releaseGate,
  double velocitySensitivity, double attack,
  double decaySlopeTop, double decaySlopeMid, double decaySlopeBottom,
  double sustain, double releaseAmt, double autoReleaseTime,
  double envelopeCurve, double envelopeDamping,
  double velocity, double level, double sampleRate
) { return 0.0; }

int soemdsp_expo_pluck_envelope_create() { return 0; }

void soemdsp_expo_pluck_envelope_destroy(int handle) {}

void soemdsp_expo_pluck_envelope_reset(int handle) {}

double soemdsp_expo_pluck_envelope_sample(
  int handle,
  double trigger,
  double gate,
  double attack,
  double decay,
  double frequency,
  double damping,
  double recalculateOnTrigger,
  double level,
  double sampleRate
) { return 0.0; }

int soemdsp_expo_pluck_envelope_2_create() { return 0; }

void soemdsp_expo_pluck_envelope_2_destroy(int handle) {}

void soemdsp_expo_pluck_envelope_2_reset(int handle) {}

double soemdsp_expo_pluck_envelope_2_sample(
  int handle,
  double trigger,
  double releaseGate,
  double velocitySensitivity,
  double attack,
  double decaySlopeTop,
  double decaySlopeMid,
  double decaySlopeBottom,
  double sustain,
  double releaseAmt,
  double autoReleaseTimeMs,
  double envelopeCurve,
  double envelopeDamping,
  double velocity,
  double level,
  double sampleRate
) { return 0.0; }

int soemdsp_flower_child_envelope_follower_create() { return 0; }

void soemdsp_flower_child_envelope_follower_destroy(int handle) {}

double soemdsp_flower_child_envelope_follower_sample(
  int handle, double input, double attack, double hold, double decay,
  double sampleRate
) { return 0.0; }

int soemdsp_vactrol_envelope_create() { return 0; }

void soemdsp_vactrol_envelope_destroy(int handle) {}

double soemdsp_vactrol_envelope_sample(
  int handle, double light, double attack, double release, double curve,
  double sensitivity, double sampleRate
) { return 0.0; }

int soemdsp_delay_effect_create() { return 0; }

void soemdsp_delay_effect_destroy(int handle) {}

void soemdsp_delay_effect_sample(
  int handle, double input, double time, double feedback, double mix,
  double level, double modAmount, double modRate, double modVariation,
  double mode, unsigned int seed, double sampleRate
) {}

double soemdsp_delay_effect_out(int handle) { return 0.0; }

double soemdsp_delay_effect_wet(int handle) { return 0.0; }

int soemdsp_soem_reverb_create(double sampleRate) { return 0; }

void soemdsp_soem_reverb_destroy(int handle) {}

void soemdsp_soem_reverb_reset(int handle) {}

void soemdsp_soem_reverb_set_params(
  int handle,
  double mix, double volume, double echoTime, double recycle, double numDelays,
  double diffusionSize, double diffusionAmount, double seed, double lfoAmp,
  double lfoFrequency, double lfoVariation, double lfoStyle, double echoMode,
  double pingPong, double doModulateEcho, double saturate, double lpfFrequency,
  double hpfFrequency, double bandFrequency, double bandDecibels, double bandQ,
  double lpfStages, double bandStages, double duckLimit, double duckRelease
) {}

void soemdsp_soem_reverb_process(int handle, double inL, double inR) {}

double soemdsp_soem_reverb_left(int handle) { return 0.0; }

double soemdsp_soem_reverb_right(int handle) { return 0.0; }

double soemdsp_soem_reverb_wet_left(int handle) { return 0.0; }

double soemdsp_soem_reverb_wet_right(int handle) { return 0.0; }

double soemdsp_soem_reverb_dry_left(int handle) { return 0.0; }

double soemdsp_soem_reverb_dry_right(int handle) { return 0.0; }

int soemdsp_soem_reverb_is_idle(int handle) { return 0; }

int soemdsp_pll_create(double sampleRate) { return 0; }

void soemdsp_pll_destroy(int handle) {}

void soemdsp_pll_reset(int handle, double sampleRate) {}

void soemdsp_pll_set_params(
  int handle, double sampleRate, int range, double offset, int type, double frequ
) {}

void soemdsp_pll_process(
  int handle, double signalIn, double cvIn, double cvConnected
) {}

double soemdsp_pll_vco_out(int handle) { return 0.0; }

double soemdsp_pll_pc_out(int handle) { return 0.0; }

double soemdsp_pll_lpf_out(int handle) { return 0.0; }

double soemdsp_pll_locked(int handle) { return 0.0; }

int soemdsp_lorenz_attractor_create() { return 0; }

void soemdsp_lorenz_attractor_destroy(int handle) {}

void soemdsp_lorenz_attractor_sample(
  int handle, double reset, double speed, double sigma, double rho, double beta,
  double rotate, double scale, double zDepth, double sampleRate
) {}

double soemdsp_lorenz_attractor_x(int handle) { return 0.0; }

double soemdsp_lorenz_attractor_y(int handle) { return 0.0; }

double soemdsp_lorenz_attractor_z(int handle) { return 0.0; }

int soemdsp_logistic_map_create() { return 0; }

void soemdsp_logistic_map_destroy(int handle) {}

double soemdsp_logistic_map_sample(
  int handle, double reset, double rate, double r, double seed, double level,
  double sampleRate
) { return 0.0; }

int soemdsp_henon_map_create() { return 0; }

void soemdsp_henon_map_destroy(int handle) {}

void soemdsp_henon_map_sample(
  int handle, double reset, double rate, double a, double b, double seedX,
  double seedY, double sampleRate
) {}

double soemdsp_henon_map_x(int handle) { return 0.0; }

double soemdsp_henon_map_y(int handle) { return 0.0; }

int soemdsp_chua_attractor_create() { return 0; }

void soemdsp_chua_attractor_destroy(int handle) {}

void soemdsp_chua_attractor_sample(
  int handle, double reset, double speed, double alpha, double beta,
  double m0, double m1, double sampleRate
) {}

double soemdsp_chua_attractor_x(int handle) { return 0.0; }

double soemdsp_chua_attractor_y(int handle) { return 0.0; }

double soemdsp_chua_attractor_z(int handle) { return 0.0; }

int soemdsp_chaosfly_create() { return 0; }

void soemdsp_chaosfly_destroy(int handle) {}

void soemdsp_chaosfly_reset(int handle) {}

void soemdsp_chaosfly_sample(
  int handle,
  double outputMode,
  double frequencyHz,
  double masterFm,
  double lowpassOct,
  double highpassOct,
  double tapsIndex,
  double hpPosition,
  double osc1Detune,
  double osc2Detune,
  double fm1Offset,
  double fm2Offset,
  double panBi,
  double amplitude,
  double pitchOctaves,
  double phaseOffsetTurns,
  double sampleRate
) {}

double soemdsp_chaosfly_left(int handle) { return 0.0; }

double soemdsp_chaosfly_right(int handle) { return 0.0; }

double soemdsp_chaosfly_out(int handle) { return 0.0; }

double soemdsp_chaosfly_z(int handle) { return 0.0; }

double soemdsp_chaosfly_x(int handle) { return 0.0; }

double soemdsp_chaosfly_y(int handle) { return 0.0; }

int soemdsp_ray_bouncer_create() { return 0; }

void soemdsp_ray_bouncer_destroy(int handle) {}

void soemdsp_ray_bouncer_sample(
  int handle, double reset, double frequency, double launchAngleDeg,
  double startX, double startY, double size, double aspect, double rotateDeg,
  double centerX, double centerY, double maxDistance, double bend,
  double xToY, double yToX, double sampleRate
) {}

double soemdsp_ray_bouncer_x(int handle) { return 0.0; }

double soemdsp_ray_bouncer_y(int handle) { return 0.0; }

int soemdsp_chord_memory_create() { return 0; }

void soemdsp_chord_memory_destroy(int handle) {}

double soemdsp_chord_memory_sample(
  int handle, double latch, double clear, double advance, double pitch
) { return 0.0; }

double soemdsp_chord_memory_note2(int handle) { return 0.0; }

double soemdsp_chord_memory_note3(int handle) { return 0.0; }

double soemdsp_chord_memory_note4(int handle) { return 0.0; }

double soemdsp_chord_memory_arp(int handle) { return 0.0; }

double soemdsp_chord_memory_gate(int handle) { return 0.0; }

int soemdsp_chord_sequencer_create() { return 0; }

void soemdsp_chord_sequencer_destroy(int handle) {}

void soemdsp_chord_sequencer_sample(
  int handle, double clock, double reset, double progression
) {}

int soemdsp_chord_sequencer_scale(int handle, double progression) { return 0; }

double soemdsp_chord_sequencer_root(int handle, double progression) { return 0.0; }

int soemdsp_chord_sequencer_step(int handle) { return 0; }

int soemdsp_turing_machine_create(unsigned int entropySeed) { return 0; }

void soemdsp_turing_machine_destroy(int handle) {}

double soemdsp_turing_machine_sample(
  int handle, double clock, double reset, double length, double probability, double level
) { return 0.0; }

double soemdsp_turing_machine_scale(int handle) { return 0.0; }

double soemdsp_turing_machine_gate(int handle) { return 0.0; }

int soemdsp_chord_pad_create() { return 0; }

void soemdsp_chord_pad_destroy(int handle) {}

double soemdsp_chord_pad_sample(
  int handle, double select, double hasSelect, double key, double mode,
  double degree, double level
) { return 0.0; }

double soemdsp_chord_pad_root(int handle) { return 0.0; }

double soemdsp_chord_pad_gate(int handle) { return 0.0; }

int soemdsp_note_glide_create() { return 0; }

void soemdsp_note_glide_destroy(int handle) {}

double soemdsp_note_glide_sample(
  int handle, double pitch, double timeSeconds, double sampleRate
) { return 0.0; }

int soemdsp_note_transpose_create() { return 0; }

void soemdsp_note_transpose_destroy(int handle) {}

double soemdsp_note_transpose_sample(
  int handle, double pitch, double semitones, double octaves
) { return 0.0; }

int soemdsp_degree_turing_create(unsigned int entropySeed) { return 0; }

void soemdsp_degree_turing_destroy(int handle) {}

double soemdsp_degree_turing_sample(
  int handle, double clock, double reset, double length, double probability,
  double octaves, double level, double scaleIn, double hasScale, double root,
  double scaleChoice
) { return 0.0; }

double soemdsp_degree_turing_gate(int handle) { return 0.0; }

double soemdsp_degree_turing_trigger(int handle) { return 0.0; }

double soemdsp_degree_turing_degree(int handle) { return 0.0; }

double soemdsp_degree_turing_cv(int handle) { return 0.0; }

int soemdsp_degree_phrase_create(unsigned int entropySeed) { return 0; }

void soemdsp_degree_phrase_destroy(int handle) {}

double soemdsp_degree_phrase_sample(
  int handle, double clock, double reset, double stepsIn, double mutateIn,
  double octaves, double level, double scaleIn, double hasScale, double root,
  double scaleChoice,
  double step1, double step2, double step3, double step4,
  double step5, double step6, double step7, double step8,
  double rest1, double rest2, double rest3, double rest4,
  double rest5, double rest6, double rest7, double rest8
) { return 0.0; }

double soemdsp_degree_phrase_gate(int handle) { return 0.0; }

double soemdsp_degree_phrase_trigger(int handle) { return 0.0; }

double soemdsp_degree_phrase_phase(int handle) { return 0.0; }

int soemdsp_fbm_create() { return 0; }

void soemdsp_fbm_destroy(int handle) {}

void soemdsp_fbm_reset(int handle) {}

void soemdsp_fbm_sample(
  int handle, int seedInt, int octaves, double persistence, double scale,
  double frequency, double level, double sampleRate
) {}

double soemdsp_fbm_x(int handle) { return 0.0; }

double soemdsp_fbm_y(int handle) { return 0.0; }

double soemdsp_fbm_z(int handle) { return 0.0; }

double soemdsp_fbm_x_raw(int handle) { return 0.0; }

double soemdsp_fbm_y_raw(int handle) { return 0.0; }

double soemdsp_fbm_z_raw(int handle) { return 0.0; }

int soemdsp_pi_spigot_noise_create() { return 0; }

void soemdsp_pi_spigot_noise_destroy(int handle) {}

void soemdsp_pi_spigot_noise_reset_seed(int handle, double start, double stride) {}

void soemdsp_pi_spigot_noise_sample(int handle, double color, double smoothing, double level) {}

double soemdsp_pi_spigot_noise_left(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_right(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_hex(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_n(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_t(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_b3(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_b2(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_b1(int handle) { return 0.0; }

double soemdsp_pi_spigot_noise_b0(int handle) { return 0.0; }

int soemdsp_random_walk_create() { return 0; }

void soemdsp_random_walk_destroy(int handle) {}

void soemdsp_random_walk_reset_seed(int handle, double seed) {}

double soemdsp_random_walk_sample(
  int handle, double method, double frequency, double jitter, double level, double sampleRate
) { return 0.0; }

int soemdsp_cheap_walk_create() { return 0; }

void soemdsp_cheap_walk_destroy(int handle) {}

double soemdsp_cheap_walk_sample(
  int handle, double rateHz, double amplitude, double seedParam, double sampleRate
) { return 0.0; }

void soemdsp_cheap_walk_sample_stereo(
  int handle, double rateHz, double amplitude, double seedParam, double sampleRate,
  double* outLeft, double* outRight
) {}

int soemdsp_pulse_explosion_create() { return 0; }

void soemdsp_pulse_explosion_destroy(int handle) {}

double soemdsp_pulse_explosion_sample(
  int handle, double trigger, double startTime, double centerTime, double endTime,
  double timeSpread, int numberOfPulses, double lowAmplitude, double highAmplitude,
  double seed, double sampleRate
) { return 0.0; }

double soemdsp_pulse_explosion_curve(int handle) { return 0.0; }

int soemdsp_jerobeam_spiral_create() { return 0; }

void soemdsp_jerobeam_spiral_destroy(int handle) {}

void soemdsp_jerobeam_spiral_sample(
  int handle, double frequency, double density, double size, double sharp,
  double sharpCurve, double sharpCurveMult, double morph, double morphSpeed,
  double position, double positionSpeed, double rotX, double rotXSpeed,
  double rotY, double rotYSpeed, double zAmount, double zDepth, double sampleRate
) {}

double soemdsp_jerobeam_spiral_x(int handle) { return 0.0; }

double soemdsp_jerobeam_spiral_y(int handle) { return 0.0; }

double soemdsp_jerobeam_spiral_z(int handle) { return 0.0; }

int soemdsp_fractal_spiral_create() { return 0; }

void soemdsp_fractal_spiral_destroy(int handle) {}

void soemdsp_fractal_spiral_sample(
  int handle, double frequency, double spin, double size, double growth,
  double gain, double lacunarity, double octaves, double twist, double sampleRate
) {}

double soemdsp_fractal_spiral_x(int handle) { return 0.0; }

double soemdsp_fractal_spiral_y(int handle) { return 0.0; }

double soemdsp_fractal_spiral_z(int handle) { return 0.0; }

int soemdsp_log_spiral_create() { return 0; }

void soemdsp_log_spiral_destroy(int handle) {}

void soemdsp_log_spiral_sample(
  int handle, double frequency, double spin, double size, double growth,
  double turns, double sampleRate
) {}

double soemdsp_log_spiral_x(int handle) { return 0.0; }

double soemdsp_log_spiral_y(int handle) { return 0.0; }

double soemdsp_log_spiral_z(int handle) { return 0.0; }

int soemdsp_jbblubb_create() { return 0; }

void soemdsp_jbblubb_destroy(int handle) {}

void soemdsp_jbblubb_reset(int handle) {}

void soemdsp_jbblubb_sample(
  int handle, double frequency, double shape, double rotX, double rotY,
  double zDepth, double sampleRate
) {}

double soemdsp_jbblubb_x(int handle) { return 0.0; }

double soemdsp_jbblubb_y(int handle) { return 0.0; }

int soemdsp_jbboing_create() { return 0; }

void soemdsp_jbboing_destroy(int handle) {}

void soemdsp_jbboing_reset(int handle) {}

void soemdsp_jbboing_sample(
  int handle, double frequency, double density, double sharpness, double rotX,
  double rotY, double zDepth, double zAmount, double ends, double boing,
  double boingStrength, double dir, double shape, double volume,
  double volumePreJump, double sampleRate
) {}

double soemdsp_jbboing_x(int handle) { return 0.0; }

double soemdsp_jbboing_y(int handle) { return 0.0; }

int soemdsp_jbkepler_create() { return 0; }

void soemdsp_jbkepler_destroy(int handle) {}

void soemdsp_jbkepler_reset(int handle) {}

void soemdsp_jbkepler_sample(
  int handle, double frequency, double start, double length, double circles,
  double zoom, double rotation, double tri, double sampleRate
) {}

double soemdsp_jbkepler_x(int handle) { return 0.0; }

double soemdsp_jbkepler_y(int handle) { return 0.0; }

int soemdsp_jbmushroom_create() { return 0; }

void soemdsp_jbmushroom_destroy(int handle) {}

void soemdsp_jbmushroom_reset(int handle) {}

void soemdsp_jbmushroom_sample(
  int handle, double frequency, double phaseOffset, double numMushrooms,
  double grow, double density, double capRotation, double stemRotationSpeed,
  double head, double spread, double wobble, double clusterRotation,
  double clusterRotationSpeed, double sharp, double width, double stem,
  double apart, double capStemTransition, double sampleRate
) {}

double soemdsp_jbmushroom_x(int handle) { return 0.0; }

double soemdsp_jbmushroom_y(int handle) { return 0.0; }

int soemdsp_jbnyquist_create() { return 0; }

void soemdsp_jbnyquist_destroy(int handle) {}

void soemdsp_jbnyquist_reset(int handle) {}

void soemdsp_jbnyquist_sample(
  int handle, double frequencyA, double midiNoteRaw, double rate, double sampleDots,
  double phaseOffset, double frequencyB, double subPhase, double subPhaseRotationSpeed,
  double tone, double toneSmoothTime, double artifact, double enableToneModPitch,
  double enableToneModFreq, double enableToneModNote, double sampleRate
) {}

double soemdsp_jbnyquist_x(int handle) { return 0.0; }

double soemdsp_jbnyquist_y(int handle) { return 0.0; }

int soemdsp_jbradar_create() { return 0; }

void soemdsp_jbradar_destroy(int handle) {}

void soemdsp_jbradar_reset(int handle) {}

void soemdsp_jbradar_sample(
  int handle, double frequency, double phaseOffset, double density, double sharp,
  double fade, double rotation, double direction, double shade, double lap,
  double ringcut, double pow1Up, double pow1Down, double pow2Bend, double phaseInv,
  double tunnelInv, double spiralReturn, double length, double ratio, double frontring,
  double zoom, double zDepth, double inner, double x, double y, double sampleRate
) {}

double soemdsp_jbradar_x(int handle) { return 0.0; }

double soemdsp_jbradar_y(int handle) { return 0.0; }

int soemdsp_jbtorus_create() { return 0; }

void soemdsp_jbtorus_destroy(int handle) {}

void soemdsp_jbtorus_reset(int handle) {}

void soemdsp_jbtorus_sample(
  int handle, double frequency, double density, double quantizeDensity,
  double subdensity, double quantizeSubDensity, double sharp, double size,
  double length, double balance, double wander, double darkAngle, double darkIntensity,
  double rotX, double rotY, double rotZ, double zAngleX, double zAngleY,
  double zDepth, double sampleRate
) {}

double soemdsp_jbtorus_x(int handle) { return 0.0; }

double soemdsp_jbtorus_y(int handle) { return 0.0; }

int soemdsp_jbwirdo_create() { return 0; }

void soemdsp_jbwirdo_destroy(int handle) {}

void soemdsp_jbwirdo_reset(int handle) {}

void soemdsp_jbwirdo_sample(
  int handle, double frequency, double sharp, double cross, double density,
  double length, double rotate, double splashDepth, double splashDensity,
  double cut, double scrap, double ringCut, double splashSpeed, double syncCut,
  double sampleRate
) {}

double soemdsp_jbwirdo_x(int handle) { return 0.0; }

double soemdsp_jbwirdo_y(int handle) { return 0.0; }

int soemdsp_phosphillator_create() { return 0; }

void soemdsp_phosphillator_destroy(int handle) {}

int soemdsp_phosphillator_path_x_ptr(int handle) { return 0; }

int soemdsp_phosphillator_path_y_ptr(int handle) { return 0; }

int soemdsp_phosphillator_set_path(int handle, int count) { return 0; }

double soemdsp_phosphillator_sample(
  int handle, double cvInput, double frequency, double phaseOffset, double reset,
  double rate, double sharpness
) { return 0.0; }

double soemdsp_phosphillator_y(int handle) { return 0.0; }

int soemdsp_smooth_graph_create() { return 0; }

void soemdsp_smooth_graph_destroy(int handle) {}

double soemdsp_smooth_graph_sample(
  int handle, double x, double smoothingMode, double tension
) { return 0.0; }

int soemdsp_step_graph_create() { return 0; }

void soemdsp_step_graph_destroy(int handle) {}

double soemdsp_step_graph_sample(
  int handle, double x, double segmentShape, double curveOffset
) { return 0.0; }

int soemdsp_crossover_create(int bandCount) { return 0; }

void soemdsp_crossover_destroy(int handle) {}

void soemdsp_crossover_sample(
  int handle,
  double mono,
  double leftIn,
  double rightIn,
  double f0,
  double f1,
  double f2,
  double f3,
  double f4,
  int lrOrder,
  double sampleRate
) {}

double soemdsp_crossover_band_l(int handle, int bandIndex) { return 0.0; }

double soemdsp_crossover_band_r(int handle, int bandIndex) { return 0.0; }

int soemdsp_crossover_band_count(int handle) { return 0; }

int soemdsp_papoulis_filter_create() { return 0; }

void soemdsp_papoulis_filter_destroy(int handle) {}

void soemdsp_papoulis_filter_snap(int handle, double value) {}

double soemdsp_papoulis_filter_sample(
  int handle, double input, double cutoffHz, double sampleRate
) { return 0.0; }

int soemdsp_phase_disperse_create() { return 0; }

void soemdsp_phase_disperse_destroy(int handle) {}

double soemdsp_phase_disperse_sample(
  int handle, double input, double freqHz, double stages, double qOrPinch, double sampleRate
) { return 0.0; }

int soemdsp_quadrature_create() { return 0; }

void soemdsp_quadrature_destroy(int handle) {}

void soemdsp_quadrature_process_sample(
  int handle, double in, double mid, double side,
  double* outI, double* outQ, double* outMidI, double* outSideQ
) {}

void soemdsp_quadrature_process_mono(
  int handle, double in, int mode, double* out
) {}

int soemdsp_arp_create() { return 0; }

void soemdsp_arp_destroy(int handle) {}

double soemdsp_arp_sample(
  int handle, double heldKeys, double hasHeldKeys,
  double trigger, double hasTrigger, double reset,
  double rateHz, double mode, double steps, double seed,
  double octaveOffset, double sequenceOffset, double sampleRate
) { return 0.0; }

double soemdsp_arp_gate(int handle) { return 0.0; }

double soemdsp_arp_trigger(int handle) { return 0.0; }

double soemdsp_arp_step(int handle) { return 0.0; }

double soemdsp_arp_frequency(int handle) { return 0.0; }

int soemdsp_arp_play_midi(int handle) { return 0; }

void soemdsp_arp_set_chunks(int handle, double c0, double c1, double c2) {}

void soemdsp_arp_set_override_midi(int handle, int midi) {}

int soemdsp_binary_clock_create() { return 0; }

void soemdsp_binary_clock_destroy(int handle) {}

double soemdsp_binary_clock_sample(
  int handle, double clock, double hasClock, double reset,
  double rate, double bits, double sampleRate
) { return 0.0; }

double soemdsp_binary_clock_bit0(int handle) { return 0.0; }

double soemdsp_binary_clock_bit1(int handle) { return 0.0; }

double soemdsp_binary_clock_bit2(int handle) { return 0.0; }

double soemdsp_binary_clock_bit3(int handle) { return 0.0; }

double soemdsp_binary_clock_gate(int handle) { return 0.0; }

int soemdsp_speaker_protection_create() { return 0; }

void soemdsp_speaker_protection_destroy(int handle) {}

double soemdsp_speaker_protection_sample(int handle, double input) { return 0.0; }

int soemdsp_speaker_protector2_create() { return 0; }

void soemdsp_speaker_protector2_destroy(int handle) {}

void soemdsp_speaker_protector2_sample(
  int handle,
  double leftIn,
  double rightIn,
  double sampleRate,
  double dropSeconds,
  double holdSeconds,
  double riseSeconds,
  double* outLeft,
  double* outRight,
  double* outMono
) {}

double soemdsp_speaker_protector2_gain(int handle) { return 0.0; }

int soemdsp_voice_manager_voice_state(int handle, int slot) { return 0; }

int soemdsp_sample_player_create() { return 0; }
void soemdsp_sample_player_destroy(int handle) {}
double soemdsp_sample_player_sample(
  int handle,
  double gate, double modeParam, double speedParam,
  double start, double end, double engineSampleRate
) { return 0.0; }
double soemdsp_sample_player_left(int handle) { return 0.0; }
double soemdsp_sample_player_right(int handle) { return 0.0; }
double soemdsp_sample_player_phase(int handle) { return 0.0; }

int soemdsp_cookbook_filter_create() { return 0; }
void soemdsp_cookbook_filter_destroy(int handle) {}
double soemdsp_cookbook_filter_sample(
  int handle, double input, double mode, double frequencyHz, double q,
  double gainDb, double stages, double topology, double sampleRate
) { return 0.0; }

int soemdsp_phaser_create() { return 0; }
void soemdsp_phaser_destroy(int handle) {}
double soemdsp_phaser_sample(
  int handle, double input, double frequency, double q, double slopeChoice,
  double bands, double spreadOct, double stereoOct, double rateHz, double depthOct,
  double feedback, double mix, double amplitude, double kernelMode, double reset,
  double sampleRate
) { return 0.0; }

int soemdsp_flanger_create() { return 0; }
void soemdsp_flanger_destroy(int handle) {}
double soemdsp_flanger_sample(
  int handle, double input, double timeSeconds, double depthSeconds,
  double stereoSeconds, double rateHz, double feedback, double mix,
  double amplitude, double reset, double sampleRate
) { return 0.0; }

int soemdsp_wavetable_2d_create() { return 0; }
void soemdsp_wavetable_2d_destroy(int handle) {}
void soemdsp_wavetable_2d_reset(int handle) {}
double soemdsp_wavetable_2d_sample(
  int handle, double reset, double frequencyHz, double phaseOffset,
  double amplitude, double morph, double engineSampleRate
) { return 0.0; }
double soemdsp_wavetable_2d_phase(int handle) { return 0.0; }

double soemdsp_amp_db_sample(double input, double mode) { return 0.0; }

int soemdsp_vcvrack_superlove_filter_create() { return 0; }
void soemdsp_vcvrack_superlove_filter_destroy(int handle) {}
double soemdsp_vcvrack_superlove_filter_sample(
  int handle, double input, double frequency, double resonance,
  double noise01, double drive, int mode, double sampleRate
) { return 0.0; }

double soemdsp_attenumax_sample(double input, double amplitudePercent, double offset) {
  return 0.0;
}

int soemdsp_chorus_create() { return 0; }
void soemdsp_chorus_destroy(int handle) {}
void soemdsp_chorus_sample(
  int handle, double inL, double inR, double voicesN, double delayMs, double depthMs,
  double mix, double spread, double speedHz, double topMorph, double sideMorph,
  double phase, double randomFreq, double randomAmp, double seedParam, double feedback,
  double hpHz, double lpHz, double sampleRate, double* outL, double* outR
) {
  if (outL) *outL = 0.0;
  if (outR) *outR = 0.0;
}

void soemdsp_ellipsoid_sample_pair(
  double phase, double offset, double shape, double scale,
  double frequencyHz, double sampleRate, int antialias,
  double* outX, double* outY
) {
  if (outX) *outX = 0.0;
  if (outY) *outY = 0.0;
}

} // extern C