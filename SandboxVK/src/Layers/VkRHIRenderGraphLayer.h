#pragma once
#include <BlackPearl.h>
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/RenderGraph.h"
#include "BlackPearl/Renderer/ForwardRenderGraph.h"


class VkRHIRenderGraphLayer :public BlackPearl::Layer {
public:

	VkRHIRenderGraphLayer(const std::string& name)
		: Layer(name)
	{
		
	}

	virtual ~VkRHIRenderGraphLayer() {

		DestroyObjects();

	}
	void OnSetup() override {
		m_RenderGraph = DBG_NEW BlackPearl::ForwardRenderGraph(m_DeviceManager);
		m_Scene = DBG_NEW BlackPearl::Scene();
		m_RenderGraph->Init(m_Scene);


	}

	void OnUpdate(BlackPearl::Timestep ts) override {

		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		//Update Camera, Materials ..
		m_RenderGraph->Render(m_DeviceManager->GetFrameBuffer());
		
		
	}

	void OnAttach() override {
	}

private:



	BlackPearl::Scene* m_Scene;
	BlackPearl::ForwardRenderGraph* m_RenderGraph;


};
#pragma once
