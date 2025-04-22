#pragma once
#include "BlackPearl/Window.h"
#include "dinput.h"
namespace BlackPearl {
	class D3D12Window: public Window
	{
	public:
		D3D12Window()
		:Window(){
			Init();
		}
		~D3D12Window(){
			if (m_DI) {
				m_DI->Release();
			}
		}
		void Init() override;
		void OnUpdate() override;
		void SetCursorCallBack();
		bool ShouldClose() override;
		bool IsKeyPressed(int keycode) override;
		bool IsMouseButtonPressed(int button) override;
		std::pair<float, float> GetMousePosition() override;
		void* GetNativeWindow() const override { return m_hwnd; }
		math::vector<int, 2> GetCurWindowSize() override;

	private:
		static LRESULT CALLBACK D3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		HWND m_hwnd;
		bool m_CloseWindow;
		unsigned int m_PressKey;
		unsigned int m_PressMouseButton;
		std::pair<float, float> m_MousePosition;

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		IDirectInputDevice8A* m_Mouse;
#endif
		DIMOUSESTATE2 m_MouseState;
		bool m_MouseButtons[8];
		IDirectInput8A* m_DI;



	};

}

