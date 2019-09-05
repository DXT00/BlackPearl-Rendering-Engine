#pragma once
#include<string>
#include "GameEngine/Renderer/Context.h"

class Window
{
public:
	struct WindowData {
		std::string Title;
		unsigned int Height;
		unsigned int Width;
		WindowData(const std::string& title = "GameEngine", unsigned int width = 800, unsigned int height = 600)
			:Title(title), Width(width), Height(height) {}
	};
	Window(const WindowData& data = WindowData());
	~Window();
	inline unsigned int GetHeight() const { return m_Data.Height; }
	inline unsigned int GetWidth() const { return m_Data.Width; }
	inline std::string GetTitle() const { return m_Data.Title; }
	inline GLFWwindow* GetNativeWindow() const { return m_Window; }


private:
	GLFWwindow* m_Window;
	Context* m_Context;

	WindowData m_Data;
};

