#pragma once
#include "DynamicRHI.h"
namespace BlackPearl {
	class DynamicModule
	{
	public:
		/** Checks whether the RHI is supported by the current system. */
		virtual bool IsSupported() = 0;

		virtual DynamicRHI* CreateRHI() = 0;
	};
}


