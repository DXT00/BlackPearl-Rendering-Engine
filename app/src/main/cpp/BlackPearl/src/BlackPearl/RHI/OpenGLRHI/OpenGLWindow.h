#pragma once
#include "BlackPearl/Window.h"
namespace BlackPearl {
	//TODO:: Distinguish between Android and pc versions
	class OpenGLWindow :public Window
	{
	public:
		OpenGLWindow()
			:Window() {
			Init();
			SetCursorCallBack();
		}
		void Init() override;
		void OnUpdate() override;
		void SetCursorCallBack();
		bool ShouldClose() override;
		bool IsKeyPressed(int keycode) override;
		bool IsMouseButtonPressed(int button) override;
		std::pair<float, float> GetMousePosition() override;
		//void* GetNativeWindow() const override { return m_Window; }
		void* GetNativeWindow() const override { return m_WindowHandle; }

		math::vector<int, 2> GetCurWindowSize() override;

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Context> m_Context;

		HWND m_WindowHandle;

	};

}

