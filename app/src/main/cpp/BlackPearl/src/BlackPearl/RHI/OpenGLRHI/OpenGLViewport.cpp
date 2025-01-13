#include "pch.h"
#include "OpenGLViewport.h"
namespace BlackPearl {



	BlackPearl::OpenGLViewport::OpenGLViewport(OpenGLDynamicRHI* InOpenGLRHI, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, EPixelFormat PreferredPixelFormat)
	{
	}

	OpenGLViewport::~OpenGLViewport()
	{
	}

	void OpenGLViewport::Resize(uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen)
	{
	}

	void OpenGLViewport::WaitForFrameEventCompletion()
	{
	}

	void OpenGLViewport::IssueFrameEvent()
	{
	}

	void* OpenGLViewport::GetNativeWindow(void** AddParam) const
	{
		return nullptr;
	}

}