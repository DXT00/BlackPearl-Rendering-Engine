#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VoxelConeTracingSVOLayer :public BlackPearl::Layer {
public:

	VoxelConeTracingSVOLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{
		m_CubeObj = CreateCube();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(25.0f));//必须是单位cube
		m_QuadVisualObj = CreateQuad();
		m_QuadBRDFLUTObj = CreateQuad();
		m_SurroundSphere = CreateSphere(1, 64, 64);
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_VoxelConeTracingSVORenderer = DBG_NEW BlackPearl::VoxelConeTracingSVORenderer();


		

	

		BlackPearl::Renderer::Init();

		/***************************************** Scene ********************************************************/
		m_SkyBoxObj = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
		//LoadStaticBackGroundObject("Sphere");
		LoadScene("CornellScene");
		//BlackPearl::Object* cube1 = CreateCube();
		//cube1->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		//cube1->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorSpecularColor({ 0,0,0 });
		//cube1->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->SetMaterialColorDiffuseColor({ 0,0.294f,1.0f });
		//m_BackGroundObjsList.push_back(cube1);

		//BlackPearl::Object* light = CreateLight(BlackPearl::LightType::PointLight);
		//light->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0,1.25,3.0 });
		//light->GetComponent<BlackPearl::Transform>()->SetLastPosition({ 0.0,-1.0,0.0 });//0.0,0.0,3.0
		//light->GetComponent<BlackPearl::MeshRenderer>()->SetIsShadowObjects(false);
		//LoadScene("SpheresScene");

		m_skybox = m_SkyBoxObj;
		/*******************************************************************************************************/
		/*******************************************************************************************************/
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		m_VoxelConeTracingSVORenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,
			m_CubeObj, m_QuadBRDFLUTObj, m_BackGroundObjsList, m_SkyBoxObj);
	}

	virtual ~VoxelConeTracingSVOLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;

		//Switch mode
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {
			m_Mode = (m_Mode + 1) % 2;
			if (m_Mode == 0)
				m_CurrentRenderingMode = BlackPearl::VoxelConeTracingSVORenderer::RenderingMode::VOXELIZATION_VISUALIZATION;
			else if (m_Mode == 1)
				m_CurrentRenderingMode = BlackPearl::VoxelConeTracingSVORenderer::RenderingMode::VOXEL_CONE_TRACING;

		}

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		/*if (BlackPearl::Input::IsKeyPressed(BP_KEY_Y)) {

			m_skybox = m_SkyBoxObj;
			GE_CORE_INFO("m_skybox = m_SkyBoxObj");
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_N)) {

			m_skybox = nullptr;
			GE_CORE_INFO("m_skybox = nullptr");

		}*/
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_R)) {

			m_VoxelConeTracingSVORenderer->RebuildSVO(m_BackGroundObjsList, m_SkyBoxObj);
		}

		m_VoxelConeTracingSVORenderer->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>(), m_BackGroundObjsList, GetLightSources(),
			BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_SkyBoxObj, m_CurrentRenderingMode);



		//if (m_CurrentRenderingMode == BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING) {
		//	glEnable(GL_DEPTH_TEST);
		//	glDepthFunc(GL_LEQUAL);
		//	//m_VoxelizationShader->Bind();

		//	m_BasicRenderer->DrawObject(m_SkyBoxObj);

		//}
		//glDepthFunc(GL_LESS);
		m_BasicRenderer->DrawObject(m_CubeObj);
		m_BasicRenderer->DrawLightSources(GetLightSources());

	}

	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;

	BlackPearl::Object* m_QuadVisualObj;
	BlackPearl::Object* m_QuadGBufferObj;
	BlackPearl::Object* m_QuadFinalScreenObj;
	BlackPearl::Object* m_QuadBRDFLUTObj;
	BlackPearl::Object* m_SurroundSphere;

	BlackPearl::Object* m_DebugQuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_skybox = nullptr;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

	/* Renderer */
	BlackPearl::VoxelConeTracingSVORenderer* m_VoxelConeTracingSVORenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;

	unsigned int m_Mode = 0;
	BlackPearl::VoxelConeTracingSVORenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingSVORenderer::RenderingMode::VOXELIZATION_VISUALIZATION;// VOXEL_CONE_TRACING;//VOXELIZATION_VISUALIZATION
};
