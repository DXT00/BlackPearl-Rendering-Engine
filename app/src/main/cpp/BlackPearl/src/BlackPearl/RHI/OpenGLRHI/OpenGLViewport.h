#pragma once
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
namespace BlackPearl {
	class OpenGLViewport : public RHIViewport
	{

	public:

		FOpenGLViewport(class FOpenGLDynamicRHI* InOpenGLRHI, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, EPixelFormat PreferredPixelFormat);
		~FOpenGLViewport();

		void Resize(uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen);

		// Accessors.
		//FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
		Texture* GetBackBuffer() const { return BackBuffer; }
		bool IsFullscreen(void) const { return bIsFullscreen; }

		virtual void WaitForFrameEventCompletion() ;
		virtual void IssueFrameEvent() ;

		virtual void* GetNativeWindow(void** AddParam) const ;

		struct FPlatformOpenGLContext* GetGLContext() const { return OpenGLContext; }
		FOpenGLDynamicRHI* GetOpenGLRHI() const { return OpenGLRHI; }

		virtual void SetCustomPresent(FRHICustomPresent* InCustomPresent) 
		{
			CustomPresent = InCustomPresent;
		}
		FRHICustomPresent* GetCustomPresent() const { return CustomPresent.GetReference(); }
	private:

		friend class FOpenGLDynamicRHI;

		FOpenGLDynamicRHI* OpenGLRHI;
		struct FPlatformOpenGLContext* OpenGLContext;
		uint32_t SizeX;
		uint32_t SizeY;
		bool bIsFullscreen;
		EPixelFormat PixelFormat;
		bool bIsValid;
		Texture* BackBuffer;
		TUniquePtr<FOpenGLEventQuery> FrameSyncEvent;
		FCustomPresentRHIRef CustomPresent;


	};

}

