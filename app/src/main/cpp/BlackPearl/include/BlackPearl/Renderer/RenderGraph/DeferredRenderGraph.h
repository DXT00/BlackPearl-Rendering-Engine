#pragma once
#include "RenderGraph.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "Renderer/MasterRenderer/GBufferRenderer.h"
#include "Renderer/MasterRenderer/DeferredShadingRenderer.h"

namespace BlackPearl {
	class DeferredRenderGraph :public RenderGraph
	{
	public:
		explicit DeferredRenderGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{
		}
		virtual void Init(Scene* scene);
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View);

		void RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View);
		void RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View);


	private:
		// RenderTarget:

		std::vector<ITexture*> m_ColorRTs;
		TextureHandle m_DepthRT;


		CommandListHandle    m_CommandList;
		Scene* m_Scene = nullptr;

		SkyboxRenderer* m_SkyboxRenderer;
		GBufferRenderer* m_GbufferRenderer;
		DeferredShadingRenderer* m_DeferredShadingRenderer;


        FramebufferHandle m_DeferredFramebuffer;
	};

}

