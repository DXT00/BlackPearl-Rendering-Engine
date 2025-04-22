#include "pch.h"
#if GE_API_VULKAN

#include "RHI/VulkanRHI/VkDynamicRHI.h"
#include "RHI/VulkanRHI/VkWindow.h"
#include "BlackPearl/Application.h"


namespace BlackPearl {

  
	/* vk Window is the same as opengl Window */
	Window* VkDynamicRHI::InitWindow()
	{
		return DBG_NEW VkWindow();
	}

	void VkDynamicRHI::InitRHI()
	{
    
	}

   
}
#endif