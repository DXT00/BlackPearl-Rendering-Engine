#include "pch.h"
#include "Renderer/RenderGraph/DeferredRenderGraph.h"
#include "Application.h"
#include "Renderer/GbufferInfo.h"
#include "Renderer/Material/MaterialManager.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
#include "Renderer/SystemTextures.h"
namespace BlackPearl {

    extern MaterialManager* g_materialManager;

	void DeferredRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_SkyboxRenderer = DBG_NEW SkyboxRenderer(m_DeviceManager->GetDevice());
        m_GbufferRenderer = DBG_NEW GBufferRenderer(m_DeviceManager->GetDevice());
        m_DeferredShadingRenderer = DBG_NEW DeferredShadingRenderer(m_DeviceManager->GetDevice());

        m_SkyboxRenderer->Init();
        m_GbufferRenderer->Init();
        m_DeferredShadingRenderer->Init();

        /*m_PostProcessRenderer = DBG_NEW PostProcessRenderer();
        m_PostProcessRenderer->Init(GetDevice(), m_ShaderFactory);*/

        AddPass(m_SkyboxRenderer);
        AddPass(m_GbufferRenderer);
        AddPass(m_DeferredShadingRenderer);


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
        m_CommandList->open();

		FRHIRenderPassInfo RPInfo(
            m_ColorRTs.size(),
			m_ColorRTs.data(),
			ERenderTargetActions::Clear_Store,
            SystemTexture::Get().SceneDepth,
            EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);


		m_CommandList->beginRenderPass(RPInfo, "DeferredSinglePass");

	  //  m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);
		m_GbufferRenderer->Render(m_CommandList, framebuffer, m_Scene);
		m_CommandList->nextSubpass();
		m_DeferredShadingRenderer->Render(m_CommandList, framebuffer, m_Scene);

		m_CommandList->endRenderPass();
		m_CommandList->close();
	}
	void DeferredRenderGraph::RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
  

        m_CommandList->open();

        {
            FRHIRenderPassInfo RPInfo(
                m_ColorRTs.size(),
                m_ColorRTs.data(),
                ERenderTargetActions::Load_Store,
                SystemTexture::Get().SceneDepth,
                EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);

            m_CommandList->beginRenderPass(RPInfo, "DeferredGbufferPass");
            //todo:: sky在哪里画好？
            m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);

            m_GbufferRenderer->Render(m_CommandList, framebuffer, m_Scene);
            m_CommandList->endRenderPass();
        }

        {
            FRHIRenderPassInfo RPShadingInfo(framebuffer->getDesc().colorAttachments[0].texture, ERenderTargetActions::Load_Store);
         /*   FRHIRenderPassInfo RPShadingInfo(
                SystemTexture::Get().SceneColor,
                ERenderTargetActions::Load_Store);*/
            m_CommandList->beginRenderPass(RPShadingInfo, "DeferredShadingPass");

            m_DeferredShadingRenderer->Render(m_CommandList, framebuffer, m_Scene);

            m_CommandList->endRenderPass();
        }

        m_CommandList->close();
	}
    void DeferredRenderGraph::InitRT()
    {
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
           // m_DeferredFramebuffer = GetDevice()->createFramebuffer(fboDesc);

        
       

    }
}