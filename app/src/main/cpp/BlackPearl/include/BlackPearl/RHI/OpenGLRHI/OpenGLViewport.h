#pragma once
#include <algorithm>
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "OpenGLQuery.h"
#include "Config.h"
namespace BlackPearl {
	class EventQuery;
	class Texture;
	class Device;
	class OpenGLViewport : public RHIViewport
	{

	public:

		OpenGLViewport(class OpenGLDynamicRHI* InOpenGLRHI, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, Format PreferredPixelFormat);
		OpenGLViewport(Device* device, void* InWindowHandle,uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, Format PreferredPixelFormat);

		~OpenGLViewport();

		void Resize(uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen);

		// Accessors.
		//FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
		//GLES �� eglSwapBuffers �������ģ��޷�ֱ��ʵ�ֶ�֡���С�
		TextureHandle GetBackBuffer(int frameIndex = 0) const { return m_DefaultBackBuffers[frameIndex]; }
		FramebufferHandle GetFrameBuffer(int frameIndex = 0) const { return m_DefaultFramebuffers[frameIndex]; }

		bool IsFullscreen(void) const { return m_bIsFullscreen; }

		virtual void WaitForFrameEventCompletion() ;
		virtual void IssueFrameEvent() ;

		virtual void* GetNativeWindow(void** AddParam) const ;

		struct FPlatformOpenGLContext* GetGLContext() const { return OpenGLContext; }
	//	OpenGLDynamicRHI* GetOpenGLRHI() const { return OpenGLRHI; }

		/*virtual void SetCustomPresent(FRHICustomPresent* InCustomPresent) 
		{
			CustomPresent = InCustomPresent;
		}
		FRHICustomPresent* GetCustomPresent() const { return CustomPresent.GetReference(); }*/
	private:


		struct FPlatformOpenGLContext* OpenGLContext = nullptr;
		uint32_t m_SizeX = 0;
		uint32_t m_SizeY = 0;
		bool m_bIsFullscreen;
		Format m_PixelFormat;
		bool bIsValid;
//		Texture* BackBuffer;
		std::unique_ptr<EventQuery> m_FrameSyncEvent;

		Device* m_Device = nullptr;
		//default gl front/ back buffer, empty texture, just adapt the rhi
		FramebufferHandle m_DefaultFramebuffers[GL_BACKBUFFER_CNT];
		TextureHandle m_DefaultBackBuffers[GL_BACKBUFFER_CNT];
		//TUniquePtr<FOpenGLEventQuery> 
		//FCustomPresentRHIRef CustomPresent;


	};

}

