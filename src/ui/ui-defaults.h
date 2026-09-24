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

#ifndef BACONPAUL_SoemDSP_UI_UI_DEFAULTS_H
#define BACONPAUL_SoemDSP_UI_UI_DEFAULTS_H

#include "configuration.h"
#include <sst/plugininfra/userdefaults.h>

namespace soemdsp_ap::ui
{
enum Defaults
{
    useLightSkin,
    zoomLevel,
    useSoftwareRenderer, // only used on windows
    numDefaults
};

inline std::string defaultName(Defaults d)
{
    switch (d)
    {
    case useLightSkin:
        return "useLightSkin";
    case zoomLevel:
        return "zoomLevel";
    case useSoftwareRenderer:
        return "useSoftwareRenderer";
    case numDefaults:
    {
        SQLOG("Software Error - defaults found");
        return "";
    }
    }
    return "";
}

using defaultsProvder_t = sst::plugininfra::defaults::Provider<Defaults, Defaults::numDefaults>;
} // namespace soemdsp_ap::ui

#endif // UI_DEFAULTS_H
