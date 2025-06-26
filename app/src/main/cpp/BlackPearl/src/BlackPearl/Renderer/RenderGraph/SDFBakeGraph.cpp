#include "pch.h"
#include "Renderer/RenderGraph/SDFBakeGraph.h"
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

	void SDFBakeGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_GDFRnderer = DBG_NEW GlobalDFRenderer(m_DeviceManager->GetDevice());
      
        m_GDFRnderer->Init(m_Scene);
      


	}

	void SDFBakeGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

       

        m_CommandList->open();

        //pass 1 render diffuse probes and reflect probes
        m_GDFRnderer->Render(m_CommandList, framebuffer, m_Scene);
        m_CommandList->close();
        bIsProbesDirty = false;
	}



	

    void SDFBakeGraph::InitRT()
    {
       

       

    }
}