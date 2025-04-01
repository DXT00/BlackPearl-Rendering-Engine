#include "pch.h"
#include "OpenGLViewport.h"
#include "OpenGLDevice.h"
#include "OpenGLUtil.h"
#include "OpenGLContext.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {



	OpenGLViewport::OpenGLViewport(OpenGLDynamicRHI* InOpenGLRHI, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, Format PreferredPixelFormat)
	{
	}

	OpenGLViewport::OpenGLViewport(Device* device, void* InWindowHandle, uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen, Format PreferredPixelFormat)
	{
		m_Device = device;
		m_PixelFormat = PreferredPixelFormat;
		//for (size_t i = 0; i < GL_BACKBUFFER_CNT; i++)
		//{
		//	m_DefaultBackBuffers[i] = backbuffers[i];
		//}
		//for (uint32_t index = 0; index < GL_BACKBUFFER_CNT; index++)
		//{
		//	FramebufferDesc fboDesc;
		//	fboDesc.addColorAttachment(m_DefaultBackBuffers[index]);
		//	m_DefaultFramebuffers[index] = device->createFramebuffer(fboDesc);
		//}

		device->Viewports.push_back(this);

		GE_ASSERT(PlatformOpenGLCurrentContext(device->m_Context->PlatformDevice) == CONTEXT_Shared)

		OpenGLContext = PlatformCreateOpenGLContext(device->m_Context->PlatformDevice, InWindowHandle);
		Resize(InSizeX, InSizeY, bInIsFullscreen);

		GE_ASSERT(PlatformOpenGLCurrentContext(device->m_Context->PlatformDevice) == CONTEXT_Shared);

		/*ENQUEUE_RENDER_COMMAND(CreateFrameSyncEvent)([this](FRHICommandListImmediate& RHICmdList)
			{
				RunOnGLRenderContextThread([this]()
					{
						FrameSyncEvent = MakeUnique<FOpenGLEventQuery>();
					});
			});*/
	}

	OpenGLViewport::~OpenGLViewport()
	{
	}

	void OpenGLViewport::Resize(uint32_t InSizeX, uint32_t InSizeY, bool bInIsFullscreen)
	{


		if ((InSizeX == m_SizeX) && (InSizeY == m_SizeY) && (bInIsFullscreen == m_bIsFullscreen))
		{
			return;
		}

		m_SizeX = InSizeX;
		m_SizeY = InSizeY;
		bool bWasFullscreen = m_bIsFullscreen;
		m_bIsFullscreen = bInIsFullscreen;


		for (size_t i = 0; i < GL_BACKBUFFER_CNT; i++)
		{
			TextureDesc textureDesc;
			textureDesc.width = InSizeX;
			textureDesc.height = InSizeY;
			textureDesc.format = m_PixelFormat;
			textureDesc.debugName = "GL backbuffer image " + std::to_string(i);
			textureDesc.initialState = ResourceStates::Present;
			textureDesc.keepInitialState = true;
			textureDesc.isRenderTarget = true;

			m_DefaultBackBuffers[i] = m_Device->createTexture(textureDesc);
		}
		for (uint32_t index = 0; index < GL_BACKBUFFER_CNT; index++)
		{
			FramebufferDesc fboDesc;
			fboDesc.addColorAttachment(m_DefaultBackBuffers[index]);
			m_DefaultFramebuffers[index] = m_Device->createFramebuffer(fboDesc);
		}
		for (uint32_t index = 0; index < GL_BACKBUFFER_CNT; index++) {

			PlatformResizeGLContext(m_Device->m_Context->PlatformDevice, OpenGLContext, InSizeX, InSizeY, bInIsFullscreen, bWasFullscreen,OpenGLUtil::convertTextureDimension(m_DefaultBackBuffers[index]->getDesc().dimension), static_cast<Texture*>(m_DefaultBackBuffers[index].Get())->GetRendererID());
		}
		//ENQUEUE_RENDER_COMMAND(ResizeViewport)([this, InSizeX, InSizeY, bInIsFullscreen, bWasFullscreen](FRHICommandListImmediate& RHICmdList)
			//{
			/*	if (IsValidRef(CustomPresent))
				{
					CustomPresent->OnBackBufferResize();
				}*/

				//BackBuffer.SafeRelease();	// when the rest of the engine releases it, its framebuffers will be released too (those the engine knows about)

			//	BackBuffer = PlatformCreateBuiltinBackBuffer(OpenGLRHI, InSizeX, InSizeY);
			//	if (!BackBuffer)
			//	{
			//		const FRHITextureCreateDesc Desc =
			//			FRHITextureCreateDesc::Create2D(TEXT("FOpenGLViewport"), InSizeX, InSizeY, PixelFormat)
			//			.SetClearValue(FClearValueBinding::Transparent)
			//			.SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::ResolveTargetable)
			//			.DetermineInititialState();

			//		BackBuffer = new FOpenGLTexture(Desc);
			//	}

			//	//RHICmdList.EnqueueLambda([this, InSizeX, InSizeY, bInIsFullscreen, bWasFullscreen](FRHICommandListImmediate&)
			//		{
			//			PlatformResizeGLContext(OpenGLRHI->PlatformDevice, OpenGLContext, InSizeX, InSizeY, bInIsFullscreen, bWasFullscreen, BackBuffer->Target, BackBuffer->GetResource());
			//		});
			////});
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