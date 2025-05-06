#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "RHI/OpenGLRHI/OpenGLDeviceManager.h"
#include "RHI/OpenGLRHI/OpenGLDevice.h"
#include "RHI/OpenGLRHI/OpenGLViewport.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/Application.h"

namespace BlackPearl {

    /**
     * Initialize RHI capabilities for the current OpenGL context.
     */

    const char *OpenGLDeviceManager::GetRendererString() const {
        return nullptr;
    }

    IDevice *OpenGLDeviceManager::GetDevice() const {
        return m_NvrhiDevice;

    }

    bool OpenGLDeviceManager::CreateViewport(uint32_t width, uint32_t height, Format format, bool bFullScreen) {


        m_NvrhiDevice = Device::createDevice();

        m_CommandList = GetDevice()->createCommandList();

        m_Viewport = static_cast<OpenGLViewport *>(GetDevice()->createViewport(
                Application::Get().GetWindow().GetNativeWindow(), width, height, format, bFullScreen));
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


    void OpenGLDeviceManager::DestroyViewport() {

    }

    void OpenGLDeviceManager::ResizeViewport() {

    }

    void OpenGLDeviceManager::BackBufferResizedInner() {
        m_Viewport->Resize(m_DeviceParams.backBufferWidth, m_DeviceParams.backBufferHeight,
                           m_DeviceParams.startFullscreen);
        //b0, b1
        // uint32_t backBufferCount = GetBackBufferCount();
        //m_DefaultFramebuffers.resize(backBufferCount);
        /* for (uint32_t index = 0; index < GL_BACKBUFFER_CNT; index++)
         {
             FramebufferDesc fboDesc;
             fboDesc.addColorAttachment(m_DefaultBackBuffers[index]);
             m_DefaultFramebuffers[index] = GetDevice()->createFramebuffer(fboDesc);
         }*/
    }

    FramebufferHandle OpenGLDeviceManager::GetCurrentFramebuffer() {
        return m_Viewport->GetFrameBuffer(m_BackBufferIndex);
    }

    void OpenGLDeviceManager::BeginFrame() {
        _RHIBeginDrawingViewport(m_Viewport, nullptr);
    }

    void OpenGLDeviceManager::Present() {
        m_BackBufferIndex = (m_BackBufferIndex + 1) % GL_BACKBUFFER_CNT;
        _RHIEndDrawingViewport(m_Viewport);

    }

    void OpenGLDeviceManager::_RHIBeginDrawingViewport(RHIViewport *viewport, ITexture *renderTarget) {
        m_CommandList->open();
        m_CommandList->beginDrawingViewport(viewport, renderTarget);
        m_CommandList->close();

    }

    void OpenGLDeviceManager::_RHIEndDrawingViewport(RHIViewport *viewport) {
        m_CommandList->endDrawingViewport(viewport, true, false);

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

    DeviceManager *DeviceManager::CreateOpenGL() {
        return new OpenGLDeviceManager();
    }
}