#include <optional>
#include <stdexcept>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/version.h>

#include "SelectionHook.hpp"
#include "dispatchers.hpp"
#include "globals.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
	PHANDLE = handle;

#ifndef HY3_NO_VERSION_CHECK
	if (GIT_COMMIT_HASH != std::string(__hyprland_api_get_hash())) {
		HyprlandAPI::addNotification(
		    PHANDLE,
		    "[hy3] hy3 was compiled for a different version of hyprland; refusing to load.",
		    CColor {1.0, 0.2, 0.2, 1.0},
		    10000
		);

		throw std::runtime_error("[hy3] target hyprland version mismatch");
	}
#endif

	selection_hook::init();

#define CONF(NAME, TYPE, VALUE)                                                                    \
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:hy3:" NAME, SConfigValue {.TYPE##Value = VALUE})

	// general
	CONF("no_gaps_when_only", int, 0);
	CONF("node_collapse_policy", int, 2);
	CONF("group_inset", int, 10);
	CONF("tab_first_window", int, 0);

	// tabs
	CONF("tabs:height", int, 15);
	CONF("tabs:padding", int, 5);
	CONF("tabs:from_top", int, 0);
	CONF("tabs:rounding", int, 3);
	CONF("tabs:render_text", int, 1);
	CONF("tabs:text_center", int, 0);
	CONF("tabs:text_font", str, "Sans");
	CONF("tabs:text_height", int, 8);
	CONF("tabs:text_padding", int, 3);
	CONF("tabs:col.active", int, 0xff32b4ff);
	CONF("tabs:col.urgent", int, 0xffff4f4f);
	CONF("tabs:col.inactive", int, 0x80808080);
	CONF("tabs:col.text.active", int, 0xff000000);
	CONF("tabs:col.text.urgent", int, 0xff000000);
	CONF("tabs:col.text.inactive", int, 0xff000000);

	// autotiling
	CONF("autotile:enable", int, 0);
	CONF("autotile:ephemeral_groups", int, 1);
	CONF("autotile:trigger_height", int, 0);
	CONF("autotile:trigger_width", int, 0);
	CONF("autotile:workspaces", str, "all");

#undef CONF

	g_Hy3Layout = std::make_unique<Hy3Layout>();
	HyprlandAPI::addLayout(PHANDLE, "hy3", g_Hy3Layout.get());

	registerDispatchers();

	HyprlandAPI::reloadConfig();

	return {"hy3", "i3 like layout for hyprland", "outfoxxed", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
