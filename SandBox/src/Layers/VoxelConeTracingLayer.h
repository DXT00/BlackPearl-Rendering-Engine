#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VoxelConeTracingLayer :public BlackPearl::Layer {
public:

	VoxelConeTracingLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		

		m_VoxelConeTracingRenderer = DBG_NEW BlackPearl::VoxelConeTracingRenderer();
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_CubeObj = CreateCube();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(1.0f));//必须是单位cube
		m_QuadObj = CreateQuad();
		//m_QuadObj->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });
		
		/*BlackPearl::Object* debugQuad = CreateQuad();
		debugQuad->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });*/

		///*create pointlights*/
		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight);
		light->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,0.0,3.0 });
		//debugQuad->GetComponent<BlackPearl::MeshRenderer>()->SetEnableRender(false);
		m_VoxelConeTracingRenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,m_QuadObj,m_CubeObj);
		


		BlackPearl::Renderer::Init();
	
		//CreateCube();
		BlackPearl::Object *deer=  CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false);
		deer->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.001));
		deer->GetComponent<BlackPearl::Transform>()->SetRotation(glm::vec3(0.000));

		deer->GetComponent<BlackPearl::Transform>()->SetPosition({0.0f,-1.0f,0.0f});
		deer->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);


		/*BlackPearl::Object* ironMan = CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl", false);
		ironMan->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
		ironMan->GetComponent<BlackPearl::Transform>()->SetPosition({ 1.5f,-1.0f,-5.0f });
		ironMan->GetComponent<BlackPearl::Transform>()->SetRotation({ 0.0f,0.0f,0.0f });
		m_BackGroundObjsList.push_back(ironMan);*/

		auto cube = CreateCube();
		cube->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(cube);

	/*	BlackPearl::Object *ironMan = CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl",false);
		ironMan->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.005));
		ironMan->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f,-1.0f,0.0f });*/

		//CreateCube();

		//CreateLight(BlackPearl::LightType::PointLight);
	}

	virtual ~VoxelConeTracingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		m_VoxelConeTracingRenderer->Render(m_BackGroundObjsList, GetLightSources(), BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_CurrentRenderingMode);

		m_BasicRenderer->DrawLightSources(GetLightSources());



	}

	void OnAttach() override {


	}
	
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::VoxelConeTracingRenderer* m_VoxelConeTracingRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;

	BlackPearl::VoxelConeTracingRenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING;//VOXELIZATION_VISUALIZATION
};
