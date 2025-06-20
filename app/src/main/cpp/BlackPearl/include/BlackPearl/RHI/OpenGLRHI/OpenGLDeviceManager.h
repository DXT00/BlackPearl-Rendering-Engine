#include "pch.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "RHI/RHIDefinitions.h"
#include "OpenGLViewport.h"
namespace BlackPearl {

    class OpenGLViewport;
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
        void CreateDevice() override;
        bool CreateViewport(uint32_t width, uint32_t height, Format format, bool bFullScreen) override;
        void DestroyViewport() override;
        virtual void ResizeViewport() override;
        virtual void BackBufferResizedInner() override;
        virtual FramebufferHandle GetCurrentFramebuffer() override;

        void BeginFrame() override;
        void Present() override;

        virtual RHIViewport* GetViewport()  override { return m_Viewport; };
        virtual void _RHIBeginDrawingViewport(RHIViewport* viewport, ITexture* renderTarget) override;
        virtual void _RHIEndDrawingViewport(RHIViewport* viewport) override;

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

        OpenGLViewport* m_Viewport = nullptr;

        CommandListHandle m_CommandList = nullptr;
        uint32_t m_BackBufferIndex = 0;


    };


}