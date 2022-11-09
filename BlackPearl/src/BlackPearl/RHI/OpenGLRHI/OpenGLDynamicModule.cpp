#include "pch.h"
#include "BlackPearl/Core.h"
#include "OpenGLDynamicModule.h"
#include "OpenGLDynamicRHI.h"

namespace BlackPearl {
	bool OpenGLDynamicModule::IsSupported()
	{
		return false;
	}

	DynamicRHI* OpenGLDynamicModule::CreateRHI()
	{
		return DBG_NEW OpenGLDynamicRHI();
	}

}
