#include "pch.h"
#include "ForwardRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/FileSystem/FileSystem.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
namespace BlackPearl {
	extern CullingManager* g_cullingManager;
	extern RootFileSystem* g_rootFileSystem;
	void ForwardRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
		m_Scene = scene;
		_CreateRenderTagets();
		
		////PrePass
		//m_IndirectCullRenderer = DBG_NEW IndirectCullRenderer();
		//m_IndirectCullRenderer->Init(scene);

		//BasePass
		m_BasePassRenderer = DBG_NEW BasePassRenderer();
		BasePassRenderer::CreateParameters params;
		//params.materialBindings = std::make_shared<MaterialBindingCache>(m_DeviceManager->GetDevice(),);
		m_BasePassRenderer->Init(m_DeviceManager->GetDevice(), m_ShaderFactory, params);
		//PostProcessPass
		/*m_PostProcessRenderer = DBG_NEW PostProcessRenderer();
		m_PostProcessRenderer->Init(GetDevice(), m_ShaderFactory);*/
		

		//AddPass(m_IndirectCullRenderer);
		AddPass(m_BasePassRenderer);
		//AddPass(m_PostProcessRenderer);
	}

	void ForwardRenderGraph::Render(IFramebuffer* framebuffer, IView* View)
	{
		m_CommandList->open();
//		m_BasePassRenderer->PrepareLights(m_CommandList, m_Scene->GetLightSources(), math::float3(1.0), math::float3(1.0,0.0,1.0), m_Scene->GetLightProbes());
		m_BasePassRenderer->Render(m_CommandList, framebuffer, m_Scene);
		//m_PostProcessRenderer->Render(m_CommandList, PostProcessRenderer::RenderPassType::Debug_BlendDebugViz,
		//	m_ConstantBuffer, miniConstants, m_FrameBuffer, *m_RenderTargets, m_RenderTargets->OutputColor);
		m_CommandList->close();

		GetDevice()->executeCommandList(m_CommandList);

	}

	void ForwardRenderGraph::_CreateRenderTagets()
	{
		m_RenderTargets = std::make_unique<RenderTargets>();
		math::int2 renderSize = math::int2(Configuration::WindowWidth);
		m_RenderTargets->Init(GetDevice(), renderSize, renderSize, true, true, Configuration::SwapchainCount);
	}

}