#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class VoxelConeTracingDeferredLayer :public BlackPearl::Layer {
public:

	VoxelConeTracingDeferredLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{


		m_CubeObj = CreateCube();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale(glm::vec3(20.0f));//必须是单位cube
		m_QuadObj = CreateQuad();
		m_QuadBRDFLUTObj = CreateQuad();
		m_QuadGBufferObj = CreateQuad();
		m_QuadFinalScreenObj = CreateQuad();
		m_SurroundSphereObj = CreateSphere(1.0, 64, 64, "assets/shaders/Sphere.glsl", "", "SurroundSphere");

		glEnable(GL_MULTISAMPLE);

		// detect current settings
		GLint iMultiSample = 0;
		GLint iNumSamples = 0;
		glGetIntegerv(GL_SAMPLE_BUFFERS, &iMultiSample);
		glGetIntegerv(GL_SAMPLES, &iNumSamples);

		GE_CORE_INFO("MSAA on, GL_SAMPLE_BUFFERS ={0}, GL_SAMPLES = {1}", iMultiSample, iNumSamples);
	

		/***************************************** Scene ********************************************************/
		m_SkyBoxObj = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
	

		LoadScene("CornellScene");
		//LoadChurchScene();
		LoadScene("SpheresScene");

		m_skybox = m_SkyBoxObj;
		/********************************************* Renderer **********************************************************/

		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_VoxelConeTracingDeferredRenderer = DBG_NEW BlackPearl::VoxelConeTracingDeferredRenderer();
		BlackPearl::Renderer::Init();
		m_VoxelConeTracingDeferredRenderer->Init(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight,
			m_QuadObj, m_QuadGBufferObj, m_QuadBRDFLUTObj, m_QuadFinalScreenObj, m_SurroundSphereObj, m_CubeObj);



	}

	virtual ~VoxelConeTracingDeferredLayer() {
		GE_SAVE_DELETE(m_VoxelConeTracingDeferredRenderer);
		GE_SAVE_DELETE(m_BasicRenderer);
	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;

		//Switch mode
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {
			m_Mode = (m_Mode + 1) % 2;
			if (m_Mode == 0)
				m_CurrentRenderingMode = BlackPearl::VoxelConeTracingDeferredRenderer::RenderingMode::VOXELIZATION_VISUALIZATION;
			else if (m_Mode == 1)
				m_CurrentRenderingMode = BlackPearl::VoxelConeTracingDeferredRenderer::RenderingMode::VOXEL_CONE_TRACING;

		}
		// render
	

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

		m_VoxelConeTracingDeferredRenderer->Render(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>(), m_BackGroundObjsList, GetLightSources(),
			BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, m_skybox, m_CurrentRenderingMode);
	
		m_BasicRenderer->DrawLightSources(GetLightSources());

	}

	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_IronManObj = nullptr;
	BlackPearl::Object* m_QuadObj = nullptr;
	BlackPearl::Object* m_QuadBRDFLUTObj = nullptr;

	BlackPearl::Object* m_QuadGBufferObj = nullptr;
	BlackPearl::Object* m_QuadFinalScreenObj = nullptr;

	BlackPearl::Object* m_SurroundSphereObj = nullptr;

	BlackPearl::Object* m_PlaneObj = nullptr;
	BlackPearl::Object* m_CubeObj = nullptr;
	BlackPearl::Object* m_SkyBoxObj = nullptr;
	BlackPearl::Object* m_skybox = nullptr;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::VoxelConeTracingDeferredRenderer* m_VoxelConeTracingDeferredRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;

	unsigned int m_Mode = 0;
	BlackPearl::VoxelConeTracingDeferredRenderer::RenderingMode m_CurrentRenderingMode = BlackPearl::VoxelConeTracingDeferredRenderer::RenderingMode::VOXEL_CONE_TRACING;// VOXELIZATION_VISUALIZATION;// VOXEL_CONE_TRACING;//VOXELIZATION_VISUALIZATION
};
