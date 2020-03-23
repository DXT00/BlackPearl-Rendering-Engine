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
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(25.0f));//必须是单位cube
		m_QuadObj = CreateQuad();
		m_QuadBRDFLUTObj = CreateQuad();
		m_VoxelConeTracingRenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,m_QuadObj, m_QuadBRDFLUTObj,m_CubeObj);
		
		m_DebugQuadObj = CreateQuad();
		m_DebugQuadObj->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 0.0f, 1.0f });

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
		LoadScene("SpheresScene");
		
		m_skybox = m_SkyBoxObj;
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
		/*m_FrameNum++;
		GE_ASSERT(m_FrameNum < MAXLONGLONG, "m_FrameNum out of range!");
		m_FPS =(double) m_FrameNum / runtimeSecond;*/

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
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_Y)) {

			m_skybox = m_SkyBoxObj;
			GE_CORE_INFO("m_skybox = m_SkyBoxObj");
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_N)) {

			m_skybox = nullptr;
			GE_CORE_INFO("m_skybox = nullptr");

		}

		/*if (BlackPearl::Input::IsKeyPressed(BP_KEY_V)) {

			BlackPearl::VoxelConeTracingRenderer::s_VoxelizeNow = !BlackPearl::VoxelConeTracingRenderer::s_VoxelizeNow;
			GE_CORE_INFO("vozelize = "+(BlackPearl::VoxelConeTracingRenderer::s_VoxelizeNow)?"true":"false");

		}*/
	m_VoxelConeTracingRenderer->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>(),m_BackGroundObjsList, GetLightSources(), 
		BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_skybox, m_CurrentRenderingMode);


		//m_VoxelConeTracingRenderer->DrawFrontBackFaceOfCube(m_DebugQuadObj);
		/*
		*/
		//glDisable(GL_CULL_FACE);
		/*m_BasicRenderer->DrawObjects(m_BackGroundObjsList);
		m_shader->Bind();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetPosition(BlackPearl::Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetRotation(BlackPearl::Renderer::GetSceneData()->CameraRotation);
		m_BasicRenderer->DrawObject(m_CubeObj, m_shader);*/
		if (m_CurrentRenderingMode == BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXEL_CONE_TRACING) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
		//m_VoxelizationShader->Bind();

			m_BasicRenderer->DrawObject(m_SkyBoxObj);
			
		}
		glDepthFunc(GL_LESS);
		m_BasicRenderer->DrawLightSources(GetLightSources());

	}

	void OnAttach() override {


	}
	
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_QuadBRDFLUTObj;

	BlackPearl::Object* m_DebugQuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_skybox = nullptr;

	std::shared_ptr<BlackPearl::Shader> m_shader;
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::VoxelConeTracingRenderer* m_VoxelConeTracingRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	
	unsigned int m_Mode = 0;
	BlackPearl::VoxelConeTracingRenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingRenderer::RenderingMode::VOXELIZATION_VISUALIZATION;// VOXEL_CONE_TRACING;//VOXELIZATION_VISUALIZATION
};
