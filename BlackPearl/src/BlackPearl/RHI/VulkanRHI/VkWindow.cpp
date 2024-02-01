#include "pch.h"
#include "VkWindow.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "BlackPearl/Event/MouseEvent.h"
namespace BlackPearl {
	static bool g_GLFWInitialized = false;
	void VkWindow::Init()
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
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		glfwWindowHint(GLFW_SAMPLES, BlackPearl::Configuration::MSAA_SAMPLES);


		m_Window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), NULL, NULL);
		GE_ASSERT(m_Window, "fail to create window!");

		//glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		//glfwMakeContextCurrent(m_Window);//must be called before glewInit()
		// -------------------------------------
		// Initialize GLEW.
		// -------------------------------------
		//glewExperimental = GL_TRUE;
		//if (glewInit() == GLEW_OK) {
		//	GE_CORE_INFO(" GLEW initialized.");
		//}
		//else {
		//	GE_CORE_ERROR("GLEW failed to initialize (glewExperimental might not be supported).");
		//}

		/*m_Context.reset(DBG_NEW Context(m_Window));
		m_Context->Init();*/
	}
	void VkWindow::OnUpdate()
	{
		//glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}
	void VkWindow::SetCursorCallBack()
	{
		glfwSetWindowUserPointer(m_Window, &m_Data);

		//set callback
		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {

			MouseMovedEvent event(xpos, ypos);

			WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.EventCallback(event);

			});
	}
	bool VkWindow::ShouldClose()
	{
		if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(m_Window, true);
			return true;
		}
		return false;
	}
	bool VkWindow::IsKeyPressed(int keycode)
	{
		int status = glfwGetKey(m_Window, keycode);
		return status == GLFW_PRESS || status == GLFW_REPEAT;
	}
	bool VkWindow::IsMouseButtonPressed(int button)
	{
		int status = glfwGetMouseButton(m_Window, button);
		return status == GLFW_PRESS;
	}
	std::pair<float, float> VkWindow::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(m_Window, &xpos, &ypos);
		return { (float)xpos,(float)ypos };
	}
	
}
