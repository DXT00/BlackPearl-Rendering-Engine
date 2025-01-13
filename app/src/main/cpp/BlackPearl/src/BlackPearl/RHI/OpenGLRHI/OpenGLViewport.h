#pragma once
#include <algorithm>
#include "BlackPearl/RHI/RHIDefinitions.h"
namespace BlackPearl {
	class OpenGLEventQuery;
	class Texture;
	class OpenGLViewport : public RHIViewport
	{

	public:

		OpenGLViewport(class OpenGLDynamicRHI* InOpenGLRHI, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, EPixelFormat PreferredPixelFormat);
		~OpenGLViewport();

		void Resize(uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen);

		// Accessors.
		//FIntPoint GetSizeXY() const { return FIntPoint(SizeX, SizeY); }
		Texture* GetBackBuffer() const { return BackBuffer; }
		bool IsFullscreen(void) const { return bIsFullscreen; }

		virtual void WaitForFrameEventCompletion() ;
		virtual void IssueFrameEvent() ;

		virtual void* GetNativeWindow(void** AddParam) const ;

		struct FPlatformOpenGLContext* GetGLContext() const { return OpenGLContext; }
		OpenGLDynamicRHI* GetOpenGLRHI() const { return OpenGLRHI; }

		/*virtual void SetCustomPresent(FRHICustomPresent* InCustomPresent) 
		{
			CustomPresent = InCustomPresent;
		}
		FRHICustomPresent* GetCustomPresent() const { return CustomPresent.GetReference(); }*/
	private:

		friend class OpenGLDynamicRHI;

		OpenGLDynamicRHI* OpenGLRHI;
		struct FPlatformOpenGLContext* OpenGLContext;
		uint32_t SizeX;
		uint32_t SizeY;
		bool bIsFullscreen;
		Format PixelFormat;
		bool bIsValid;
		Texture* BackBuffer;
		std::unique_ptr<OpenGLEventQuery>FrameSyncEvent;
		//TUniquePtr<FOpenGLEventQuery> 
		//FCustomPresentRHIRef CustomPresent;


	};

}

