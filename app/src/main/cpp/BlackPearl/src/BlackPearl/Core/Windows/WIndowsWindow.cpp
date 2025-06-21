//
// Created by DXT00 on 2025/4/13.
//
#include "pch.h"
#define GLEW_STATIC
#include "GL/glew.h"

#include "Core/Windows/WindowsWindow.h"
#include "BlackPearl/Application.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Event/WindowEvent.h"
#include <windowsx.h>
#ifdef USE_IMGUI
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"
#endif
#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
namespace BlackPearl {

	static LRESULT CALLBACK PC_PlatformGLWndproc(HWND hWnd, uint32_t Message, WPARAM wParam, LPARAM lParam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
			return true;
#endif
        WindowsWindow* pWindow = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if(!pWindow)
            return DefWindowProc(hWnd, Message, wParam, lParam);
        switch (Message) {
		//case WM_NCHITTEST: {
		//	LRESULT hit = DefWindowProc(hWnd, Message, wParam, lParam);
		//	return (hit == HTCLIENT) ? HTCAPTION : hit; // 允许客户区拖动
		//}
        case WM_SETCURSOR: {
            if (LOWORD(lParam) == HTCLIENT) {
                // 在客户区强制设置为箭头光标
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE; // 表示已处理此消息
            }
            break;
        }
        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            MouseMovedEvent* event = new MouseMovedEvent(x, y);
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_KEYDOWN: {
            if (wParam == VK_ESCAPE) {
                PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
            KeyPressedEvent *event = new KeyPressedEvent(wParam); // 需实现键码映射
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_KEYUP: {
    
            KeyReleasedEvent* event = new KeyReleasedEvent(wParam); // 需实现键码映射
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_LBUTTONDOWN: {
            KeyPressedEvent* event = new KeyPressedEvent(KeyCodes::Get(BP_MOUSE_BUTTON_LEFT));
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_RBUTTONDOWN: {
            KeyPressedEvent* event = new KeyPressedEvent(KeyCodes::Get(BP_MOUSE_BUTTON_RIGHT));
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_LBUTTONUP: {
            KeyReleasedEvent* event = new KeyReleasedEvent(KeyCodes::Get(BP_MOUSE_BUTTON_LEFT));
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_RBUTTONUP: {
            KeyReleasedEvent* event = new KeyReleasedEvent(KeyCodes::Get(BP_MOUSE_BUTTON_RIGHT));
            pWindow->ProcessEvent(event);
            break;
        }
        case WM_SIZE: {
            // 不包括边框栏
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            WindowResizeEvent* event = new WindowResizeEvent(width, height);
            pWindow->ProcessEvent(event);
           
        }
         return 0;
        case WM_CLOSE: {
            WindowCloseEvent* event = new WindowCloseEvent();
            pWindow->ProcessEvent(event);
            DestroyWindow(hWnd);
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, Message, wParam, lParam);
		}
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}


static bool g_GLFWInitialized = false;


void WindowsWindow::ProcessEvent(Event* event) {
    m_Queue.push(event);
	
}
unsigned int WindowsWindow::GetHeight() 
{
    return GetCurWindowSize().x;
}
unsigned int WindowsWindow::GetWidth() 
{
    return GetCurWindowSize().y;
}
//unsigned int __stdcall WindowThreadProc(void* param) {
//}

void WindowsWindow::Init()
{

    //EventCallback = ProcessEvent;
    ////glfw:initialize and configure
    //if (!g_GLFWInitialized) {
    //	int success = glfwInit();
    //	GE_ASSERT(success, "Could not intialize GLFW!");
    //	glfwSetErrorCallback([](int error, const char* description) {
    //		GE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    //		});
    //	g_GLFWInitialized = true;
    //}
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_SAMPLES, BlackPearl::Configuration::MSAA_SAMPLES);


    //m_Window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), NULL, NULL);
    //GE_ASSERT(m_Window, "fail to create window!");

    //glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    //glfwMakeContextCurrent(m_Window);//must be called before glewInit()
    Application& app = Application::Get();

    const TCHAR* WindowClassName = TEXT("PC_GLWindow");

    // Register a dummy window class.
    static bool bInitializedWindowClass = false;
    if (!bInitializedWindowClass)
    {
        //	WNDCLASS wc;

        bInitializedWindowClass = true;



        // 1. 定义窗口样式（和 CreateWindowEx 一致）
        DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
        // 2. 计算窗口矩形（RECT 的 right/bottom 是宽度和高度）
        RECT windowRect = { 0, 0, Configuration::WindowWidth, Configuration::WindowHeight }; // 客户区目标大小
        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, 0); // 调整窗口大小

        // 3. 创建窗口时使用调整后的尺寸(包括边框栏）
        int adjustedWidth = windowRect.right - windowRect.left;
        int adjustedHeight = windowRect.bottom - windowRect.top;


        //	//FMemory::Memzero(wc);
        //	memset(&wc, 0, sizeof(WNDCLASS));

        //	wc.style = CS_OWNDC;
        //	wc.lpfnWndProc = PC_PlatformGLWndproc;
        //	wc.cbClsExtra = 0;
        //	wc.cbWndExtra = 0;
        //	wc.hInstance = NULL;
        //	wc.hIcon = NULL;
        //	wc.hCursor = NULL;
        //	wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
        //	wc.lpszMenuName = NULL;
        //	wc.lpszClassName = WindowClassName;
        //	ATOM ClassAtom = ::RegisterClass(&wc);
        //	assert(ClassAtom);
        //}
        //	HINSTANCE HInstance = (HINSTANCE)GetModuleHandle(NULL);

        // Initialize the window class.
        WNDCLASSEX windowClass = {  };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        //windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = PC_PlatformGLWndproc;
        windowClass.hInstance = app.GetAppConf().hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"BlackPearl";
        RegisterClassEx(&windowClass);

        int32_t WinX = adjustedWidth;
        int32_t WinY = adjustedHeight;
        /*FParse::Value(FCommandLine::Get(), TEXT("WinX="), WinX);
        FParse::Value(FCommandLine::Get(), TEXT("WinY="), WinY);*/

        // Create a dummy window.
        //m_WindowHandle = CreateWindowEx(
        //	WS_EX_WINDOWEDGE,
        //	WindowClassName,
        //	NULL,
        //	WS_POPUP,
        //	WinX, WinY, 1, 1,
        //	NULL, NULL, NULL, NULL);
        //GE_ASSERT(m_WindowHandle);
        m_WindowHandle = CreateWindowEx(NULL,
                                        windowClass.lpszClassName,
                                        windowClass.lpszClassName, /*title*/
                                        dwStyle,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        WinX,
                                        WinY,
                                        NULL,
                                        NULL,
                                        windowClass.hInstance,
                                        this);
        ShowWindow(m_WindowHandle, SW_SHOW);
        UpdateWindow(m_WindowHandle);
        // 关键步骤：将 this 指针绑定到窗口
        //WindowsWindow* pWindow = this; // 假设 this 是有效的 WindowsWindow 对象
        SetWindowLongPtr(m_WindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    //    HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, WindowThreadProc, NULL, 0, NULL);

    }
}
void WindowsWindow::OnUpdate()
{
    /*glfwSwapBuffers(m_Window);
    glfwPollEvents();*/
   // UpdateWindow(m_WindowHandle);
    MSG msg = {};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE )) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }


    while (!m_Queue.empty()) {
        Event* event = m_Queue.front();
        m_Queue.pop();
        switch (event->GetEventType())
        {
        case EventType::WindowClose: {
            m_ShouldClose = true;
        }
        break;
        case EventType::WIndowResize: {
            m_CurrentWidth = static_cast<WindowResizeEvent*>(event)->GetWidth();
            m_CurrentHeight = static_cast<WindowResizeEvent*>(event)->GetHeight();
        }
        case EventType::KeyPressed: {
            m_KeyPressMap[static_cast<KeyPressedEvent*>(event)->GetKeyCode()] = true;
        }
                                  break;
        case EventType::KeyReleased: {
            m_KeyPressMap[static_cast<KeyReleasedEvent*>(event)->GetKeyCode()] = false;
        }
                                   break;
        case EventType::MouseMoved: {
            m_Xpos = static_cast<MouseMovedEvent*>(event)->GetMouseX();
            m_Ypos = static_cast<MouseMovedEvent*>(event)->GetMouseY();

        }
          break;

        default:
            break;
        }
        if (event)
            delete event;
    }

}
void WindowsWindow::SetCursorCallBack()
{
    //glfwSetWindowUserPointer(m_Window, &m_Data);

    ////set callback
    //glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {

    //    MouseMovedEvent event(xpos, ypos);

    //    WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
    //    data.EventCallback(event);

    //});
}
bool WindowsWindow::ShouldClose()
{
   /* if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
        return true;
    }
    return false;*/
    return m_ShouldClose;
}
bool WindowsWindow::IsKeyPressed(int keycode)
{
    if (m_KeyPressMap.find(keycode) != m_KeyPressMap.end()) {
      
        bool press = m_KeyPressMap[keycode];
        m_KeyPressMap[keycode] = false;
        return press;
    }
    else {
        return false;
    }
   /*     int status = glfwGetKey(m_Window, keycode);
    return status == GLFW_PRESS || status == GLFW_REPEAT;*/
}
bool WindowsWindow::IsMouseButtonPressed(int button)
{
    /*int status = glfwGetMouseButton(m_Window, button);
    return status == GLFW_PRESS;*/

    if (m_KeyPressMap.find(button) != m_KeyPressMap.end()) {

        return m_KeyPressMap[button];
    }
    else {
        return false;
    }
}
std::pair<float, float> WindowsWindow::GetMousePosition()
{
    /*double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    return { (float)xpos,(float)ypos };*/
    return { m_Xpos, m_Ypos };
}
math::vector<int, 2> WindowsWindow::GetCurWindowSize()
{
    RECT windowRect;
    GetWindowRect(m_WindowHandle, &windowRect);

    int width = windowRect.right - windowRect.left;  // 窗口总宽度（含边框）
    int height = windowRect.bottom - windowRect.top; // 窗口总高度（含标题栏）
   /* int width;
    int height;
    glfwGetWindowSize(m_Window, &width, &height);*/
    return math::vector<int, 2>(m_CurrentWidth, m_CurrentHeight);
}
}

