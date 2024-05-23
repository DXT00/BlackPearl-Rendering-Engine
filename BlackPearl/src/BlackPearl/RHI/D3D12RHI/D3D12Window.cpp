#include "pch.h"
#include "D3D12Window.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/KeyCodes.h"
#include "dinput.h"
#pragma comment(lib, "dinput8")

namespace BlackPearl {
	class Application;
	void D3D12Window::Init()
	{
		Application& app = Application::Get();

		// Initialize the window class.
		WNDCLASSEX windowClass = { 0 };
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = app.GetAppConf().hInstance;
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.lpszClassName = L"BlackPearl";
		RegisterClassEx(&windowClass);

		m_hwnd = CreateWindowEx(NULL,
			windowClass.lpszClassName,
			windowClass.lpszClassName, /*title*/
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			GetWidth(),
			GetHeight(),
			NULL,
			NULL,
			windowClass.hInstance,
			this);
		m_CloseWindow = false;
		ShowWindow(m_hwnd, app.GetAppConf().nShowCmd);
		//DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DI, nullptr);
		ThrowIfFailed(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_DI, nullptr),
			L"DirectInput8 initialization failed.");
		ThrowIfFailed(m_DI->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr), L"Mouse CreateDevice failed.");
		ThrowIfFailed(m_Mouse->SetDataFormat(&c_dfDIMouse2), L"Mouse SetDataFormat failed.");
		memset(&m_MouseState, 0, sizeof(DIMOUSESTATE2));

	}
	void D3D12Window::OnUpdate()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				m_CloseWindow = true;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
		HWND foreground = GetForegroundWindow();
		bool visible = IsWindowVisible(foreground) != 0;

		for (size_t i = 0; i < VK_MOUSECOUNT; i++)
		{
			m_MouseButtons[i] = false;
		}

		if (foreground != m_hwnd // wouldn't be able to acquire
			|| !visible)
		{
			memset(&m_MouseState, 0, sizeof(DIMOUSESTATE2));
		}
		else
		{
			m_Mouse->Acquire();
			m_Mouse->GetDeviceState(sizeof(DIMOUSESTATE2), &m_MouseState);

		}

		for (UINT i = 0; i < VK_MOUSECOUNT; ++i)
		{
			if (m_MouseState.rgbButtons[i] > 0)
				m_MouseButtons[i] = true;
		}
#endif
		
	}
	void D3D12Window::SetCursorCallBack()
	{
	}
	bool D3D12Window::ShouldClose()
	{
		return m_CloseWindow;
	}
	bool D3D12Window::IsKeyPressed(int keycode)
	{
		return m_PressKey == keycode;
	}
	bool D3D12Window::IsMouseButtonPressed(int button)
	{
		GE_ASSERT(button < VK_MOUSECOUNT, "MOUSE BUTTON INVALID")
		GE_ASSERT(button >= VK_MOUSELEFT, "MOUSE BUTTON INVALID")

		return m_MouseButtons[button] == true;
	}
	std::pair<float, float> D3D12Window::GetMousePosition()
	{
		return { m_MousePosition.first, m_MousePosition.second};
	}

	donut::math::vector<int, 2> D3D12Window::GetCurWindowSize()
	{
		HWND foreground = GetForegroundWindow();
		RECT rct;
		GetWindowRect(foreground, &rct);
		int width = rct.right - rct.left;   //窗口的宽度
		int height = rct.bottom - rct.top;  //窗口的高度

		return donut::math::vector<int, 2>(width, height);
	}

	LRESULT CALLBACK D3D12Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
		D3D12Window* pWindow = reinterpret_cast<D3D12Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		switch (message)
		{
		case WM_CREATE:
		{
			// Save the DXSample* passed in to CreateWindow.
			LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
		}
		return 0;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
					L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					pWindow->m_CloseWindow = true;
					DestroyWindow(hWnd);
				}
			}
			else {
				pWindow->m_PressKey = static_cast<int>(wParam);
			}
			return 0;
		case WM_KEYUP:
			pWindow->m_PressKey = 0x00;
			return 0;
		case WM_MOUSEMOVE:
			if (static_cast<UINT8>(wParam) == MK_RBUTTON)
			{
				pWindow->m_MousePosition.first = LOWORD(lParam);
				pWindow->m_MousePosition.second = HIWORD(lParam);
			}
			return 0;
		case WM_RBUTTONDOWN:
			pWindow->m_PressMouseButton = WM_RBUTTONDOWN;
			return 0;
		case WM_RBUTTONUP:
			pWindow->m_PressMouseButton = WM_RBUTTONUP;
			return 0;
		case WM_LBUTTONDOWN:
			pWindow->m_PressMouseButton = WM_LBUTTONDOWN;
			return 0;
		case WM_LBUTTONUP:
			pWindow->m_PressMouseButton = WM_LBUTTONUP;
			return 0;
		case WM_DESTROY: // x button on top right corner of window was pressed
			pWindow->m_CloseWindow = true;
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd,
			message,
			wParam,
			lParam);
	}
}
