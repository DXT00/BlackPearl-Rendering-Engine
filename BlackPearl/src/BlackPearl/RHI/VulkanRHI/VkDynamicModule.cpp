#include "pch.h"
#include "VkDynamicModule.h"
#include "VkDynamicRHI.h"

namespace BlackPearl {
	DynamicRHI* VkDynamicModule::CreateRHI()
	{
		return DBG_NEW VkDynamicRHI();
	}

}
