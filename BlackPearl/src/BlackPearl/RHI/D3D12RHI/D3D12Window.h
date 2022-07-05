#pragma once
#include "BlackPearl/Window.h"
namespace BlackPearl {
	class D3D12Window: public Window
	{
	public:
		D3D12Window()
		:Window(){
			Init();
		}
		void Init() override;
		void OnUpdate() override;
		void SetCursorCallBack();
		bool ShouldClose() override;
		bool IsKeyPressed(int keycode) override;
		bool IsMouseButtonPressed(int button) override;
		std::pair<float, float> GetMousePosition() override;
		void* GetNativeWindow() const override { return m_hwnd; }

	private:
		static LRESULT CALLBACK D3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		HWND m_hwnd;
		bool m_CloseWindow;
		unsigned int m_PressKey;
	};

}

