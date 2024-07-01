#include "pch.h"
#include "DeviceManager.h"
#include "RenderGraph.h"
#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/Application.h"
namespace BlackPearl {
    void DeviceManager::Init(const DeviceCreationParameters& params)
    {
        CreateDeviceAndSwapChain(params);
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
    IFramebuffer* DeviceManager::GetFrameBuffer()
    {
        return m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];
    }
    bool DeviceManager::CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle)
    {
        return false;
    }
    bool DeviceManager::CreateDeviceAndSwapChain(const DeviceCreationParameters& params)
    {
        m_RequestedVSync = params.vsyncEnabled;

        m_DeviceParams.backBufferWidth = params.backBufferWidth;
        m_DeviceParams.backBufferHeight = params.backBufferHeight;

        if (!CreateDeviceAndSwapChain())
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
    void DeviceManager::Run()
    {
        BeginFrame();

        IFramebuffer* framebuffer = m_SwapChainFramebuffers[GetCurrentBackBufferIndex()];

        for (auto it : m_vRenderGraphs)
        {
            it->Render(framebuffer);
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
            width = m_DeviceParams.backBufferWidth;
            height = m_DeviceParams.backBufferHeight;
        }

        m_windowVisible = true;

        if (int(m_DeviceParams.backBufferWidth) != width ||
            int(m_DeviceParams.backBufferHeight) != height ||
            (m_DeviceParams.vsyncEnabled != m_RequestedVSync && GetGraphicsAPI() == DynamicRHI::Type::Vulkan) ||
            GetBackBufferCount() != m_SwapChainFramebuffers.size())
        {
            // window is not minimized, and the size has changed

            BackBufferResizing();

            m_DeviceParams.backBufferWidth = width;
            m_DeviceParams.backBufferHeight = height;
            m_DeviceParams.vsyncEnabled = m_RequestedVSync;

            ResizeSwapChain();
            BackBufferResized();

            assert(GetBackBufferCount() == m_SwapChainFramebuffers.size());
        }

        m_DeviceParams.vsyncEnabled = m_RequestedVSync;
    }

    void DeviceManager::BackBufferResizing()
    {
        m_SwapChainFramebuffers.clear();

        for (auto it : m_vRenderGraphs)
        {
            it->BackBufferResizing();
        }
    }

    void DeviceManager::BackBufferResized()
    {
        for (auto it : m_vRenderGraphs)
        {
            it->BackBufferResized(m_DeviceParams.backBufferWidth,
                m_DeviceParams.backBufferHeight,
                m_DeviceParams.swapChainSampleCount);
        }

        uint32_t backBufferCount = GetBackBufferCount();
        m_SwapChainFramebuffers.resize(backBufferCount);
        for (uint32_t index = 0; index < backBufferCount; index++)
        {
            FramebufferDesc fboDesc;
            fboDesc.addColorAttachment(GetBackBuffer(index));
            m_SwapChainFramebuffers[index] = GetDevice()->createFramebuffer(fboDesc);
        }
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
    IFramebuffer* DeviceManager::GetCurrentFramebuffer()
    {
        return nullptr;
    }
    IFramebuffer* DeviceManager::GetFramebuffer(uint32_t index)
    {
        return nullptr;
    }
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