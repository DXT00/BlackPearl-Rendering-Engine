#include "pch.h"
#include "D3D12Window.h"
#include "BlackPearl/Application.h"
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
			NULL);
		m_CloseWindow = false;
		ShowWindow(m_hwnd, app.GetAppConf().nShowCmd);

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
		return false;
	}
	std::pair<float, float> D3D12Window::GetMousePosition()
	{
		return std::pair<float, float>();
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
