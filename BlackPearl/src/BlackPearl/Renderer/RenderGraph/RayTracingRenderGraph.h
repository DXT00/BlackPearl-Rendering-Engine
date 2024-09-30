#pragma once
#include "RenderGraph.h"

#include "BlackPearl/Renderer/MasterRenderer/BasePassRenderer.h"
#include "BlackPearl/Renderer/MasterRenderer/RayTraceRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
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

		void _CreateRenderTagets();
		IFramebuffer* m_FrameBuffer;

		CommandListHandle                    m_CommandList;
		BufferHandle                         m_ConstantBuffer;

		RayTraceRenderer* m_RayTracePass;
		BindingLayoutHandle m_BindingLayout;
		BindingLayoutHandle m_BindlessLayout;
		BindingLayoutHandle m_ExtraBindingLayout;
		//Lighting pass

		std::shared_ptr<ShaderFactory> m_ShaderFactory;
		std::unique_ptr<RenderTargets> m_RenderTargets;
		glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };


		Scene* m_Scene = nullptr;
	};
}

