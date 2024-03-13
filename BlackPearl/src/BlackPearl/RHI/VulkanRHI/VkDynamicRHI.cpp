#include "pch.h"
#include "VkDynamicRHI.h"
#include "VkWindow.h"
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
