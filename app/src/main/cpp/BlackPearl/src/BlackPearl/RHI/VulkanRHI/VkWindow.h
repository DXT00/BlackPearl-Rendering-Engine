#pragma once
#if GE_API_VULKAN

#include "BlackPearl/Window.h"

namespace BlackPearl {
	class VkWindow :public Window
	{
	public:
		VkWindow()
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
		void* GetNativeWindow() const override { return m_Window; }
		virtual math::vector<int, 2> GetCurWindowSize() override;
	private:
		GLFWwindow* m_Window;
		std::unique_ptr<Context> m_Context;
	};
}
#endif