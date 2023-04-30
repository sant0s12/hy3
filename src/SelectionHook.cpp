#include "globals.hpp"
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/Compositor.hpp>

namespace selection_hook {
	inline CFunctionHook* g_LastSelectionHook = nullptr;

	void hook_updateDecos(void* thisptr, CWindow* window) {
		bool explicitly_selected = g_Hy3Layout->shouldRenderSelected(window);
		Debug::log(LOG, "update decos for %p - selected: %d", window, explicitly_selected);

		auto* lastWindow = g_pCompositor->m_pLastWindow;
		if (explicitly_selected) {
			g_pCompositor->m_pLastWindow = window;
		}

		((void (*)(void*, CWindow*)) g_LastSelectionHook->m_pOriginal)(thisptr, window);

		if (explicitly_selected) {
			g_pCompositor->m_pLastWindow = lastWindow;
		}
	}

	void init() {
		static const auto decoUpdateCandidates = HyprlandAPI::findFunctionsByName(PHANDLE, "updateWindowAnimatedDecorationValues");

		if (decoUpdateCandidates.size() != 1) {
			g_LastSelectionHook = nullptr;
			Debug::log(ERR, "Expected one matching function to hook for \"updateWindowAnimatedDecorationValues\", found %d", decoUpdateCandidates.size());
			HyprlandAPI::addNotificationV2(PHANDLE, {
				{"text", "Failed to load function hooks: \"updateWindowAnimatedDecorationValues\""},
				{"time", (uint64_t)10000},
				{"color", CColor(1.0, 0.0, 0.0, 1.0)},
				{"icon", ICON_ERROR},
			});
			return;
		}

		g_LastSelectionHook = HyprlandAPI::createFunctionHook(PHANDLE, decoUpdateCandidates[0].address, (void*)&hook_updateDecos);
	}

	void enable() {
		if (g_LastSelectionHook != nullptr) {
			g_LastSelectionHook->hook();
		}
	}

	void disable() {
		if (g_LastSelectionHook != nullptr) {
			g_LastSelectionHook->unhook();
		}
	}
}
