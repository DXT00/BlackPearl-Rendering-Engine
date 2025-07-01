#pragma once

#include "RenderGraph.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIFrameBuffer.h"
#include "RHI/RHICommandList.h"
#include "Renderer/MasterRenderer/GI/IBLProbeRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/DeviceManager.h"
#include "Renderer/MasterRenderer/GlobalDFRenderer.h"
#include "Timestep/Timestep.h"
namespace BlackPearl {
	class SDFGraph :public RenderGraph
	{
	public:
		explicit SDFGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{
		}
		virtual void Init(Scene* scene) override;
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View) override;

		


	private:

        void InitRT();
		// RenderTarget:

		std::vector<ITexture*> m_ColorRTs;
		TextureHandle m_DepthRT;


		CommandListHandle    m_CommandList;
		Scene* m_Scene = nullptr;

        GlobalDFRenderer* m_GDFRnderer = nullptr;

        FramebufferHandle m_DeferredFramebuffer;

        /*texture*/
        TextureHandle m_SpecularBrdfLUTTexture = nullptr;
        bool bRenderBRDFLUT = false;


        bool bIsProbesDirty = true;
	};

}