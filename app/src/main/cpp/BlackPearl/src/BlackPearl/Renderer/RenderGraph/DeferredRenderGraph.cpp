#include "pch.h"
#include "Renderer/RenderGraph/DeferredRenderGraph.h"
#include "Application.h"
#include "Renderer/GbufferInfo.h"
#include "Renderer/Material/MaterialManager.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
//#include <EGL/eglext.h>
//#include <EGL/eglplatform.h>

#endif
#include "Renderer/SystemTextures.h"
#include "Timestep/TimeCounter.h"
#include "Renderer/GIManager.h"

namespace BlackPearl {

    extern MaterialManager* g_materialManager;
    extern GIManager* g_GIManager;

	void DeferredRenderGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_SkyboxRenderer = DBG_NEW SkyboxRenderer(m_DeviceManager->GetDevice());
        m_GbufferRenderer = DBG_NEW GBufferRenderer(m_DeviceManager->GetDevice());
        m_DeferredShadingRenderer = DBG_NEW DeferredShadingRenderer(m_DeviceManager->GetDevice());
        m_PlsCopyRenderer = DBG_NEW GrabPassRenderer(m_DeviceManager->GetDevice());
        m_GrabPassRenderer = DBG_NEW GrabPassRenderer(m_DeviceManager->GetDevice());
        m_ToneMappingRenderer = DBG_NEW ToneMappingRenderer(m_DeviceManager->GetDevice());
      //  m_GIRenderer = g_GIManager->CreateGIRenderer(m_DeviceManager->GetDevice(),GIMethod::DDGI);

        m_SkyboxRenderer->Init();
        m_GbufferRenderer->Init();
        m_DeferredShadingRenderer->Init();
        m_GrabPassRenderer->Init(SystemTexture::Get().SceneColor);
        m_PlsCopyRenderer->Init(nullptr, true);
    //    m_GIRenderer->Init(scene);

        m_ToneMappingRenderer->Init(SystemTexture::Get().SceneColor);
        
        
      
        
        /*m_PostProcessRenderer = DBG_NEW PostProcessRenderer();
        m_PostProcessRenderer->Init(GetDevice(), m_ShaderFactory);*/

        AddPass(m_SkyboxRenderer);
        AddPass(m_GbufferRenderer);
        AddPass(m_DeferredShadingRenderer);


	}
	void DeferredRenderGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {
		
        if (Configuration::DebugView != DebugView::DV_Lit)
            return;

      //  m_CommandList->beginMarker("DeferredRenderGraph");

		if (SupportSinglePass(Configuration::MSAA_SAMPLES)) {
			RenderSinglePass(ts, framebuffer, View);
		}
		else {
			RenderMultiPass(ts, framebuffer, View);
		}
     //   m_CommandList->endMarker();
	}
    void DeferredRenderGraph::RenderUI(IFramebuffer* framebuffer, IView* View)
    {
        if (Configuration::bUseIndirectLight
            && g_GIManager->GetGIRenderer()) {
            g_GIManager->GetGIRenderer()->RenderUI(framebuffer, View);
        }
    }
	// 直接 draw 到 defualt frambuffer -- >也就是 SceneColor 上
	void DeferredRenderGraph::RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
        m_CommandList->open();

        {
            SCOPE_TIME_COUNTER(Deferred_SinglePass0)
            FRHIRenderPassInfo RPInfo(
                    m_ColorRTs.size(),
                    m_ColorRTs.data(),
                    ERenderTargetActions::Clear_Store,
                    SystemTexture::Get().SceneDepth,
                    EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);

            RPInfo.SubpassHint = ESubpassHint::DeferredShadingSubpass;
            m_CommandList->beginRenderPass(RPInfo, "DeferredSinglePass");


            m_GbufferRenderer->Render(m_CommandList, framebuffer, m_Scene);

            m_CommandList->nextSubpass();

            bool renderGI = false;

            m_DeferredShadingRenderer->Render(m_CommandList, framebuffer, m_Scene, renderGI);

           // pls-->SceneColor(equal to BackBuffer)
            m_PlsCopyRenderer->Render(m_CommandList, framebuffer, m_Scene);


            //m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);

// 在渲染结束时手动刷新
//            if (GL_EXT_shader_pixel_local_storage) {
//                glMemoryBarrierEXT(GL_SHADER_PIXEL_LOCAL_BARRIER_BIT_EXT);
//            }
            //glFlush();
            m_CommandList->endRenderPass();
        }

        {
//            //Grab pass: SceneColor-->default framebuffer
//            SCOPE_TIME_COUNTER(Deferred_SinglePass1)
//
//            FRHIRenderPassInfo GrabPassInfo(framebuffer->getDesc().colorAttachments[0].texture,
//                                             ERenderTargetActions::Load_Store,
//                                             SystemTexture::Get().SceneDepth,
//                                             EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);
//            m_CommandList->beginRenderPass(GrabPassInfo, "GrabPass");
//
//            m_GrabPassRenderer->Render(m_CommandList, framebuffer, m_Scene);
//
//            m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);
//
//            m_CommandList->endRenderPass();

        }
		m_CommandList->close();
	}
	void DeferredRenderGraph::RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View)
	{
  

        m_CommandList->open();

        {
            SCOPE_TIME_COUNTER(Deferred_MultiPass0)
            FRHIRenderPassInfo RPInfo(
                m_ColorRTs.size(),
                m_ColorRTs.data(),
                ERenderTargetActions::Clear_Store,
                SystemTexture::Get().SceneDepth,
                EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil);
            //draw emissive to sceneColor
            m_CommandList->beginRenderPass(RPInfo, "DeferredGbufferPass");
            m_GbufferRenderer->Render(m_CommandList, framebuffer, m_Scene);

            m_CommandList->endRenderPass();
           
        }
        bool renderGI = true;// ShouldRender();
        if(Configuration::bUseIndirectLight && g_GIManager->GetGIRenderer() && renderGI)
            g_GIManager->GetGIRenderer()->ProbeGather(m_CommandList, framebuffer, m_Scene);


        {
            SCOPE_TIME_COUNTER(Deferred_MultiPass1)

            FRHIRenderPassInfo RPShadingInfo(SystemTexture::Get().SceneColor,
                                             ERenderTargetActions::Load_Store,
                                             SystemTexture::Get().SceneDepth,
                                             EDepthStencilTargetActions::LoadDepthStencil_StoreDepthStencil);
            //draw direct light, indirect light to sceneColor
            m_CommandList->beginRenderPass(RPShadingInfo, "DeferredShadingPass");
            m_DeferredShadingRenderer->Render(m_CommandList, framebuffer, m_Scene, renderGI);

            //draw skybox
            m_SkyboxRenderer->Render(m_CommandList, framebuffer, m_Scene);

            //todo:: draw probes --> use gi manager
            if (Configuration::bUseIndirectLight && Configuration::bShowProbes
                && g_GIManager->GetGIRenderer()) {
                g_GIManager->GetGIRenderer()->ShowProbes(m_CommandList, framebuffer, m_Scene);
            }

            //m_DeferredShadingRenderer->ShowPointLight(m_CommandList, framebuffer, m_Scene);
            m_CommandList->endRenderPass();
        }

        {
            SCOPE_TIME_COUNTER(Deferred_MultiPass2)

                FRHIRenderPassInfo RPShadingInfo(SystemTexture::Get().GetBackBuffer(),
                    ERenderTargetActions::Clear_Store
                  );
            //gamma correction, tonemapping
            m_CommandList->beginRenderPass(RPShadingInfo, "ToneMapping");
            m_ToneMappingRenderer->Render(m_CommandList, framebuffer, m_Scene);


            m_CommandList->endRenderPass();
        }

        


        m_CommandList->close();
	}
    void DeferredRenderGraph::InitRT()
    {
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
           

        
       

    }
}