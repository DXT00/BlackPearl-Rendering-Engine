#include "pch.h"
#include "OpenGLDynamicRHI.h"
#include "OpenGLWindow.h"

namespace BlackPearl {
	Window* OpenGLDynamicRHI::InitWindow()
	{
		return new OpenGLWindow();
	}

}
