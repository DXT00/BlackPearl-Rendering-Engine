#include "pch.h"
#include "Window.h"
#include "BlackPearl/Core.h"
#include "Event/MouseEvent.h"
#include "BlackPearl/Config.h"
#define GLEW_STATIC
#include "GL/glew.h"
namespace BlackPearl {

	static bool s_GLFWInitialized = false;
	void Window::Init()
	{	//glfw:initialize and configure
		if (!s_GLFWInitialized) {
			int success = glfwInit();
			GE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback([](int error, const char* description) {
				GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
			});
			s_GLFWInitialized = true;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

		m_Context.reset(DBG_NEW Context(m_Window));
		m_Context->Init();
	}

	void Window::OnUpdate()
	{

		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}


	Window::Window(const WindowData & data)
	{
		m_Data.Height = data.Height;
		m_Data.Width = data.Width;
		m_Data.Title = data.Title;

		GE_CORE_INFO("Creating window {0} ({1} ,{2})", data.Title, data.Width, data.Height);

		this->Init();
		glfwSetWindowUserPointer(m_Window, &m_Data);

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

}