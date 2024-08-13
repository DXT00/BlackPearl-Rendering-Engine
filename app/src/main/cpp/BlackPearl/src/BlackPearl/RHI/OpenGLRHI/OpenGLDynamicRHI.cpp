#include "pch.h"
#include "OpenGLDynamicRHI.h"
#include "OpenGLWindow.h"
#include "OpenGLLogger.h"
namespace BlackPearl {
	Window* OpenGLDynamicRHI::InitWindow()
	{
#ifdef GE_PLATFORM_WINDOWS
		return DBG_NEW OpenGLWindow();
#else
        return nullptr;
#endif
	}

	void OpenGLDynamicRHI::InitRHI()
	{
	}

	void OpenGLDynamicRHI::EngineExit()
	{
#ifdef GE_PLATFORM_WINDOWS
		glfwTerminate();
#endif
	}

	/*void OpenGLDynamicRHI::InitLogger() {
		m_OpenGLLogger = std::make_shared<OpenGLLogger>();
	}*/

}
