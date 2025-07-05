#include "pch.h"
#include "Renderer/RenderGraph/DDGIGraph.h"
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

	void DDGIGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;


        m_DDGIRenderer = DBG_NEW DDGIRenderer(m_DeviceManager->GetDevice());
      
        m_DDGIRenderer->Init(scene);
      


	}

	void DDGIGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

       

        m_CommandList->open();

   
        //pass 0 render volumes
        m_DDGIRenderer->Render(m_CommandList, framebuffer, m_Scene);
        m_CommandList->close();


        bIsProbesDirty = false;
	}



	

}