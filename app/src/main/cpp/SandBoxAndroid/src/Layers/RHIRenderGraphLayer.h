#pragma once
//#include <BlackPearl.h>
#include "BlackPearl/Renderer/RenderTargets.h"
#include "BlackPearl/Renderer/RenderGraph/RenderGraph.h"
#include "BlackPearl/Renderer/RenderGraph/ForwardRenderGraph.h"
#include "BlackPearl\LayerScene\Layer.h"

class RHIRenderGraphLayer :public BlackPearl::Layer {
public:

	RHIRenderGraphLayer(const std::string& name)
		: Layer(name)
	{
		
	}

	virtual ~RHIRenderGraphLayer() {

		DestroyObjects();

	}
	void OnSetup() override {
		m_RenderGraph = DBG_NEW BlackPearl::ForwardRenderGraph(m_DeviceManager);

		m_Scene = DBG_NEW BlackPearl::Scene();
        GE_ERROR_JUDGE();
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_CubeObj = CreateCube("assets/shaders/Cube.glsl", "assets/texture/wood.png");
        GE_ERROR_JUDGE();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });
        m_CubeObj->GetComponent<BlackPearl::Transform>()->SetRotation({0,30,0});

        m_SphereObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.5,0.5,0.5 });
		m_CubeObj->SetPosition({ 0.0,0.0,-2.0 });
		m_SphereObj->SetPosition({ -0.4,0.0,-2.0 });

        GE_ERROR_JUDGE();
		m_MainCamera->SetMoveSpeed(0.5f);

        m_MainCamera->SetPosition(glm::vec3(0.0, 0.0, 60.0f));
		//m_Scene->AddObject(m_SphereObj);
		m_Scene->AddObject(m_CubeObj);
        GE_ERROR_JUDGE();


		m_Scene->SetLightSources(GetLightSources());
        GE_ERROR_JUDGE();
		m_RenderGraph->Init(m_Scene);
        GE_ERROR_JUDGE();
		m_DeviceManager->AddRenderGraphToBack(m_RenderGraph);
        GE_ERROR_JUDGE();

	}

	void OnUpdate(BlackPearl::Timestep ts) override {

		InputCheck(ts);

        GE_ERROR_JUDGE();
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
        GE_ERROR_JUDGE();
        //Update Camera, Materials ..
		m_DeviceManager->UpdateWindowSize();
        GE_ERROR_JUDGE();
		m_DeviceManager->Run(ts);

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
