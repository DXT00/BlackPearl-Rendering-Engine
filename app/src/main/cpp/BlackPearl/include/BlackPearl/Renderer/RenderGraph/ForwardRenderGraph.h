#pragma once
#include "RenderGraph.h"
#include "Renderer/RenderTargets.h"
//#include "BlackPearl/Renderer/MasterRenderer/IndirectCullRenderer.h"
//#include "BlackPearl/Renderer/MasterRenderer/PostProcessRenderer.h"
#include "Renderer/Renderer.h"
#include "Renderer/MasterRenderer/ForwardShadingRenderer.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "Timestep/Timestep.h"
namespace BlackPearl {
	class ForwardRenderGraph : public RenderGraph
	{
	public:
		explicit ForwardRenderGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{ }
		virtual void Init(Scene* scene) override;
		virtual void Render(Timestep ts, IFramebuffer* framebuffer, IView* View);
	

		void RenderSinglePass(Timestep ts, IFramebuffer* framebuffer, IView* View);
		void RenderMultiPass(Timestep ts, IFramebuffer* framebuffer, IView* View);

	private:

		void _CreateRenderTagets();

		CommandListHandle                    m_CommandList;
		BufferHandle                         m_ConstantBuffer;


		//Lighting pass
		
		//PostProcess pass
		//PostProcessRenderer::SampleMiniConstants  miniConstants = { (0, 0, 0, 0) };
		std::shared_ptr<ShaderFactory> m_ShaderFactory;
		std::unique_ptr<RenderTargets> m_RenderTargets;
		glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
		//PostProcessRenderer* m_PostProcessRenderer;
		//IndirectCullRenderer* m_IndirectCullRenderer;
		//BasePassRenderer* m_BasePassRenderer;
		ForwardShadingRenderer* m_ForwardBasePassRenderer;
		SkyboxRenderer* m_SkyboxRenderer;
		//sky renderer

		// bloom renderer


		Scene* m_Scene = nullptr;
	};
}

