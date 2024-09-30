#include "pch.h"
#include "RayTracingRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
namespace BlackPearl {
#define RTXDI_SCREEN_SPACE_GROUP_SIZE 8

    extern RootFileSystem* g_rootFileSystem;

    void RayTracingRenderGraph::Init(Scene* scene)
    {
        m_CommandList = GetDevice()->createCommandList();
        m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
        m_Scene = scene;
        m_RayTracePass = DBG_NEW RayTraceRenderer();

        std::vector<ShaderMacro> regirMacros;
        bool useRayQuery = false;
        m_RayTracePass->Init(m_DeviceManager->GetDevice(), *m_ShaderFactory, "app/RTXDI/GenerateInitialSamples.hlsl",
            regirMacros, useRayQuery, RTXDI_SCREEN_SPACE_GROUP_SIZE, m_BindingLayout, m_ExtraBindingLayout, m_BindlessLayout);



    }
    void RayTracingRenderGraph::Render(IFramebuffer* framebuffer, IView* View)
    {
    }
}
