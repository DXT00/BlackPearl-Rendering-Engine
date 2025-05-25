#include "pch.h"
#include "Renderer/RenderGraph/DeferredRenderGraph.h"
#include "Application.h"
#include "Renderer/GbufferInfo.h"
#include "Renderer/Material/MaterialManager.h"
#include "Renderer/SystemTextures.h"
namespace BlackPearl {

    extern MaterialManager* g_materialManager;

	void DeferredRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;
		if (SupportSinglePass(Configuration::MSAA_SAMPLES)) {
            FramebufferDesc fboDesc;
            bool bUsingPixelLocalStorage = SupportPLS();

            if (bUsingPixelLocalStorage)
            {
                m_ColorRTs.push_back(SystemTexture::Get().SceneColor);
            }
            else
            {
                m_ColorRTs.push_back(SystemTexture::Get().SceneColor);
                m_ColorRTs.push_back(SystemTexture::Get().GBufferA);
                m_ColorRTs.push_back(SystemTexture::Get().GBufferB);
                m_ColorRTs.push_back(SystemTexture::Get().GBufferC);

               
            }
            for (size_t i = 0; i < m_ColorRTs.size(); i++)
            {
                fboDesc.addColorAttachment(m_ColorRTs[i]);
            }
            m_DeferredFramebuffer = GetDevice()->createFramebuffer(fboDesc);
			
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
            m_ColorRTs.size(),
			m_ColorRTs.data(),
			ERenderTargetActions::Load_Store,
            SystemTexture::Get().SceneDepth,
            EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);

		m_CommandList->open();

		m_CommandList->beginRenderPass(RPInfo, "DeferredSinglePass");

		m_SkyboxRenderer->Render(m_CommandList, m_DeferredFramebuffer, m_Scene);
		
		m_GbufferRenderer->Render(m_CommandList, m_DeferredFramebuffer, m_Scene);
		m_CommandList->nextSubpass();
		m_DeferredShadingRenderer->Render(m_CommandList, m_DeferredFramebuffer, m_Scene);

		m_CommandList->endRenderPass();
		m_CommandList->close();
	}
	void DeferredRenderGraph::RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
	}
}