#pragma once

#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VoxelizationTestLayer :public BlackPearl::Layer {
public:

	VoxelizationTestLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{



		m_VoxelConeTracingRenderer = DBG_NEW BlackPearl::VoxelConeTracingRenderer();
		m_CubeObj = CreateCube("", "");
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(1.0f));
		m_QuadObj = CreateQuad();
		m_QuadObj->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });
		m_VoxelConeTracingRenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_QuadObj, m_CubeObj);

		m_DebugQuadObj = CreateQuad();
		m_DebugQuadObj->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });

		BlackPearl::Renderer::Init();

		//CreateCube();
		BlackPearl::Object* deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl", false);
		deer->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
		deer->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f,-1.0f,0.0f });
		m_BackGroundObjsList.push_back(deer);

		/*	BlackPearl::Object *ironMan = CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl",false);
			ironMan->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
			ironMan->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f,-1.0f,0.0f });*/

			//CreateCube();

			//CreateLight(BlackPearl::LightType::PointLight);
	}

	virtual ~VoxelizationTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		m_VoxelConeTracingRenderer->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>(),m_ObjectsList, GetLightSources(), BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,nullptr, m_CurrentRenderingMode);
	
		//m_VoxelConeTracingRenderer->VoxelizeTest(m_BackGroundObjsList);
	//	m_VoxelConeTracingRenderer->DrawFrontBackFaceOfCube(m_DebugQuadObj);



	}

	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_DebugQuadObj;

	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::VoxelConeTracingRenderer* m_VoxelConeTracingRenderer;
	BlackPearl::VoxelConeTracingRenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXELIZATION_VISUALIZATION;// ::VOXEL_CONE_TRACING;

};
