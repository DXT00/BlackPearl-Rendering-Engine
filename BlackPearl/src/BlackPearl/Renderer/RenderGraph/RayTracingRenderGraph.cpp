#include "pch.h"
#include "RayTracingRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/RTXDI/RtxdiRenderer.h"

namespace BlackPearl {

    extern RootFileSystem* g_rootFileSystem;

    void RayTracingRenderGraph::Init(Scene* scene)
    {
        m_CommandList = GetDevice()->createCommandList();
        m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
        m_CommonPasses = std::make_shared<CommonRenderPasses>(GetDevice(), m_ShaderFactory);


        //create render pass
        m_RtxdiPass = std::make_unique<RtxdiRenderer>(GetDevice(), m_ShaderFactory, m_CommonPasses, m_BindlessLayout);


    }
    void RayTracingRenderGraph::Render(IFramebuffer* framebuffer, IView* View)
    {
        PathTrace(framebuffer);
        Denoise(framebuffer);
    }

    void RayTracingRenderGraph::PathTrace(IFramebuffer* framebuffer)
    {
    }

    void RayTracingRenderGraph::Denoise(IFramebuffer* framebuffer)
    {
    }
    
}
