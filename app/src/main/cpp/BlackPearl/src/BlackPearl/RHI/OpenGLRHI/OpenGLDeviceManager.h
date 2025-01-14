#include "pch.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/DeviceManager.h"
namespace BlackPearl {
    class OpenGLDeviceManager : public DeviceManager
    {

    public:
        [[nodiscard]] const char* GetRendererString() const override;

        [[nodiscard]] IDevice* GetDevice() const override;

        DynamicRHI::Type GetGraphicsAPI() const override 
        {
            return DynamicRHI::Type::OpenGL;
        }

    protected:
        bool CreateDeviceAndSwapChain() override;
        void DestroyDeviceAndSwapChain() override;
        virtual void ResizeSwapChain() override;


        void BeginFrame() override;
        void Present() override;


        ITexture* GetCurrentBackBuffer() override;
        ITexture* GetBackBuffer(uint32_t index) override;
        uint32_t GetCurrentBackBufferIndex() override;
        uint32_t GetBackBufferCount() override;
    private:
        DeviceHandle m_NvrhiDevice;
        struct FPlatformOpenGLDevice* m_PlatformDevice = nullptr;

    };


}