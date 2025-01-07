#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "OpenGLDeviceManager.h"
#include "OpenGLDevice.h"
namespace BlackPearl {
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
        m_NvrhiDevice = Device::createDevice();
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