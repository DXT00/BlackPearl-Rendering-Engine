#pragma once
#include "RenderGraph.h"
#include "BlackPearl/Renderer/MasterRenderer/IndirectCullRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/PostProcessRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
namespace BlackPearl {
	class ForwardRenderGraph : public RenderGraph
	{
	public:
		explicit ForwardRenderGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{ }
		virtual void Init(Scene* scene);
		virtual void Render(IFramebuffer* framebuffer, IView* View);
	private:

		void _CreateRenderTagets();
		IFramebuffer* m_FrameBuffer;

		CommandListHandle                    m_CommandList;
		BufferHandle                         m_ConstantBuffer;


		//Lighting pass
		
		//PostProcess pass
		PostProcessRenderer::SampleMiniConstants  miniConstants = { (0, 0, 0, 0) };
		std::shared_ptr<ShaderFactory> m_ShaderFactory;
		std::unique_ptr<RenderTargets> m_RenderTargets;
		glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };
		PostProcessRenderer* m_PostProcessRenderer;
		IndirectCullRenderer* m_IndirectCullRenderer;
		BasePassRenderer* m_BasePassRenderer;

		Scene* m_Scene = nullptr;
	};
}

