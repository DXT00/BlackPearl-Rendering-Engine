#pragma once
#ifdef GE_API_OPENGL
#include "BlackPearl/Window.h"
#include <GLFW/glfw3.h>

namespace BlackPearl {
	class OpenGLWindow : public Window
	{
	public:
		OpenGLWindow()
			:Window() {
			Init();
			SetCursorCallBack();
		}
        virtual ~OpenGLWindow()
        {
            glfwDestroyWindow(m_Window);

        }
		void Init() override;
		void OnUpdate() override;
		void SetCursorCallBack();
		bool ShouldClose() override;
		bool IsKeyPressed(int keycode) override;
		bool IsMouseButtonPressed(int button) override;
		std::pair<float, float> GetMousePosition() override;
		void* GetNativeWindow() const override { return m_Window; }
		donut::math::vector<int, 2> GetCurWindowSize() override;

	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Context> m_Context;



	};

}

#endif