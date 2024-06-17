#include "pch.h"
#include "BlackPearl/Renderer/ForwardRenderGraph.h"
#include "BlackPearl/Renderer/CullingManager.h"
namespace BlackPearl {
	extern CullingManager* g_cullingManager;
	void ForwardRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_ShaderFactory = std::make_shared<ShaderFactory>(m_DeviceManager->GetDevice(), nullptr, "/shaders");
		m_Scene = scene;
		_CreateRenderTagets();
		
		//PrePass
		m_IndirectCullRenderer = DBG_NEW IndirectCullRenderer();
		m_IndirectCullRenderer->Init(scene);

		//BasePass
		m_BasePassRenderer = DBG_NEW BasePassRenderer();

		//PostProcessPass
		m_PostProcessRenderer = DBG_NEW PostProcessRenderer();
		m_PostProcessRenderer->Init(GetDevice(), m_ShaderFactory);
		

		AddPass(m_IndirectCullRenderer);
		AddPass(m_BasePassRenderer);
		AddPass(m_PostProcessRenderer);
	}

	void ForwardRenderGraph::Render(IFramebuffer* framebuffer, IView& View)
	{
		m_BasePassRenderer->Render(m_CommandList, framebuffer, m_Scene);
		m_PostProcessRenderer->Render(m_CommandList, PostProcessRenderer::RenderPassType::Debug_BlendDebugViz,
			m_ConstantBuffer, miniConstants, m_FrameBuffer, *m_RenderTargets, m_RenderTargets->OutputColor);
	}

	void ForwardRenderGraph::_CreateRenderTagets()
	{
		m_RenderTargets = std::make_unique<RenderTargets>();
		donut::math::int2 renderSize = donut::math::int2(Configuration::WindowWidth);
		m_RenderTargets->Init(GetDevice(), renderSize, renderSize, true, true, Configuration::SwapchainCount);
	}

}