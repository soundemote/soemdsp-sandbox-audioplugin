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

#ifndef BACONPAUL_SoemDSP_CLAP_PLUGIN_CLAP_ENTRY_IMPL_H
#define BACONPAUL_SoemDSP_CLAP_PLUGIN_CLAP_ENTRY_IMPL_H

namespace soemdsp_ap
{
const void *get_factory(const char *factory_id);
bool clap_init(const char *p);
void clap_deinit();
} // namespace soemdsp_ap

#endif
