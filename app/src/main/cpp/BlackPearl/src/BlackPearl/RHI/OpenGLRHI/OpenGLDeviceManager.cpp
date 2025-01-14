#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "OpenGLDeviceManager.h"
#include "OpenGLDevice.h"
#include "OpenGLDriver/OpenGLDrvPrivate.h"
namespace BlackPearl {


	/**
 * Initialize RHI capabilities for the current OpenGL context.
 */
	static void InitRHICapabilitiesForGL() {

	}

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
		m_PlatformDevice = PlatformCreateOpenGLDevice();

		InitRHICapabilitiesForGL();

		assert(PlatformOpenGLCurrentContext(m_PlatformDevice) == CONTEXT_Shared);
//
//		if (PlatformCanEnableGPUCapture())
//		{
//			EnableIdealGPUCaptureOptions(true);
//
//			
//		}
//
//		{
//			// Temp disable gpusorting for Opengl because of issues on Adreno and Mali devices
//			auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("FX.AllowGPUSorting"));
//			if (CVar)
//			{
//#if PLATFORM_ANDROID
//				if (!AndroidThunkCpp_IsOculusMobileApplication())
//#endif
//				{
//					CVar->Set(false);
//				}
//			}
//		}

#ifdef GE_PLATFORM_ANDRIOD
		// Temp disable gpu particles for OpenGL because of issues on Adreno devices with old driver version
		if (GRHIVendorId == 0x5143)
		{
			auto* CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("fx.NiagaraAllowGPUParticles"));
			if (CVar)
			{
				int32 DriverVersion = INT32_MAX;
				FString SubVersionString, DriverVersionString;
				FString VersionString = FString(ANSI_TO_TCHAR((const ANSICHAR*)glGetString(GL_VERSION)));

				if (VersionString.Split(TEXT("V@"), nullptr, &SubVersionString) && SubVersionString.Split(TEXT(" "), &DriverVersionString, nullptr))
				{
					DriverVersion = FCString::Atoi(*DriverVersionString);
				}

				if (DriverVersion < 415)
				{
					CVar->Set(false);
					UE_LOG(LogRHI, Log, TEXT("GPU particles are disabled on this device because the driver version %d is less than 415"), DriverVersion);
				}
			}
		}
#endif

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