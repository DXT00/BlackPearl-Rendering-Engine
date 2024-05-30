#include "pch.h"
#if GE_API_VULKAN

#include "VkDynamicModule.h"
#include "VkDynamicRHI.h"

namespace BlackPearl {
	DynamicRHI* VkDynamicModule::CreateRHI()
	{
		return DBG_NEW VkDynamicRHI();
	}

}
#endif
