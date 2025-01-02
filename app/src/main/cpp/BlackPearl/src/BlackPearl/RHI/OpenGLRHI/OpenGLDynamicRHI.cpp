#include "pch.h"
#include "OpenGLDynamicRHI.h"
#include "OpenGLWindow.h"
#include "OpenGLLogger.h"
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
		glfwTerminate();

	}

	/*void OpenGLDynamicRHI::InitLogger() {
		m_OpenGLLogger = std::make_shared<OpenGLLogger>();
	}*/

}
