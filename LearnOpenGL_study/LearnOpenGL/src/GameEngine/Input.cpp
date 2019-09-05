#include "pch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Application.h"
 bool Input::IsKeyPressed(int keycode)
{
	GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
	int status = glfwGetKey(window, keycode);

	return status == GLFW_PRESS ||status == GLFW_REPEAT;
}

 bool Input::IsMouseButtonPressed(int button)
{
	GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
	int status = glfwGetMouseButton(window, button);

	return status == GLFW_PRESS;
}

 std::pair<float, float> Input::GetMousePosition()
{
	GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	return { (float)xpos,(float)ypos };
}

 float Input::GetMouseX()
{
	auto[x, y] = GetMousePosition();
	return x;
}

 float Input::GetMouseY()
{
	auto[x, y] = GetMousePosition();
	return y;
}
