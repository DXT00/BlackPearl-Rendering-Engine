#include "pch.h"
#include "Renderer/RenderGraph/VoxelGraph.h"
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

	void VoxelGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_VoxelRnderer = DBG_NEW VoxelConeTracingRenderer(m_DeviceManager->GetDevice());
        m_VoxelRnderer->Init(m_Scene);
      


	}

	void VoxelGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {


        m_CommandList->open();


        if (Configuration::bUpdatVoxelsPerFrame) {
            bIsVoxelsDirty = true;

        }

        if (bIsVoxelsDirty) {
            m_VoxelRnderer->Render(m_CommandList, framebuffer, m_Scene, VoxelConeTracingRenderer::RenderingMode::VOXELIZE);
            bIsVoxelsDirty = false;
        }

        //render global df arround camera
        if (Configuration::DebugView == DebugView::DV_Voxel) {
            m_VoxelRnderer->Render(m_CommandList, framebuffer, m_Scene, VoxelConeTracingRenderer::RenderingMode::VOXELIZATION_VISUALIZATION);

        }

        m_CommandList->close();
	}



	

    void VoxelGraph::InitRT()
    {
       

       

    }
}