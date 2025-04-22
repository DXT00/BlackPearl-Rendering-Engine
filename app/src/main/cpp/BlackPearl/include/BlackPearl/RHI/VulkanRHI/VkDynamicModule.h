#pragma once
#if GE_API_VULKAN

#include "../DynamicModule.h"
namespace BlackPearl {
	class VkDynamicModule: public DynamicModule
	{
	public:
		DynamicRHI* CreateRHI() override;

	};

}

#endif