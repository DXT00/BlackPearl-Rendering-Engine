#include "pch.h"
#include "Input.h"
#include <GLFW/glfw3.h>
#include "Application.h"
namespace BlackPearl {

	bool Input::s_FirstMouse = true;
	bool Input::IsKeyPressed(int keycode)
	{
		return Application::Get().GetWindow().IsKeyPressed(keycode);
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return Application::Get().GetWindow().IsMouseButtonPressed(button);
	}

	std::pair<float, float> Input::GetMousePosition()
	{
		return Application::Get().GetWindow().GetMousePosition();
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
}