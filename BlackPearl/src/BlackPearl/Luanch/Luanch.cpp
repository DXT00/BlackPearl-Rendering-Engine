#include "pch.h"
#include "Luanch.h"
#include "BlackPearl/RHI/DynamicRHI.h"

namespace BlackPearl {
	bool g_shouldEngineExit = false;

	bool ShouldEngineExit() {
		return g_shouldEngineExit;
	}
	void RequestEngineExit() {
		g_shouldEngineExit = true;
	}
}