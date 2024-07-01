#pragma once
#include <BlackPearl.h>

using namespace DX;

class D3D12BasicRenderLayer :public BlackPearl::Layer {

public:

	D3D12BasicRenderLayer(const std::string& name)
		: Layer(name)
	{

		m_D3D12BasicRenderer = DBG_NEW BlackPearl::D3D12BasicRenderer();

		
		m_MainCamera->SetFov(90.0f);
		m_MainCamera->SetPosition(math::float3(0, 0, 0));//math::float3(0,1.387f,22.012f)
		m_CameraPosition = m_MainCamera->GetPosition();
		//BlackPearl::Renderer::Init();


		/*******************************************************************************************************/





	}

	virtual ~D3D12BasicRenderLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;




		//BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		////if (BlackPearl::Input::IsKeyPressed(BP_KEY_Y)) {

		//	m_skybox = m_SkyBoxObj;
		//	GE_CORE_INFO("m_skybox = m_SkyBoxObj");
		//}
		//if (BlackPearl::Input::IsKeyPressed(BP_KEY_N)) {

		//	m_skybox = nullptr;
		//	GE_CORE_INFO("m_skybox = nullptr");

		//}

		//m_RayTracingRenderer->RenderSpheres(m_MainCamera);
	//	m_RayTracingRenderer->Render();
		//m_RayTracingRenderer->RenderMaterialSpheres(m_MainCamera);
		//m_RayTracingRenderer->RenderGroup(m_MainCamera, m_Group->GetRoot());
		m_D3D12BasicRenderer->Render();



	}

	void OnAttach() override {


	}


private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_PointLightObj;

	BlackPearl::Object* m_QuadObj;
	BlackPearl::Group* m_Group;
	BlackPearl::Object* m_Sphere1;
	BlackPearl::Object* m_Sphere2;
	BlackPearl::Object* m_Sphere3;
	BlackPearl::Object* m_Sphere4;
	BlackPearl::Object* m_Sphere5;

	BlackPearl::Object* m_BunnyRTXTransformNode;

	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_skybox = nullptr;
	BlackPearl::Object* group_obj;
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::D3D12BasicRenderer* m_D3D12BasicRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	BlackPearl::SceneBuilder* m_SceneBuilder;



};
