#pragma once
#include<string>
#include "BlackPearl/Config.h"
#include "Event/Event.h"
#include "BlackPearl/Renderer/Context.h"
#include "BlackPearl/Math/vector.h"

namespace BlackPearl {

	class Window
	{
	public:
		struct WindowData {
			std::string Title;
			unsigned int Height;
			unsigned int Width;
			WindowData(const std::string& title = "BlackPearl", unsigned int width = Configuration::WindowWidth, unsigned int height = Configuration::WindowHeight)//1920：1080 对应的PerspeciveCamera projectionMatrix也要改
				:Title(title), Width(width), Height(height) {}

			std::function<void(Event&)> EventCallback;

		};
		Window(const WindowData& data = WindowData());
		~Window();
		virtual void Init() = 0;
		virtual void OnUpdate() = 0;
		virtual bool ShouldClose() = 0;
		virtual bool IsKeyPressed(int keycode) = 0;
		virtual bool IsMouseButtonPressed(int button) = 0;
		virtual std::pair<float, float> GetMousePosition() = 0;
		virtual math::vector<int, 2> GetCurWindowSize() = 0;

		inline unsigned int GetHeight() const { return m_Data.Height; }
		inline unsigned int GetWidth() const { return m_Data.Width; }
		inline std::string GetTitle() const { return m_Data.Title; }
		inline void SetAppCallBack(const std::function<void(Event&)>& callback) { m_Data.EventCallback = callback; }
		virtual void* GetNativeWindow() const = 0;

	protected:
		WindowData m_Data;
	};

}