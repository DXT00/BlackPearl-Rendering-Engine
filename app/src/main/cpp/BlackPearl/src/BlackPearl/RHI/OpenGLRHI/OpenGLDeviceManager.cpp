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

    void OpenGLDeviceManager::BeginFrame()
    {
    }

    void OpenGLDeviceManager::Present()
    {
    }

    ITexture* OpenGLDeviceManager::GetCurrentBackBuffer()
    {
        return nullptr;
    }

    ITexture* OpenGLDeviceManager::GetBackBuffer(uint32_t index)
    {
        return nullptr;
    }

    uint32_t OpenGLDeviceManager::GetCurrentBackBufferIndex()
    {
        return 0;
    }

    uint32_t OpenGLDeviceManager::GetBackBufferCount()
    {
        return 0;
    }

    DeviceManager* DeviceManager::CreateOpenGL()
    {
        return new OpenGLDeviceManager();
    }
}