#include "pch.h"
#include "BlackPearl/Core.h"
#include "OpenGLDynamicModule.h"
#include "OpenGLDynamicRHI.h"

namespace BlackPearl {

	DynamicRHI* OpenGLDynamicModule::CreateRHI()
	{
		return DBG_NEW OpenGLDynamicRHI();
	}

}
