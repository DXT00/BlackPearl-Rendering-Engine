#pragma once
#ifdef GE_PLATFORM_WINDOWS
#include <GLFW/glfw3.h>

namespace BlackPearl {

	class Context
	{
	public:
		Context(GLFWwindow* windowHandle);
		~Context();
		void Init();
		void SwapBuffers();

	private:
		GLFWwindow* m_WindowHandle;
	};

}
#endif