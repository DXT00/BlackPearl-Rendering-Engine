#include "pch.h"
#ifdef GE_API_OPENGL
#ifdef GE_PLATFORM_WINDOWS
#include "GL/glew.h"

#include "OpenGLWindow.h"
#define GLEW_STATIC
#include "BlackPearl/Event/MouseEvent.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	static bool g_GLFWInitialized = false;
	void OpenGLWindow::Init()
	{
		//glfw:initialize and configure
		if (!g_GLFWInitialized) {
			int success = glfwInit();
			GE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback([](int error, const char* description) {
				GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
				});
			g_GLFWInitialized = true;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, BlackPearl::Configuration::MSAA_SAMPLES);


		m_Window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), NULL, NULL);
		GE_ASSERT(m_Window, "fail to create window!");

		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		glfwMakeContextCurrent(m_Window);//must be called before glewInit()
		// -------------------------------------
		// Initialize GLEW.
		// -------------------------------------
		glewExperimental = GL_TRUE;
		if (glewInit() == GLEW_OK) {
			GE_CORE_INFO(" GLEW initialized.");
		}
		else {
			GE_CORE_ERROR("GLEW failed to initialize (glewExperimental might not be supported).");
		}
#ifdef GE_PLATFORM_WINDOWS

		m_Context.reset(DBG_NEW Context(m_Window));
		m_Context->Init();
#endif
	}
	void OpenGLWindow::OnUpdate()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
	void OpenGLWindow::SetCursorCallBack()
	{
		glfwSetWindowUserPointer(m_Window, &m_Data);

		//set callback
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {

			MouseMovedEvent event(xpos, ypos);

			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.EventCallback(event);

			});
	}
	bool OpenGLWindow::ShouldClose()
	{
		if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(m_Window, true);
			return true;
		}
		return false;
	}
	bool OpenGLWindow::IsKeyPressed(int keycode)
	{
		int status = glfwGetKey(m_Window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}
	bool OpenGLWindow::IsMouseButtonPressed(int button)
	{
		int status = glfwGetMouseButton(m_Window, button);
		return status == GLFW_PRESS;
	}
	std::pair<float, float> OpenGLWindow::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(m_Window, &xpos, &ypos);
		return { (float)xpos,(float)ypos };
	}
	donut::math::vector<int, 2> OpenGLWindow::GetCurWindowSize()
	{
		int width;
		int height;
		glfwGetWindowSize(m_Window, &width, &height);
		return donut::math::vector<int, 2>(width, height);
	}
}
#endif
#endif
