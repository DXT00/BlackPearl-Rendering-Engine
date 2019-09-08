#include "pch.h"
#include "Window.h"
#include "GameEngine/Core.h"
#include "Event/MouseEvent.h"

static bool s_GLFWInitialized = false;


void Window::Init()
{	//glfw:initialize and configure
	if (!s_GLFWInitialized) {
		int success =glfwInit();
		GE_ASSERT(success, "Could not intialize GLFW!");
		glfwSetErrorCallback([](int error, const char* description) {
			GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
		});
		s_GLFWInitialized = true;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_Window = glfwCreateWindow( GetWidth(), GetHeight(), GetTitle().c_str(), NULL, NULL);
	GE_ASSERT(m_Window, "fail to create window!")

	glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	m_Context = new Context(m_Window);
	m_Context->Init();
}


Window::Window(const WindowData & data)
{
	m_Data.Height = data.Height;
	m_Data.Width = data.Width;
	m_Data.Title = data.Title;

	GE_CORE_INFO("Creating window {0} ({1} ,{2})", data.Title, data.Width, data.Height);
	
	this->Init();
	glfwSetWindowUserPointer(m_Window,&m_Data);

	//set callback
	glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {
	
		MouseMovedEvent event(xpos, ypos);

		WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
		data.EventCallback(event);

	
	});

}

Window::~Window()
{
}

