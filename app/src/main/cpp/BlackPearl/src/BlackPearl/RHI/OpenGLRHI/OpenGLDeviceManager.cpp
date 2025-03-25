#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "OpenGLDeviceManager.h"
#include "OpenGLDevice.h"
#include "OpenGLDriver/OpenGLDrvPrivate.h"
namespace BlackPearl {

	/**
     * Initialize RHI capabilities for the current OpenGL context.
     */

    const char* OpenGLDeviceManager::GetRendererString() const
    {
        return nullptr;
    }
    IDevice* OpenGLDeviceManager::GetDevice() const
    {
            return m_NvrhiDevice;
        
    }
    bool OpenGLDeviceManager::CreateDeviceAndSwapChain()
    {
        // opengl context and driver init
        PlatformInitOpenGL();

        m_NvrhiDevice = Device::createDevice();

        for (size_t i = 0; i < GL_BACKBUFFER_CNT; i++)
        {
            TextureDesc textureDesc;
            textureDesc.width = m_DeviceParams.backBufferWidth;
            textureDesc.height = m_DeviceParams.backBufferHeight;
            textureDesc.format = m_DeviceParams.swapChainFormat;
            textureDesc.debugName = "GL backbuffer image";
            textureDesc.initialState = ResourceStates::Present;
            textureDesc.keepInitialState = true;
            textureDesc.isRenderTarget = true;

            m_DefaultBackBuffers[i] = GetDevice()->createTexture(textureDesc);
        }
		// Disable SingleRHIThreadStall for GL occlusion queiresn, which should be set for D3D11 only. Enabling it causes RT->RHIT deadlock
		/*{
			auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.Occlusion.SingleRHIThreadStall"));
			if (CVar)
			{
				CVar->Set(0);
			}
		}*/

//		PrivateOpenGLDevicePtr = this;
//		GlobalUniformBuffers.AddZeroed(FUniformBufferStaticSlotRegistry::Get().GetSlotCount());
//
//#if RHI_NEW_GPU_PROFILER == 0
//		GPUProfilingData.Emplace();
//#endif




        return true;
    }


    void OpenGLDeviceManager::DestroyDeviceAndSwapChain()
    {

    }

    void OpenGLDeviceManager::ResizeSwapChain()
    {

    }

    void OpenGLDeviceManager::BackBufferResizedInner()
    {
       // uint32_t backBufferCount = GetBackBufferCount();
        //m_DefaultFramebuffers.resize(backBufferCount);
        for (uint32_t index = 0; index < GL_BACKBUFFER_CNT; index++)
        {
            FramebufferDesc fboDesc;
            fboDesc.addColorAttachment(m_DefaultBackBuffers[index]);
            m_DefaultFramebuffers[index] = GetDevice()->createFramebuffer(fboDesc);
        }
    }

    IFramebuffer* OpenGLDeviceManager::GetCurrentFramebuffer()
    {
        return m_DefaultFramebuffers[m_BackBufferIndex];
    }

    void OpenGLDeviceManager::BeginFrame()
    {
    }

    void OpenGLDeviceManager::Present()
    {
        m_BackBufferIndex = (m_BackBufferIndex + 1) % GL_BACKBUFFER_CNT;
    }

    void OpenGLDeviceManager::_RHIViewportBeginDraw()
    {
    }

    void OpenGLDeviceManager::_RHIViewportEndDraw()
    {
    }

    //ITexture* OpenGLDeviceManager::GetCurrentBackBuffer()
    //{
    //    return nullptr;
    //}

    //ITexture* OpenGLDeviceManager::GetBackBuffer(uint32_t index)
    //{
    //    return nullptr;
    //}

    //uint32_t OpenGLDeviceManager::GetCurrentBackBufferIndex()
    //{
    //    return 0;
    //}

    //uint32_t OpenGLDeviceManager::GetBackBufferCount()
    //{
    //    return GLBackBufferCnt;
    //}

    DeviceManager* DeviceManager::CreateOpenGL()
    {
        return new OpenGLDeviceManager();
    }
}