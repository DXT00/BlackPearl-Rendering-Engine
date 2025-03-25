#include "pch.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/DeviceManager.h"
namespace BlackPearl {

#define GL_BACKBUFFER_CNT 2
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
        virtual void BackBufferResizedInner() override;
        virtual IFramebuffer* GetCurrentFramebuffer() override;

        void BeginFrame() override;
        void Present() override;


        virtual void _RHIViewportBeginDraw() override;
        virtual void _RHIViewportEndDraw() override;

        uint32_t GetBackBufferCount()
        {
            return GL_BACKBUFFER_CNT;
        }
 /*       ITexture* GetCurrentBackBuffer() override;
        ITexture* GetBackBuffer(uint32_t index) override;
        uint32_t GetCurrentBackBufferIndex() override;
        uint32_t GetBackBufferCount() override;*/
    private:
        DeviceHandle m_NvrhiDevice;
        //default gl front/ back buffer, empty texture, just adapt the rhi
        FramebufferHandle m_DefaultFramebuffers[2];
        TextureHandle m_DefaultBackBuffers[2];

        uint32_t m_BackBufferIndex = 0;


    };


}