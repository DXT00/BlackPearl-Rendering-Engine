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
		m_RenderGraph->Init(m_Scene);

		m_Scene = DBG_NEW BlackPearl::Scene();
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_CubeObj = CreateCube();
		//m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });

		m_Scene->AddObject(m_SphereObj);
		m_Scene->AddObject(m_CubeObj);

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

	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SphereObj;


};
#pragma once
