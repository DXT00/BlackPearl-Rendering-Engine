#pragma once
#include "RenderGraph.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "Renderer/MasterRenderer/GI/GIRenderer.h"
#include "Renderer/MasterRenderer/GBufferRenderer.h"
#include "Renderer/MasterRenderer/DeferredShadingRenderer.h"
#include "Renderer/MasterRenderer/GrabPassRenderer.h"
#include "Renderer/MasterRenderer/ToneMappingRenderer.h"
namespace BlackPearl {
	class DeferredRenderGraph :public RenderGraph
	{
	public:
		explicit DeferredRenderGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{
		}
		virtual void Init(Scene* scene) override;
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View) override;

		void RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View);
		void RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View);


	private:

        void InitRT();
		// RenderTarget:

		std::vector<ITexture*> m_ColorRTs;
		TextureHandle m_DepthRT;


		CommandListHandle    m_CommandList;
		Scene* m_Scene = nullptr;

		SkyboxRenderer* m_SkyboxRenderer;

        GIRenderer* m_GIRenderer;

		GBufferRenderer* m_GbufferRenderer;
		DeferredShadingRenderer* m_DeferredShadingRenderer;
        GrabPassRenderer* m_GrabPassRenderer;
        GrabPassRenderer* m_PlsCopyRenderer;

        ToneMappingRenderer* m_ToneMappingRenderer;

        FramebufferHandle m_DeferredFramebuffer;
	};

}

