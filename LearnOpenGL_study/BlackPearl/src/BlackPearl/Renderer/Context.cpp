#include "pch.h"
#include "Context.h"
#include "glad/glad.h"
#include"BlackPearl/Core.h"
namespace BlackPearl {

	Context::Context(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle)
	{
		GE_ASSERT(m_WindowHandle, "windowHandle is null");
	}


	Context::~Context()
	{
		//delete m_WindowHandle;
	}

	void Context::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
		});

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GE_ASSERT(status, "Failed to initialize Glad!");

		GE_CORE_INFO("OpenGL Vendor :{0}", glGetString(GL_VENDOR));
		GE_CORE_INFO("OpenGL Renderer :{0}", glGetString(GL_RENDERER));
		GE_CORE_INFO("OpenGL Version :{0}", glGetString(GL_VERSION));
	}

	void Context::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}
}