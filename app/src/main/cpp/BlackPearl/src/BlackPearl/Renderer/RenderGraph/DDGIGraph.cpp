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
#include "Renderer/GIManager.h"

namespace BlackPearl {
    extern GIManager* g_GIManager;
    extern MapManager* g_mapManager;

    extern MaterialManager* g_materialManager;

	void DDGIGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;


        m_DDGIRenderer = g_GIManager->CreateGIRenderer(GetDevice(), GIMethod::DDGI);

      
        m_DDGIRenderer->Init(scene);
      


	}

	void DDGIGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

       

        m_CommandList->open();

        if (!ShouldRender())
            return;
        //pass 0 render volumes
        m_DDGIRenderer->Render(m_CommandList, framebuffer, m_Scene);
        m_CommandList->close();


        bIsProbesDirty = false;
	}

    //render GI if camera is inside map
    bool DDGIGraph::ShouldRender()
    {
        auto camPos = Renderer::GetSceneData()->CameraPosition;
        int areaId = g_mapManager->CalculateAreaId(camPos);

        return areaId >=0 && g_mapManager->GetArea(areaId);
    }



	

}