#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
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

