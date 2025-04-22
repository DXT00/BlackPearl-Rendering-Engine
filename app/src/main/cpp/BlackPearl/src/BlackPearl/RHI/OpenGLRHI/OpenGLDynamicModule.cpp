#include "pch.h"
#include "BlackPearl/Core.h"
#include "RHI/OpenGLRHI/OpenGLDynamicModule.h"
#include "RHI/OpenGLRHI/OpenGLDynamicRHI.h"

namespace BlackPearl {

	DynamicRHI* OpenGLDynamicModule::CreateRHI()
	{
		return DBG_NEW OpenGLDynamicRHI();
	}

}
