//
// Created by DXT00 on 2025/4/13.
//
#include "pch.h"
#define GLEW_STATIC
#include "GL/glew.h"

#include "Core/Windows/WindowsWindow.h"
#include "BlackPearl/Event/MouseEvent.h"
#include "BlackPearl/Application.h"
namespace BlackPearl {

    static LRESULT CALLBACK PC_PlatformGLWndproc(HWND hWnd, uint32_t Message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, Message, wParam, lParam);
}


static bool g_GLFWInitialized = false;
void WindowsWindow::Init()
{
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
        WNDCLASSEX windowClass = { 0 };
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = PC_PlatformGLWndproc;
        windowClass.hInstance = app.GetAppConf().hInstance;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.lpszClassName = L"BlackPearl";
        RegisterClassEx(&windowClass);

        int32_t WinX = Configuration::WindowWidth;
        int32_t WinY = Configuration::WindowHeight;
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
                                        WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT, CW_USEDEFAULT,
                                        WinX,
                                        WinY,
                                        NULL,
                                        NULL,
                                        windowClass.hInstance,
                                        this);
        ShowWindow(m_WindowHandle, SW_SHOW);
    }
}
void WindowsWindow::OnUpdate()
{
    /*glfwSwapBuffers(m_Window);
    glfwPollEvents();*/




}
void WindowsWindow::SetCursorCallBack()
{
    glfwSetWindowUserPointer(m_Window, &m_Data);

    //set callback
    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos) {

        MouseMovedEvent event(xpos, ypos);

        WindowData data = *(WindowData*)glfwGetWindowUserPointer(window);
        data.EventCallback(event);

    });
}
bool WindowsWindow::ShouldClose()
{
    if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(m_Window, true);
        return true;
    }
    return false;
}
bool WindowsWindow::IsKeyPressed(int keycode)
{
    int status = glfwGetKey(m_Window, keycode);
    return status == GLFW_PRESS || status == GLFW_REPEAT;
}
bool WindowsWindow::IsMouseButtonPressed(int button)
{
    int status = glfwGetMouseButton(m_Window, button);
    return status == GLFW_PRESS;
}
std::pair<float, float> WindowsWindow::GetMousePosition()
{
    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    return { (float)xpos,(float)ypos };
}
math::vector<int, 2> WindowsWindow::GetCurWindowSize()
{
    int width;
    int height;
    glfwGetWindowSize(m_Window, &width, &height);
    return math::vector<int, 2>(width, height);
}
}

