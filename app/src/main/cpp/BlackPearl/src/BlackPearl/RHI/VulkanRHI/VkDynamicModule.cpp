#include "pch.h"
#if GE_API_VULKAN

#include "RHI/VulkanRHI/VkDynamicModule.h"
#include "RHI/VulkanRHI/VkDynamicRHI.h"

namespace BlackPearl {
	DynamicRHI* VkDynamicModule::CreateRHI()
	{
		return DBG_NEW VkDynamicRHI();
	}

}
#endif
