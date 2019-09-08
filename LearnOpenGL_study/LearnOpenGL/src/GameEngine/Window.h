#pragma once
#include<string>
#include "GameEngine/Renderer/Context.h"
#include "Event/Event.h"

class Window
{
public:
	struct WindowData {
		std::string Title;
		unsigned int Height;
		unsigned int Width;
		WindowData(const std::string& title = "GameEngine", unsigned int width = 800, unsigned int height = 600)
			:Title(title), Width(width), Height(height) {}

		std::function<void(Event&)> EventCallback;

	};
	Window(const WindowData& data = WindowData());
	~Window();
	void Init();
	inline unsigned int GetHeight() const { return m_Data.Height; }
	inline unsigned int GetWidth() const { return m_Data.Width; }
	inline std::string GetTitle() const { return m_Data.Title; }
	inline GLFWwindow* GetNativeWindow() const { return m_Window; }
	inline void SetCallBack(const std::function<void(Event&)>& callback) { m_Data.EventCallback = callback; }


private:

	GLFWwindow* m_Window;
	Context* m_Context;
	WindowData m_Data;
};

