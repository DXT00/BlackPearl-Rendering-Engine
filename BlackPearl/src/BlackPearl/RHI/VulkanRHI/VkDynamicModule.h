#pragma once
#include "../DynamicModule.h"
namespace BlackPearl {
	class VkDynamicModule: public DynamicModule
	{
	public:
		DynamicRHI* CreateRHI() override;

	};

}

