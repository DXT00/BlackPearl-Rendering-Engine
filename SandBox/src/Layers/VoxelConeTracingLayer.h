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

		m_shader.reset(new BlackPearl::Shader("assets/shaders/voxelization/debug/cube.glsl"));
		
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_VoxelConeTracingRenderer = DBG_NEW BlackPearl::VoxelConeTracingRenderer();
		m_CubeObj = CreateCube();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(5.0f));//必须是单位cube
		m_QuadObj = CreateQuad();
		m_VoxelConeTracingRenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,m_QuadObj,m_CubeObj);
		
		m_DebugQuadObj = CreateQuad();
		m_DebugQuadObj->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });

		BlackPearl::Renderer::Init();
	
		/***************************************** Scene ********************************************************/
		BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight);
		light->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,0.0,3.0 });

		BlackPearl::Object *deer=  CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false);
		deer->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.001));
		deer->GetComponent<BlackPearl::Transform>()->SetRotation(glm::vec3(0.000));

		deer->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,0.0,3.0f });
		deer->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(deer);

		BlackPearl::Object* bunny = Layer::CreateModel("assets/models/bunny/bunny.obj", "assets/shaders/IronMan.glsl", false);
		bunny->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(0.001));
		bunny->GetComponent<BlackPearl::Transform>()->SetRotation(glm::vec3(0.000));
		bunny->GetComponent<BlackPearl::Transform>()->SetPosition({ 3.0,0.0,3.0f });
		bunny->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(bunny);

		auto cube = CreateCube();
		cube->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(cube);

		auto cube1 = CreateCube();
		cube1->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		cube1->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,0.0,3.0f });

		m_BackGroundObjsList.push_back(cube1);

		auto cube2 = CreateCube();
		cube2->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		m_BackGroundObjsList.push_back(cube2);


		/*******************************************************************************************************/
		/*******************************************************************************************************/

	

		

	}

	virtual ~VoxelConeTracingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;
		m_FrameNum++;
		GE_ASSERT(m_FrameNum < MAXLONGLONG, "m_FrameNum out of range!");
		m_FPS =(double) m_FrameNum / runtimeSecond;

		//Switch mode
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {
			m_Mode = (m_Mode + 1) % 2;
			if(m_Mode==0)
			m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXELIZATION_VISUALIZATION;
			else if(m_Mode==1)
				m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING;

		}
		// render
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


	m_VoxelConeTracingRenderer->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>(),m_BackGroundObjsList, GetLightSources(), BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_CurrentRenderingMode);


		//m_VoxelConeTracingRenderer->DrawFrontBackFaceOfCube(m_DebugQuadObj);
		/*
		*/
		//glDisable(GL_CULL_FACE);
		/*m_BasicRenderer->DrawObjects(m_BackGroundObjsList);
		m_shader->Bind();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetPosition(BlackPearl::Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetRotation(BlackPearl::Renderer::GetSceneData()->CameraRotation);
		m_BasicRenderer->DrawObject(m_CubeObj, m_shader);*/


		m_BasicRenderer->DrawLightSources(GetLightSources());

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

	std::shared_ptr<BlackPearl::Shader> m_shader;
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::VoxelConeTracingRenderer* m_VoxelConeTracingRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	
	unsigned int m_Mode = 0;
	BlackPearl::VoxelConeTracingRenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXELIZATION_VISUALIZATION;// VOXEL_CONE_TRACING;//VOXELIZATION_VISUALIZATION
};
