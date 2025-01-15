#include "pch.h"
#include "../OpenGLDrvPrivate.h"
#include "OpenGLDrvWindows.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLViewport.h"
#include "BlackPearl/RHI/RHIDeviceContext.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDynamicRHI.h"
#include "BlackPearl/Core/ScopeLock.h"
#include "BlackPearl/Core/CriticalSection.h"
// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLWindowsLoader.cpp: Manual loading of OpenGL functions from DLL.
=============================================================================*/

namespace BlackPearl {
//
//
//#define DEFINE_GL_ENTRYPOINTS(Type,Func) Type Func = NULL;
//ENUM_GL_ENTRYPOINTS_ALL(DEFINE_GL_ENTRYPOINTS);
//#undef DEFINE_GL_ENTRYPOINTS
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;

extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT_ProcAddress;	// set in OpenGLDevice.cpp

bool GRunningUnderRenderDoc = false;

/*------------------------------------------------------------------------------
	OpenGL context management.
------------------------------------------------------------------------------*/

static void ContextMakeCurrent(HDC DC, HGLRC RC)
{
	bool Result = wglMakeCurrent(DC, RC);
	if (!Result)
	{
		Result = wglMakeCurrent(nullptr, nullptr);
	}
	assert(Result);
}

static HGLRC GetCurrentContext()
{
	return wglGetCurrentContext();
}


/** Platform specific OpenGL context. */
struct FPlatformOpenGLContext
{
	HWND WindowHandle;
	HDC DeviceContext;
	HGLRC OpenGLContext;
	bool bReleaseWindowOnDestroy;
	int32_t SyncInterval;
	GLuint	ViewportFramebuffer;
	GLuint	VertexArrayObject;	// one has to be generated and set for each context (OpenGL 3.2 Core requirements)
	GLuint	BackBufferResource;
	GLenum	BackBufferTarget;
};

class FScopeContext
{
public:
	FScopeContext(FPlatformOpenGLContext* Context)
	{
		assert(Context);
		PrevDC = wglGetCurrentDC();
		PrevContext = GetCurrentContext();
		bSameDC = (PrevDC == Context->DeviceContext);
		bSameDCAndContext = (PrevContext == Context->OpenGLContext) && bSameDC;
		if (!bSameDCAndContext)
		{
			//			if (PrevContext)
			//			{
			//				glFlush();
			//			}
						// no need to glFlush() on Windows, it does flush by itself before switching contexts
			ContextMakeCurrent(Context->DeviceContext, Context->OpenGLContext);
		}
	}

	~FScopeContext(void)
	{
		if (!bSameDCAndContext)
		{
			//			glFlush();	// not needed on Windows, it does flush by itself before switching contexts
			if (PrevContext)
			{
				ContextMakeCurrent(PrevDC, PrevContext);
			}
			else
			{
				ContextMakeCurrent(NULL, NULL);
			}
		}
	}

	bool ContextWasAlreadyActive() const
	{
		return bSameDCAndContext;
	}

	bool ContextsShareSameDC() const
	{
		return bSameDC;
	}

private:
	HDC					PrevDC;
	HGLRC				PrevContext;
	bool				bSameDCAndContext;
	bool				bSameDC;
};

void DeleteQueriesForCurrentContext(HGLRC Context);

/**
 * A dummy wndproc.
 */
static LRESULT CALLBACK PlatformDummyGLWndproc(HWND hWnd, uint32_t Message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, Message, wParam, lParam);
}

/**
 * Initialize a pixel format descriptor for the given window handle.
 */
static void PlatformInitPixelFormatForDevice(HDC DeviceContext, bool bTryIsDummyContext)
{
	// Pixel format descriptor for the context.
	PIXELFORMATDESCRIPTOR PixelFormatDesc;
	memset(&PixelFormatDesc, 0, sizeof(PIXELFORMATDESCRIPTOR));
	//FMemory::Memzero(PixelFormatDesc);
	PixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	PixelFormatDesc.nVersion = 1;
	PixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	PixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
	PixelFormatDesc.cColorBits = 32;
	PixelFormatDesc.cDepthBits = 0;
	PixelFormatDesc.cStencilBits = 0;
	PixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

	static bool bRequestedQuadBufferStereo = false;// FParse::Param(FCommandLine::Get(), TEXT("quad_buffer_stereo"));
	if (bRequestedQuadBufferStereo)
	{
		PixelFormatDesc.dwFlags = PixelFormatDesc.dwFlags | PFD_STEREO;
	}

	// Set the pixel format and create the context.
	int32_t PixelFormat = ChoosePixelFormat(DeviceContext, &PixelFormatDesc);
	if (!PixelFormat || !SetPixelFormat(DeviceContext, PixelFormat, &PixelFormatDesc))
	{
		//UE_LOG(LogRHI, Fatal, TEXT("Failed to set pixel format for device context."));
	}
}

/**
 * Create a dummy window used to construct OpenGL contexts.
 */
static void PlatformCreateDummyGLWindow(FPlatformOpenGLContext* OutContext)
{
	const TCHAR* WindowClassName = TEXT("DummyGLWindow");

	// Register a dummy window class.
	static bool bInitializedWindowClass = false;
	if (!bInitializedWindowClass)
	{
		WNDCLASS wc;

		bInitializedWindowClass = true;
		//FMemory::Memzero(wc);
		memset(&wc, 0, sizeof(WNDCLASS));

		wc.style = CS_OWNDC;
		wc.lpfnWndProc = PlatformDummyGLWndproc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = NULL;
		wc.hIcon = NULL;
		wc.hCursor = NULL;
		wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = WindowClassName;
		ATOM ClassAtom = ::RegisterClass(&wc);
		assert(ClassAtom);
	}

	int32_t WinX = Configuration::WindowWidth;
	int32_t WinY = Configuration::WindowHeight;
	/*FParse::Value(FCommandLine::Get(), TEXT("WinX="), WinX);
	FParse::Value(FCommandLine::Get(), TEXT("WinY="), WinY);*/

	// Create a dummy window.
	OutContext->WindowHandle = CreateWindowEx(
		WS_EX_WINDOWEDGE,
		WindowClassName,
		NULL,
		WS_POPUP,
		WinX, WinY, 1, 1,
		NULL, NULL, NULL, NULL);
	assert(OutContext->WindowHandle);
	OutContext->bReleaseWindowOnDestroy = true;

	// Get the device context.
	OutContext->DeviceContext = GetDC(OutContext->WindowHandle);
	assert(OutContext->DeviceContext);
	PlatformInitPixelFormatForDevice(OutContext->DeviceContext, true);
}

/**
 * Set OpenGL Context version to fixed version
 */

static void GetOpenGLVersionForCoreProfile(int& OutMajorVersion, int& OutMinorVersion)
{
	// Always initialize GL context with version 4.3, it's the only GL desktop version we support now
	OutMajorVersion = 4;
	OutMinorVersion = 3;
}

/**
 * Enable/Disable debug context from the commandline
 */
static bool PlatformOpenGLDebugCtx()
{
#if _DEBUG
	return true;
#else
	return false;
#endif
}


/**
 * Create a core profile OpenGL context.
 */
static void PlatformCreateOpenGLContextCore(FPlatformOpenGLContext* OutContext, int MajorVersion, int MinorVersion, HGLRC InParentContext)
{
	assert(wglCreateContextAttribsARB);
	assert(OutContext);
	assert(OutContext->DeviceContext);

	OutContext->SyncInterval = -1;	// invalid value to enforce setup on first buffer swap
	OutContext->ViewportFramebuffer = 0;

	int DebugFlag = 0;

	if (PlatformOpenGLDebugCtx())
	{
		DebugFlag = WGL_CONTEXT_DEBUG_BIT_ARB;
	}

#if !EMULATE_ES31
	int AttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, MajorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB, MinorVersion,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB | DebugFlag,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};
#else
	int AttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, DebugFlag,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_ES2_PROFILE_BIT_EXT,
		0
	};
#endif

	OutContext->OpenGLContext = wglCreateContextAttribsARB(OutContext->DeviceContext, InParentContext, AttribList);
}

void PlatformReleaseOpenGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context);

extern void OnQueryInvalidation(void);

/** Platform specific OpenGL device. */
struct FPlatformOpenGLDevice : public RHIDeviceContext
{
	FPlatformOpenGLContext	SharedContext;
	FPlatformOpenGLContext	RenderingContext;
	std::vector<FPlatformOpenGLContext*>	ViewportContexts;
	bool					TargetDirty;

	/** Guards against operating on viewport contexts from more than one thread at the same time. */
	FCriticalSection* ContextUsageGuard;

	FPlatformOpenGLDevice()
		: TargetDirty(true)
	{
		extern void InitDebugContext();
		ContextUsageGuard = new FCriticalSection;

		int MajorVersion = 0;
		int MinorVersion = 0;
		GetOpenGLVersionForCoreProfile(MajorVersion, MinorVersion);

		// Need to call this before we set the debug callback, otherwise if we're not running under RD, the debug extension will assert (invalid enum)
		GRunningUnderRenderDoc = glIsEnabled(GL_DEBUG_TOOL_EXT) != GL_FALSE;

		PlatformCreateDummyGLWindow(&SharedContext);
		PlatformCreateOpenGLContextCore(&SharedContext, MajorVersion, MinorVersion, NULL);
		assert(SharedContext.OpenGLContext);
		{
			FScopeContext ScopeContext(&SharedContext);
			InitDebugContext();
			glGenVertexArrays(1, &SharedContext.VertexArrayObject);
			glBindVertexArray(SharedContext.VertexArrayObject);
			InitDefaultGLContextState();
			glGenFramebuffers(1, &SharedContext.ViewportFramebuffer);
		}

		PlatformCreateDummyGLWindow(&RenderingContext);
		PlatformCreateOpenGLContextCore(&RenderingContext, MajorVersion, MinorVersion, SharedContext.OpenGLContext);
		assert(RenderingContext.OpenGLContext);
		{
			FScopeContext ScopeContext(&RenderingContext);
			InitDebugContext();
			glGenVertexArrays(1, &RenderingContext.VertexArrayObject);
			glBindVertexArray(RenderingContext.VertexArrayObject);
			InitDefaultGLContextState();
			glGenFramebuffers(1, &RenderingContext.ViewportFramebuffer);
		}

		ContextMakeCurrent(SharedContext.DeviceContext, SharedContext.OpenGLContext);
	}

	~FPlatformOpenGLDevice()
	{
		assert(ViewportContexts.size() == 0);

		ContextMakeCurrent(NULL, NULL);

		OnQueryInvalidation();
		PlatformReleaseOpenGLContext(this, &RenderingContext);
		PlatformReleaseOpenGLContext(this, &SharedContext);

		delete ContextUsageGuard;
	}
};

FPlatformOpenGLDevice* PlatformCreateOpenGLDevice()
{
	return new FPlatformOpenGLDevice;
}

bool PlatformCanEnableGPUCapture()
{
	return GRunningUnderRenderDoc;
}

void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice* Device)
{
	delete Device;
}

/**
 * Create an OpenGL context.
 */
FPlatformOpenGLContext* PlatformCreateOpenGLContext(FPlatformOpenGLDevice* Device, void* InWindowHandle)
{
	assert(InWindowHandle);

	Device->TargetDirty = true;

	FPlatformOpenGLContext* Context = new FPlatformOpenGLContext;
	Context->WindowHandle = (HWND)InWindowHandle;
	Context->bReleaseWindowOnDestroy = false;
	Context->DeviceContext = GetDC(Context->WindowHandle);
	assert(Context->DeviceContext);
	PlatformInitPixelFormatForDevice(Context->DeviceContext, false);

	int MajorVersion = 0;
	int MinorVersion = 0;
	GetOpenGLVersionForCoreProfile(MajorVersion, MinorVersion);

	PlatformCreateOpenGLContextCore(Context, MajorVersion, MinorVersion, Device->SharedContext.OpenGLContext);
	assert(Context->OpenGLContext);
	{
		FScopeContext Scope(Context);
		InitDefaultGLContextState();
		glGenFramebuffers(1, &Context->ViewportFramebuffer);
	}

	Device->ViewportContexts.push_back(Context);
	return Context;
}

/**
 * Release an OpenGL context.
 */
void PlatformReleaseOpenGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context)
{
	assert(Context && Context->OpenGLContext);

	Device->ViewportContexts.RemoveSingle(Context);
	Device->TargetDirty = true;

	bool bActiveContextWillBeReleased = false;
	bool bSharedDC = false;

	{
		FScopeLock ScopeLock(Device->ContextUsageGuard);
		{
			FScopeContext ScopeContext(Context);

			bActiveContextWillBeReleased = ScopeContext.ContextWasAlreadyActive();
			bSharedDC = ScopeContext.ContextsShareSameDC();

			DeleteQueriesForCurrentContext(Context->OpenGLContext);
			glBindVertexArray(0);
			glDeleteVertexArrays(1, &Context->VertexArrayObject);

			if (Context->ViewportFramebuffer)
			{
				glDeleteFramebuffers(1, &Context->ViewportFramebuffer);	// this can be done from any context shared with ours, as long as it's not nil.
				Context->ViewportFramebuffer = 0;
			}
		}

		wglDeleteContext(Context->OpenGLContext);
		Context->OpenGLContext = NULL;
	}

	assert(Context->DeviceContext);

	if (bActiveContextWillBeReleased)
	{
		wglMakeCurrent(NULL, NULL);
	}
	ReleaseDC(Context->WindowHandle, Context->DeviceContext);
	Context->DeviceContext = NULL;

	if (PlatformOpenGLCurrentContext(Device) == CONTEXT_Rendering && bSharedDC)
	{
		// The rendering context has been made current using the DC of the now destroyed context. Since this DC has been released the current context will be invalid.
		// To properly set the rendering context we must make current here with it's own DC.
		ContextMakeCurrent(Device->RenderingContext.DeviceContext, Device->RenderingContext.OpenGLContext);
	}

	assert(Context->WindowHandle);

	if (Context->bReleaseWindowOnDestroy)
	{
		DestroyWindow(Context->WindowHandle);
	}
	Context->WindowHandle = NULL;
}

/**
 * Destroy an OpenGL context.
 */
void PlatformDestroyOpenGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context)
{
	PlatformReleaseOpenGLContext(Device, Context);
	delete Context;
}

/**
 * Main function for transferring data to on-screen buffers.
 * On Windows it temporarily switches OpenGL context, on Mac only context's output view.
 */
bool PlatformBlitToViewport(FPlatformOpenGLDevice* Device, const OpenGLViewport& Viewport, uint32_t BackbufferSizeX, uint32_t BackbufferSizeY, bool bPresent, bool bLockToVsync)
{
	int32_t SyncInterval = RHIGetSyncInterval();

	FPlatformOpenGLContext* const Context = Viewport.GetGLContext();

	assert(Context && Context->DeviceContext);

	if (FOpenGL::IsAndroidGLESCompatibilityModeEnabled())
	{
		glDisable(GL_FRAMEBUFFER_SRGB);

		int32_t RealSyncInterval = bLockToVsync ? SyncInterval : 0;
		if (wglSwapIntervalEXT_ProcAddress && Context->SyncInterval != RealSyncInterval)
		{
			wglSwapIntervalEXT_ProcAddress(RealSyncInterval);
			Context->SyncInterval = RealSyncInterval;
		}

		::SwapBuffers(Context->DeviceContext);
		return true;
	}

	FScopeLock ScopeLock(Device->ContextUsageGuard);
	{
		FPlatformOpenGLContext TempContext = *Context;
		if (Device->ViewportContexts.size() == 1)
		{
			TempContext.OpenGLContext = Device->RenderingContext.OpenGLContext;
			TempContext.ViewportFramebuffer = Device->RenderingContext.ViewportFramebuffer;
		}
		FScopeContext ScopeContext(&TempContext);

		GLuint vfb = TempContext.ViewportFramebuffer;
		if (Viewport.GetCustomPresent())
		{
			Device->TargetDirty = false;
			glDisable(GL_FRAMEBUFFER_SRGB);
			bool bShouldPresent = Viewport.GetCustomPresent()->Present(SyncInterval);
			glEnable(GL_FRAMEBUFFER_SRGB);
			if (!bShouldPresent)
			{
				return false;
			}
			else
			{
				Device->TargetDirty = true;
			}
		}

		if (Device->ViewportContexts.size() == 1 && Device->TargetDirty)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, TempContext.ViewportFramebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Context->BackBufferTarget, Context->BackBufferResource, 0);

			FOpenGL::CheckFrameBuffer();
			Device->TargetDirty = false;
		}

		glDisable(GL_FRAMEBUFFER_SRGB);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		FOpenGL::DrawBuffer(GL_BACK);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, TempContext.ViewportFramebuffer);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		glBlitFramebuffer(
			0, 0, BackbufferSizeX, BackbufferSizeY,
			0, BackbufferSizeY, BackbufferSizeX, 0,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);
		glEnable(GL_FRAMEBUFFER_SRGB);

		if (bPresent)
		{
			int32_t RealSyncInterval = bLockToVsync ? SyncInterval : 0;
			if (wglSwapIntervalEXT_ProcAddress && Context->SyncInterval != RealSyncInterval)
			{
				wglSwapIntervalEXT_ProcAddress(RealSyncInterval);
				Context->SyncInterval = RealSyncInterval;
			}

			::SwapBuffers(Context->DeviceContext);
		}
	}
	return true;
}

void PlatformFlushIfNeeded()
{
	glFinish();
}

void PlatformRebindResources(FPlatformOpenGLDevice* Device)
{
	// @todo: Figure out if we need to rebind frame & renderbuffers after switching contexts
}

FPlatformOpenGLContext* PlatformGetOpenGLRenderingContext(FPlatformOpenGLDevice* Device)
{
	return &Device->RenderingContext;
}

void PlatformRenderingContextSetup(FPlatformOpenGLDevice* Device)
{
	assert(Device && Device->RenderingContext.DeviceContext && Device->RenderingContext.OpenGLContext);

	if (GetCurrentContext())
	{
		glFlush();
	}
	if (Device->ViewportContexts.size() == 1)
	{
		// use the HDC of the window, to reduce context swap overhead
		ContextMakeCurrent(Device->ViewportContexts[0]->DeviceContext, Device->RenderingContext.OpenGLContext);
	}
	else
	{
		ContextMakeCurrent(Device->RenderingContext.DeviceContext, Device->RenderingContext.OpenGLContext);
	}
}

void PlatformSharedContextSetup(FPlatformOpenGLDevice* Device)
{
	assert(Device && Device->SharedContext.DeviceContext && Device->SharedContext.OpenGLContext);

	// no need to glFlush() on Windows, it does flush by itself before switching contexts
	ContextMakeCurrent(Device->SharedContext.DeviceContext, Device->SharedContext.OpenGLContext);

}

void PlatformNULLContextSetup()
{
	if (wglGetCurrentDC())
	{
		// no need to glFlush() on Windows, it does flush by itself before switching contexts
		ContextMakeCurrent(NULL, NULL);
	}
}

/**
 * Resize the GL context.
 */
void PlatformResizeGLContext(FPlatformOpenGLDevice* Device, FPlatformOpenGLContext* Context, uint32_t SizeX, uint32_t SizeY, bool bFullscreen, bool bWasFullscreen, GLenum BackBufferTarget, GLuint BackBufferResource)
{
	FScopeLock ScopeLock(Device->ContextUsageGuard);
	{
		uint32_t WindowStyle = WS_CAPTION | WS_SYSMENU;
		uint32_t WindowStyleEx = 0;
		HWND InsertAfter = HWND_NOTOPMOST;

		if (bFullscreen)
		{
			// Get the monitor info from the window handle.
			HMONITOR hMonitor = MonitorFromWindow(Context->WindowHandle, MONITOR_DEFAULTTOPRIMARY);
			MONITORINFOEX MonitorInfo;
			memset(&MonitorInfo, 0, sizeof(MONITORINFOEX));
			MonitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(hMonitor, &MonitorInfo);

			DEVMODE Mode;
			Mode.dmSize = sizeof(DEVMODE);
			Mode.dmBitsPerPel = 32;
			Mode.dmPelsWidth = SizeX;
			Mode.dmPelsHeight = SizeY;
			Mode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Turn on fullscreen mode for the current monitor
			ChangeDisplaySettingsEx(MonitorInfo.szDevice, &Mode, NULL, CDS_FULLSCREEN, NULL);

			WindowStyle = WS_POPUP;
			WindowStyleEx = WS_EX_APPWINDOW | WS_EX_TOPMOST;
			InsertAfter = HWND_TOPMOST;
		}
		else if (bWasFullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		Device->TargetDirty = true;
		Context->BackBufferResource = BackBufferResource;
		Context->BackBufferTarget = BackBufferTarget;

		//SetWindowLong(Context->WindowHandle, GWL_STYLE, WindowStyle);
		//SetWindowLong(Context->WindowHandle, GWL_EXSTYLE, WindowStyleEx);

		if (!FOpenGL::IsAndroidGLESCompatibilityModeEnabled())
		{
			FScopeContext ScopeContext(Context);

			glBindFramebuffer(GL_FRAMEBUFFER, Context->ViewportFramebuffer);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, BackBufferTarget, BackBufferResource, 0);
#if UE_BUILD_DEBUG
			glReadBuffer(GL_COLOR_ATTACHMENT0);
			FOpenGL::DrawBuffer(GL_COLOR_ATTACHMENT0);
#endif
			FOpenGL::CheckFrameBuffer();

			glViewport(0, 0, SizeX, SizeY);
			static GLfloat ZeroColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			glClearBufferfv(GL_COLOR, 0, ZeroColor);
		}
	}
}

void PlatformGetSupportedResolution(uint32_t& Width, uint32_t& Height)
{
	uint32_t InitializedMode = false;
	uint32_t BestWidth = 0;
	uint32_t BestHeight = 0;
	uint32_t ModeIndex = 0;
	DEVMODE DisplayMode;
	//FMemory::Memzero(&DisplayMode, sizeof(DEVMODE));
	memset(&DisplayMode, 0, sizeof(DEVMODE));

	while (EnumDisplaySettings(NULL, ModeIndex++, &DisplayMode))
	{
		bool IsEqualOrBetterWidth = FMath::Abs((int32_t)DisplayMode.dmPelsWidth - (int32_t)Width) <= FMath::Abs((int32_t)BestWidth - (int32_t)Width);
		bool IsEqualOrBetterHeight = FMath::Abs((int32_t)DisplayMode.dmPelsHeight - (int32_t)Height) <= FMath::Abs((int32_t)BestHeight - (int32_t)Height);
		if (!InitializedMode || (IsEqualOrBetterWidth && IsEqualOrBetterHeight))
		{
			BestWidth = DisplayMode.dmPelsWidth;
			BestHeight = DisplayMode.dmPelsHeight;
			InitializedMode = true;
		}
	}
	assert(InitializedMode);
	Width = BestWidth;
	Height = BestHeight;
}

bool PlatformGetAvailableResolutions(std::vector<RHIScreenResolution>& Resolutions, bool bIgnoreRefreshRate)
{
	int32_t MinAllowableResolutionX = 0;
	int32_t MinAllowableResolutionY = 0;
	int32_t MaxAllowableResolutionX = 10480;
	int32_t MaxAllowableResolutionY = 10480;
	int32_t MinAllowableRefreshRate = 0;
	int32_t MaxAllowableRefreshRate = 10480;

	if (MaxAllowableResolutionX == 0) //-V547
	{
		MaxAllowableResolutionX = 10480;
	}
	if (MaxAllowableResolutionY == 0) //-V547
	{
		MaxAllowableResolutionY = 10480;
	}
	if (MaxAllowableRefreshRate == 0) //-V547
	{
		MaxAllowableRefreshRate = 10480;
	}

	uint32_t ModeIndex = 0;
	DEVMODE DisplayMode;
	//FMemory::Memzero(&DisplayMode, sizeof(DEVMODE));
	memset(&DisplayMode, 0, sizeof(DEVMODE));

	while (EnumDisplaySettings(NULL, ModeIndex++, &DisplayMode))
	{
		if (((int32_t)DisplayMode.dmPelsWidth >= MinAllowableResolutionX) &&
			((int32_t)DisplayMode.dmPelsWidth <= MaxAllowableResolutionX) &&
			((int32_t)DisplayMode.dmPelsHeight >= MinAllowableResolutionY) &&
			((int32_t)DisplayMode.dmPelsHeight <= MaxAllowableResolutionY)
			)
		{
			bool bAddIt = true;
			if (bIgnoreRefreshRate == false)
			{
				if (((int32_t)DisplayMode.dmDisplayFrequency < MinAllowableRefreshRate) ||
					((int32_t)DisplayMode.dmDisplayFrequency > MaxAllowableRefreshRate)
					)
				{
					continue;
				}
			}
			else
			{
				// See if it is in the list already
				for (int32_t CheckIndex = 0; CheckIndex < Resolutions.size(); CheckIndex++)
				{
					RHIScreenResolution& CheckResolution = Resolutions[CheckIndex];
					if ((CheckResolution.Width == DisplayMode.dmPelsWidth) &&
						(CheckResolution.Height == DisplayMode.dmPelsHeight))
					{
						// Already in the list...
						bAddIt = false;
						break;
					}
				}
			}

			if (bAddIt)
			{
				// Add the mode to the list
				int32_t Temp2Index = Resolutions.AddZeroed();

				RHIScreenResolution& ScreenResolution = Resolutions[Temp2Index];

				ScreenResolution.Width = DisplayMode.dmPelsWidth;
				ScreenResolution.Height = DisplayMode.dmPelsHeight;
				ScreenResolution.RefreshRate = DisplayMode.dmDisplayFrequency;
			}
		}
	}

	return true;
}

void PlatformRestoreDesktopDisplayMode()
{
	ChangeDisplaySettings(NULL, 0);
}

bool PlatformInitOpenGL()
{
	static bool bInitialized = false;
	static bool bOpenGLSupported = false;
	if (!bInitialized)
	{
		// Disable warning C4191: 'type cast' : unsafe conversion from 'PROC' to 'XXX' while getting GL entry points.
#pragma warning(push)
#pragma warning(disable:4191)

		// Create a dummy context so that wglCreateContextAttribsARB can be initialized.
		FPlatformOpenGLContext DummyContext;
		PlatformCreateDummyGLWindow(&DummyContext);
		DummyContext.OpenGLContext = wglCreateContext(DummyContext.DeviceContext);
		assert(DummyContext.OpenGLContext);
		ContextMakeCurrent(DummyContext.DeviceContext, DummyContext.OpenGLContext);
		wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
		if (wglCreateContextAttribsARB)
		{
			int MajorVersion = 0;
			int MinorVersion = 0;

			ContextMakeCurrent(NULL, NULL);
			wglDeleteContext(DummyContext.OpenGLContext);
			GetOpenGLVersionForCoreProfile(MajorVersion, MinorVersion);
			PlatformCreateOpenGLContextCore(&DummyContext, MajorVersion, MinorVersion, NULL);
			if (DummyContext.OpenGLContext)
			{
				bOpenGLSupported = true;
				ContextMakeCurrent(DummyContext.DeviceContext, DummyContext.OpenGLContext);
			}
			else
			{
				//UE_LOG(LogRHI, Error, TEXT("OpenGL %d.%d not supported by driver"), MajorVersion, MinorVersion);
			}
		}

		if (bOpenGLSupported)
		{
			// Retrieve the OpenGL DLL.
			void* OpenGLDLL = FPlatformProc::GetDllHandle("opengl32.dll");
			if (!OpenGLDLL)
			{
				//UE_LOG(LogRHI, Fatal, TEXT("Couldn't load opengl32.dll"));
			}

			// Initialize entry points required by Unreal from opengl32.dll
#define GET_GL_ENTRYPOINTS_DLL(Type,Func) Func = (Type)FPlatformProc::GetDllExport(OpenGLDLL,#Func);
			ENUM_GL_ENTRYPOINTS_DLL(GET_GL_ENTRYPOINTS_DLL);
#undef GET_GL_ENTRYPOINTS_DLL

			// Release the OpenGL DLL.
			FPlatformProc::FreeDllHandle(OpenGLDLL);

			// Initialize all entry points required by Unreal.
#define GET_GL_ENTRYPOINTS(Type,Func) Func = (Type)wglGetProcAddress(#Func);
			ENUM_GL_ENTRYPOINTS(GET_GL_ENTRYPOINTS);
			ENUM_GL_ENTRYPOINTS_OPTIONAL(GET_GL_ENTRYPOINTS);
#undef GET_GL_ENTRYPOINTS

			// Restore warning C4191.
#pragma warning(pop)

// Check that all of the entry points have been initialized.
			bool bFoundAllEntryPoints = true;
#define CHECK_GL_ENTRYPOINTS(Type,Func) if (Func == NULL) { bFoundAllEntryPoints = false; printf("Failed to find entry point for %s", #Func); }
			ENUM_GL_ENTRYPOINTS_DLL(CHECK_GL_ENTRYPOINTS);
			ENUM_GL_ENTRYPOINTS(CHECK_GL_ENTRYPOINTS);
#undef CHECK_GL_ENTRYPOINTS
			assert(bFoundAllEntryPoints, ("Failed to find all OpenGL entry points."));
		}

		// The dummy context can now be released.
		if (DummyContext.OpenGLContext)
		{
			ContextMakeCurrent(NULL, NULL);
			wglDeleteContext(DummyContext.OpenGLContext);
		}
		ReleaseDC(DummyContext.WindowHandle, DummyContext.DeviceContext);
		assert(DummyContext.bReleaseWindowOnDestroy);
		DestroyWindow(DummyContext.WindowHandle);

	}
	return bOpenGLSupported;
}

bool PlatformOpenGLContextValid()
{
	return(GetCurrentContext() != NULL);
}

int32_t PlatformGlGetError()
{
	return glGetError();
}

EOpenGLCurrentContext PlatformOpenGLCurrentContext(FPlatformOpenGLDevice* Device)
{
	HGLRC Context = GetCurrentContext();

	if (Context == Device->RenderingContext.OpenGLContext)	// most common case
	{
		return CONTEXT_Rendering;
	}
	else if (Context == Device->SharedContext.OpenGLContext)
	{
		return CONTEXT_Shared;
	}
	else if (Context)
	{
		return CONTEXT_Other;
	}
	else
	{
		return CONTEXT_Invalid;
	}
}

void* PlatformOpenGLCurrentContextHandle(FPlatformOpenGLDevice* Device)
{
	return GetCurrentContext();
}

void PlatformGetBackbufferDimensions(uint32_t& OutWidth, uint32_t& OutHeight)
{
	OutWidth = OutHeight = 0;
	HDC DeviceContext = wglGetCurrentDC();
	if (DeviceContext)
	{
		OutWidth = GetDeviceCaps(DeviceContext, HORZRES);
		OutHeight = GetDeviceCaps(DeviceContext, VERTRES);
	}
}

// =============================================================

struct FOpenGLReleasedQuery
{
	HGLRC	Context;
	GLuint	Query;
};

static std::vector<FOpenGLReleasedQuery>	ReleasedQueries;
static FCriticalSection* ReleasedQueriesGuard;

void PlatformGetNewRenderQuery(GLuint* OutQuery, uint64_t* OutQueryContext)
{
	if (!ReleasedQueriesGuard)
	{
		ReleasedQueriesGuard = new FCriticalSection;
	}

	{
		FScopeLock Lock(ReleasedQueriesGuard);

#ifdef UE_BUILD_DEBUG
		assert(OutQuery && OutQueryContext);
#endif

		HGLRC Context = GetCurrentContext();
		assert(Context);

		GLuint NewQuery = 0;

		// Check for possible query reuse
		const int32_t ArraySize = ReleasedQueries.size();
		for (int32_t Index = 0; Index < ArraySize; ++Index)
		{
			if (ReleasedQueries[Index].Context == Context)
			{
				NewQuery = ReleasedQueries[Index].Query;
				ReleasedQueries.RemoveAtSwap(Index);
				break;
			}
		}

		if (!NewQuery)
		{
			FOpenGL::GenQueries(1, &NewQuery);
		}

		*OutQuery = NewQuery;
		*OutQueryContext = (uint64_t)Context;
	}
}

void PlatformReleaseRenderQuery(GLuint Query, uint64_t QueryContext)
{
	HGLRC Context = GetCurrentContext();
	if ((uint64_t)Context == QueryContext)
	{
		FOpenGL::DeleteQueries(1, &Query);
	}
	else
	{
		FScopeLock Lock(ReleasedQueriesGuard);
#ifdef UE_BUILD_DEBUG
		assert(Query && QueryContext && ReleasedQueriesGuard);
#endif
		FOpenGLReleasedQuery ReleasedQuery;
		ReleasedQuery.Context = (HGLRC)QueryContext;
		ReleasedQuery.Query = Query;
		ReleasedQueries.Add(ReleasedQuery);
	}
}

void DeleteQueriesForCurrentContext(HGLRC Context)
{
	if (!ReleasedQueriesGuard)
	{
		ReleasedQueriesGuard = new FCriticalSection;
	}

	{
		FScopeLock Lock(ReleasedQueriesGuard);
		for (int32_t Index = 0; Index < ReleasedQueries.size(); ++Index)
		{
			if (ReleasedQueries[Index].Context == Context)
			{
				FOpenGL::DeleteQueries(1, &ReleasedQueries[Index].Query);
				ReleasedQueries.RemoveAtSwap(Index);
				--Index;
			}
		}
	}
}

bool PlatformContextIsCurrent(uint64_t QueryContext)
{
	return (uint64_t)GetCurrentContext() == QueryContext;
}
//
//FOpenGLTexture* PlatformCreateBuiltinBackBuffer(OpenGLDynamicRHI* OpenGLRHI, uint32_t SizeX, uint32_t SizeY)
//{
//	if (FOpenGL::IsAndroidGLESCompatibilityModeEnabled())
//	{
//		const FRHITextureCreateDesc Desc =
//			FRHITextureCreateDesc::Create2D(TEXT("PlatformCreateBuiltinBackBuffer"), SizeX, SizeY, PF_B8G8R8A8)
//			.SetClearValue(FClearValueBinding::Transparent)
//			.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::Presentable | ETextureCreateFlags::ResolveTargetable)
//			.DetermineInititialState();
//
//		return new FOpenGLTexture(Desc);
//	}
//
//	return nullptr;
//}

void* PlatformGetWindow(FPlatformOpenGLContext* Context, void** AddParam)
{
	assert(Context && Context->WindowHandle);

	return (void*)&Context->WindowHandle;
}

}