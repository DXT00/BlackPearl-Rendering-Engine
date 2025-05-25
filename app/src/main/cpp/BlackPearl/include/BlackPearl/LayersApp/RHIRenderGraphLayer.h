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
		m_SphereObj = CreateSphere(0.5, 64, 64);
		m_CubeObj = CreateCube("assets/shaders/glsl/Cube.glsl", "assets/texture/wood.png");
		m_SkyBox = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });
        m_CubeObj->GetComponent<BlackPearl::Transform>()->SetRotation({0,30,0});
		m_SphereObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.5,0.5,0.5 });
		m_CubeObj->SetPosition({ 0.0,0.0,-2.0 });
		m_SphereObj->SetPosition({ -0.4,0.0,-2.0 });


		m_MainCamera->SetMoveSpeed(0.5f);

		m_DirectionLight = CreateLight(BlackPearl::LightType::DirectionLight, "DirectionLight");

		//m_Scene->AddObject(m_SphereObj);
		m_Scene->AddObject(m_CubeObj);
		m_Scene->AddObject(m_SphereObj);



		m_Scene->SetLightSources(GetLightSources());
		m_Scene->SetSkyBox(m_SkyBox);
		m_RenderGraph->Init(m_Scene);

		m_DeviceManager->AddRenderGraphToBack(m_RenderGraph);


	}

	void OnUpdate(BlackPearl::Timestep ts) override {

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();

		InputCheck(ts);


		BlackPearl::Renderer::BeginScene((m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		//Update Camera, Materials ..
		m_DeviceManager->UpdateWindowSize();

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
	BlackPearl::Object* m_SkyBox;
	BlackPearl::Object* m_DirectionLight;


};
#pragma once
