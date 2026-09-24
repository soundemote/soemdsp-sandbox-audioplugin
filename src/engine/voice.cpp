/*
 * SoemDSP Starting Point
 *
 * Basically lets paul bootstrap his projects.
 *
 * Copyright 2024-2025, Paul Walker and Various authors, as described in the github
 * transaction log.
 *
 * This source repo is released under the MIT license, but has
 * GPL3 dependencies, as such the combined work will be
 * released under GPL3.
 *
 * The source code and license are at https://github.com/baconpaul/SoemDSP-startingpoint
 */

#include "voice.h"
#include "sst/cpputils/constructors.h"
#include "engine/patch.h"
#include "libMTSClient.h"

namespace soemdsp_ap
{

namespace scpu = sst::cpputils;

void Voice::cleanup() {}

void Voice::retriggerAllEnvelopesForKeyPress() {}

void Voice::retriggerAllEnvelopesForReGate() {}

void Voice::setupPortaTo(uint16_t newKey, float log2Time) {}

void Voice::restartPortaTo(float sourceKey, uint16_t newKey, float log2Time, float portaFrac) {}

} // namespace soemdsp_ap