#pragma once
#include <BlackPearl.h>

using namespace DX;

class D3D12MeshShaderLayer :public BlackPearl::Layer {

public:

	D3D12MeshShaderLayer(const std::string& name)
		: Layer(name)
	{
		m_D3D12MeshShaderRenderer = DBG_NEW BlackPearl::D3D12MeshShaderRenderer();
		std::shared_ptr<BlackPearl::Shader> shader(DBG_NEW BlackPearl::Shader(""));
		m_pDragonModel = DBG_NEW BlackPearl::Model("assets/models/meshletModel/Dragon_LOD0.bin", shader, false/*isAnimated*/, false/*verticesSorted*/, false/*createMeshlet*/, true/*isMeshletModel*/);

		m_MainCamera->SetPosition(glm::vec3(0, 0, 0));//glm::vec3(0,1.387f,22.012f)
		m_MainCamera->SetRotation(glm::vec3(0, -90, 0));
		m_MainCamera->SetMoveSpeed(50.0f);
		m_MainCamera->SetRotateSpeed(1.0f);

		m_MainCamera->SetZFar(1000.0f);
		m_MainCamera->SetFov(60.0f);
		m_CameraPosition = m_MainCamera->GetPosition();

		m_Dragon = CreateModel("assets/models/meshletModel/Dragon_LOD0.bin", "", false, "Dragon", false, false, true);
		m_Scene = DBG_NEW BlackPearl::Scene();
		m_Scene->AddObject(m_Dragon);
		m_Scene->AddModel(m_pDragonModel);

		m_D3D12MeshShaderRenderer->Init(m_Scene);
		/*******************************************************************************************************/

	}

	virtual ~D3D12MeshShaderLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;

		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		m_D3D12MeshShaderRenderer->Render(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()));

	}

	void OnAttach() override {


	}


private:
	BlackPearl::Model* m_pDragonModel;
	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_skybox = nullptr;
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

	BlackPearl::D3D12MeshShaderRenderer* m_D3D12MeshShaderRenderer;

	BlackPearl::Scene* m_Scene;
	BlackPearl::Object* m_Dragon;

};
