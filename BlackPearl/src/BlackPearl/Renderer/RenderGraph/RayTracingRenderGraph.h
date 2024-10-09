#pragma once
#include "RenderGraph.h"

#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Renderer/CommonRenderPass.h"
#include "BlackPearl/Renderer/MasterRenderer/RTXDI/RtxdiRenderer.h"
namespace BlackPearl {
	class RayTracingRenderGraph : public RenderGraph
	{
	public:
		explicit RayTracingRenderGraph(DeviceManager* deviceManager)
			: RenderGraph(deviceManager)
		{ }
		virtual void Init(Scene* scene);
		virtual void Render(IFramebuffer* framebuffer, IView* View);
	private:
		void BuildOpacityMicromaps(ICommandList* commandList, uint32_t frameIndex);
		void BuildTLAS(ICommandList* commandList, uint32_t frameIndex) const;
		void PathTrace(IFramebuffer* framebuffer);
		void Denoise(IFramebuffer* framebuffer);
		void PostProcessAA(IFramebuffer* framebuffer);
		IFramebuffer* m_FrameBuffer;

		CommandListHandle                    m_CommandList;
		BufferHandle                         m_ConstantBuffer;
		std::shared_ptr<CommonRenderPasses> m_CommonPasses;
		BindingLayoutHandle                  m_BindlessLayout;

		//Rtxdi pass
		std::unique_ptr<RtxdiRenderer>                  m_RtxdiPass;

		//Lighting pass

		std::shared_ptr<ShaderFactory> m_ShaderFactory;
		std::unique_ptr<RenderTargets> m_RenderTargets;
		glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };

	};
}

