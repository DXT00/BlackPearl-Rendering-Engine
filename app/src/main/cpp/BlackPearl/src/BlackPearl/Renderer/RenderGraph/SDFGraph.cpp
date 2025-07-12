#include "pch.h"
#include "Renderer/RenderGraph/SDFGraph.h"
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
namespace BlackPearl {

    extern MaterialManager* g_materialManager;

	void SDFGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_GDFRenderer = DBG_NEW GlobalDFRenderer(m_DeviceManager->GetDevice());
        m_GDFRenderer->Init(m_Scene);
      
        m_GDFDebugGrabRenderer = DBG_NEW GrabPassRenderer(m_DeviceManager->GetDevice());
        m_GDFDebugGrabRenderer->Init(m_GDFRenderer->GetGDFDebugTexture());

	}

	void SDFGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

       

        m_CommandList->open();
      //  m_CommandList->beginMarker("SDFGraph");

        //render global df arround camera
        m_GDFRenderer->Render(m_CommandList, framebuffer, m_Scene);

        if (Configuration::DebugView == DebugView::DV_SDF) {
           FRHIRenderPassInfo RPShadingInfo(SystemTexture::Get().GetBackBuffer(),
                        ERenderTargetActions::Clear_Store
                    );
           m_CommandList->beginRenderPass(RPShadingInfo, "ShowGDF");

            m_GDFRenderer->ShowGDF(m_CommandList, framebuffer, m_Scene);

         
            //m_GDFDebugGrabRenderer->Render(m_CommandList, framebuffer, m_Scene);
            m_CommandList->endRenderPass();
        }
       // m_CommandList->endMarker();

        m_CommandList->close();
        bIsProbesDirty = false;
	}



	

    void SDFGraph::InitRT()
    {
       

       

    }
}