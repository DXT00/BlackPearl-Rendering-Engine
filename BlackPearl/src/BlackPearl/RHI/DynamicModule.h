#pragma once
#include "DynamicRHI.h"
namespace BlackPearl {
	class DynamicModule
	{
	public:
		virtual DynamicRHI* CreateRHI() = 0;
	};
}


