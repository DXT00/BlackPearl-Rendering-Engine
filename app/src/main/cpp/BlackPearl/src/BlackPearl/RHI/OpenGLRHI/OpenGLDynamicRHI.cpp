#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLDynamicRHI.h"
#include "RHI/OpenGLRHI/OpenGLWindow.h"
#include "RHI/OpenGLRHI/OpenGLLogger.h"
namespace BlackPearl {
	Window* OpenGLDynamicRHI::InitWindow()
	{
		return DBG_NEW OpenGLWindow();
	}

	void OpenGLDynamicRHI::InitRHI()
	{
	}

	void OpenGLDynamicRHI::EngineExit()
	{
#ifdef GE_PLATFORM_WINDOWS
		glfwTerminate();
#endif
        //Android todo:: terminate in: ~FPlatformOpenGLDevice()
        //FAndroidAppEntry::ReleaseEGL();
	}

	/*void OpenGLDynamicRHI::InitLogger() {
		m_OpenGLLogger = std::make_shared<OpenGLLogger>();
	}*/

}
