#pragma once

#include "RenderGraph.h"
#include "RHI/RHITexture.h"
#include "RHI/RHIFrameBuffer.h"
#include "RHI/RHICommandList.h"
#include "Renderer/Renderer.h"
#include "Renderer/DeviceManager.h"
#include "Renderer/MasterRenderer/VoxelConeTracingRenderer.h"
#include "Timestep/Timestep.h"
namespace BlackPearl {
	class VoxelGraph :public RenderGraph
	{
	public:
		explicit VoxelGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{
		}
		virtual void Init(Scene* scene) override;
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View) override;

        bool ShouldRender();



	private:

        void InitRT();
		// RenderTarget:

		//std::vector<ITexture*> m_ColorRTs;
		//TextureHandle m_DepthRT;


		CommandListHandle    m_CommandList;
		Scene* m_Scene = nullptr;

        VoxelConeTracingRenderer* m_VoxelRnderer = nullptr;

        FramebufferHandle m_DeferredFramebuffer;

        /*texture*/
        TextureHandle m_SpecularBrdfLUTTexture = nullptr;
        bool bRenderBRDFLUT = false;


        bool bIsVoxelsDirty = true;
    };

}