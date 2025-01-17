#pragma once
#include "OpenGLState.h"
namespace BlackPearl {

	class OpenGLContext
	{

	public:
		/** Underlying platform-specific data */
		FPlatformOpenGLDevice* PlatformDevice;
	};
}
