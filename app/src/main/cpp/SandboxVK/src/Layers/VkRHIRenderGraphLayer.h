#pragma once
#include <BlackPearl.h>
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/RenderGraph/RenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/ForwardRenderGraph.h"


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
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_CubeObj = CreateCube();

		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });
		m_SphereObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.5,0.5,0.5 });
		m_CubeObj->SetPosition({ 0.0,0.0,-2.0 });
		m_SphereObj->SetPosition({ -0.4,0.0,-2.0 });


		m_MainCamera->SetMoveSpeed(5.0f);
		m_Scene->AddObject(m_SphereObj);
		m_Scene->AddObject(m_CubeObj);



		m_Scene->SetLightSources(GetLightSources());

		m_RenderGraph->Init(m_Scene);

		m_DeviceManager->AddRenderGraphToBack(m_RenderGraph);


	}

	void OnUpdate(BlackPearl::Timestep ts) override {

		InputCheck(ts);


		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		//Update Camera, Materials ..
		m_DeviceManager->UpdateWindowSize();

		m_DeviceManager->Run();

		//m_RenderGraph->Render(m_DeviceManager->GetFrameBuffer(), BlackPearl::Renderer::GetSceneData());
		
		
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
