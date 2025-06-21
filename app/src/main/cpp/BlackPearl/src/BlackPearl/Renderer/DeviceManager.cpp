#include "pch.h"
#include "Renderer/DeviceManager.h"
#include "BlackPearl/Renderer/RenderGraph/RenderGraph.h"
#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/Application.h"
#include "Renderer/SystemTextures.h"

namespace BlackPearl {
    void DeviceManager::Init(const DeviceCreationParameters& params)
    {
        m_FirstFrame = true;
        CreateDevice();

        SystemTexture::Get().Init(GetDevice());
        CreateViewport(params);
    }
    DeviceManager* DeviceManager::Create(DynamicRHI::Type api)
    {
        switch (api)
        {

#if GE_API_D3D12
        case DynamicRHI::Type::D3D12:
            return CreateD3D12();
#endif
#if GE_API_VULKAN
        case DynamicRHI::Type::Vulkan:
            return CreateVK();
#endif
        default:
            //GE_CORE_ERROR("DeviceManager::Create: Unsupported Graphics API");
            return CreateOpenGL();
        }
    }
    //IFramebuffer* DeviceManager::GetFrameBuffer()
    //{
    //    return m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];
    //}
    bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle)
    {
        return false;
    }
    bool DeviceManager::CreateViewport(const DeviceCreationParameters& params)
    {
        m_RequestedVSync = params.vsyncEnabled;

        m_DeviceParams.backBufferWidth = params.backBufferWidth;
        m_DeviceParams.backBufferHeight = params.backBufferHeight;

        if (!CreateViewport(m_DeviceParams.backBufferWidth, m_DeviceParams.backBufferHeight, m_DeviceParams.swapChainFormat, m_DeviceParams.startFullscreen))
            return false;

        UpdateWindowSize();

        return true;
    }
    void DeviceManager::AddRenderGraphToFront(RenderGraph* pRenderGraph)
    {
        m_vRenderGraphs.remove(pRenderGraph);
        m_vRenderGraphs.push_front(pRenderGraph);

        pRenderGraph->BackBufferResizing();
        pRenderGraph->BackBufferResized(
            m_DeviceParams.backBufferWidth,
            m_DeviceParams.backBufferHeight,
            m_DeviceParams.swapChainSampleCount);
    }
    void DeviceManager::AddRenderGraphToBack(RenderGraph* pRenderGraph)
    {
        m_vRenderGraphs.remove(pRenderGraph);
        m_vRenderGraphs.push_back(pRenderGraph);

        pRenderGraph->BackBufferResizing();
        pRenderGraph->BackBufferResized(
            m_DeviceParams.backBufferWidth,
            m_DeviceParams.backBufferHeight,
            m_DeviceParams.swapChainSampleCount);
    }
    void DeviceManager::RemoveRenderGraph(RenderGraph* pRenderGraph)
    {
        m_vRenderGraphs.remove(pRenderGraph);
    }
    void DeviceManager::Run(Timestep ts)
    {
        //BeginFrame();

        IFramebuffer* framebuffer = GetCurrentFramebuffer();

        for (auto it : m_vRenderGraphs)
        {
            it->Render(ts, framebuffer, Renderer::GetSceneData());
        }
        //Present();

    }
    void DeviceManager::RunUI()
    {
        IFramebuffer* framebuffer = GetCurrentFramebuffer();

        for (auto it : m_vRenderGraphs)
        {
            it->RenderUI(framebuffer, Renderer::GetSceneData());
        }

    }
    void DeviceManager::GetWindowDimensions(int& width, int& height)
    {
    }
    void DeviceManager::UpdateWindowSize()
    {
        int width;
        int height;
        math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();

        if (windowSize.x == 0 || windowSize.y == 0)
        {
            // window is minimized
            m_windowVisible = false;
            return;
            
        }
        else
        {
            width = windowSize.x;//m_DeviceParams.backBufferWidth;
            height =  windowSize.y;//m_DeviceParams.backBufferHeight;
        }

        m_windowVisible = true;

        if (int(m_DeviceParams.backBufferWidth) != width ||
            int(m_DeviceParams.backBufferHeight) != height ||
            (m_DeviceParams.vsyncEnabled != m_RequestedVSync) ||
            m_FirstFrame)
        {
            m_FirstFrame = false;
            m_DeviceParams.backBufferWidth = width;
            m_DeviceParams.backBufferHeight = height;
            m_DeviceParams.vsyncEnabled = m_RequestedVSync;

            ResizeViewport();
            BackBufferResized();

        }

        m_DeviceParams.vsyncEnabled = m_RequestedVSync;
    }



    void DeviceManager::BackBufferResized()
    {
        for (auto it : m_vRenderGraphs)
        {
            it->BackBufferResized(m_DeviceParams.backBufferWidth,
                m_DeviceParams.backBufferHeight,
                m_DeviceParams.swapChainSampleCount);
        }
        BackBufferResizedInner();
      
    }
    void DeviceManager::Animate(double elapsedTime)
    {
    }
  
    void DeviceManager::UpdateAverageFrameTime(double elapsedTime)
    {
    }
    const DeviceCreationParameters& DeviceManager::GetDeviceParams()
    {
        return m_DeviceParams;
    }
   
    void DeviceManager::WindowPosCallback(int xpos, int ypos)
    {
    }
    void DeviceManager::KeyboardUpdate(int key, int scancode, int action, int mods)
    {
    }
    void DeviceManager::KeyboardCharInput(unsigned int unicode, int mods)
    {
    }
    void DeviceManager::MousePosUpdate(double xpos, double ypos)
    {
    }
    void DeviceManager::MouseButtonUpdate(int button, int action, int mods)
    {
    }
    void DeviceManager::MouseScrollUpdate(double xoffset, double yoffset)
    {
    }
    //IFramebuffer* DeviceManager::GetCurrentFramebuffer()
    //{
    //    return nullptr;
    //}
    //IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
    //{
    //    return nullptr;
    //}
    void DeviceManager::Shutdown()
    {
    }
    void DeviceManager::SetWindowTitle(const char* title)
    {
    }
    void DeviceManager::SetInformativeWindowTitle(const char* applicationName, bool includeFramerate, const char* extraInfo)
    {
    }
}