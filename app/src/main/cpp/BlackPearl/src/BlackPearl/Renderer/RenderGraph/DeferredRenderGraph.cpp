#include "pch.h"
#include "Renderer/RenderGraph/DeferredRenderGraph.h"
#include "Application.h"
namespace BlackPearl {


	void DeferredRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;
		if (SupportSinglePass(Configuration::MSAA_SAMPLES)) {

			
		}
		else {
			
		}



	}
	void DeferredRenderGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {
		
		if (SupportSinglePass(Configuration::MSAA_SAMPLES)) {
			RenderSinglePass(ts, framebuffer, View);
		}
		else {
			RenderMultiPass(ts, framebuffer, View);
		}
	}
	void DeferredRenderGraph::RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
		FRHIRenderPassInfo RPInfo(
			4,
			m_ColorRTs.data(),
			ERenderTargetActions::Load_Store);


		if (SupportPLS()) {

		}

		m_CommandList->open();

		m_CommandList->beginRenderPass(RPInfo, "DeferredSinglePass");

		m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);
		
		m_GbufferRenderer->Render(m_CommandList, framebuffer, m_Scene);
		m_CommandList->nextSubpass();
		m_DeferredShadingRenderer->Render(m_CommandList, framebuffer, m_Scene);

		m_CommandList->endRenderPass();
		m_CommandList->close();
	}
	void DeferredRenderGraph::RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
	}
}