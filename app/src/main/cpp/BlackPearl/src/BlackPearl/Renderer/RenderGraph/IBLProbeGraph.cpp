#include "pch.h"
#include "Renderer/RenderGraph/IBLProbeGraph.h"
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

	void IBLProbeGraph::Init(Scene* scene) {
		m_CommandList = GetDevice()->createCommandList();
		m_Scene = scene;

        InitRT();

        m_IBLProbeRenderer = DBG_NEW IBLProbeRenderer(m_DeviceManager->GetDevice());
      
        m_IBLProbeRenderer->Init(scene);
      


	}

	void IBLProbeGraph::Render(Timestep ts, IFramebuffer* framebuffer, IView* View) {

        if (Configuration::bUpdateProbePerFrame) {
            bIsProbesDirty = true;

        }
        
        if (!bIsProbesDirty)
            return;

        m_CommandList->open();

        //pass 0 render brdf lut
        if (!bRenderBRDFLUT) {
            FRHIRenderPassInfo RPInfo(
                m_SpecularBrdfLUTTexture.Get(),
                ERenderTargetActions::Clear_Store
               );

            m_CommandList->beginRenderPass(RPInfo, "BRDF_LUTPass");
            m_IBLProbeRenderer->RenderSpecularBRDFLUTMap(m_CommandList, framebuffer, m_Scene);
            m_CommandList->endRenderPass();
            bRenderBRDFLUT = true;
        }

        //pass 1 render diffuse probes and reflect probes
        m_IBLProbeRenderer->Render(m_CommandList, framebuffer, m_Scene);
        m_CommandList->close();


        bIsProbesDirty = false;
	}



	

    void IBLProbeGraph::InitRT()
    {
       
            TextureDesc desc;
            desc.type = TextureType::DiffuseMap;
            desc.width = Configuration::EnvironmantMapResolution;
            desc.height = Configuration::EnvironmantMapResolution;
            desc.minFilter = FilterMode::Linear;
            desc.magFilter = FilterMode::Linear;
            desc.wrap = SamplerAddressMode::ClampToEdge;
            desc.format = Format::RG16_FLOAT;
            m_SpecularBrdfLUTTexture = g_deviceManager->GetDevice()->createTexture(desc);

       

    }
}