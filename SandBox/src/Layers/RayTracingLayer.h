#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class RayTracingLayer :public BlackPearl::Layer {
public:

	RayTracingLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{


		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_RayTracingRenderer = DBG_NEW BlackPearl::RayTracingRenderer();
		
		m_QuadObj = CreateQuad();
		m_RayTracingRenderer->Init( m_QuadObj);

		
		m_MainCamera->SetFov(90.0f);
		m_MainCamera->SetPosition(glm::vec3(0, 0, 0));//glm::vec3(0,1.387f,22.012f)
		m_CameraPosition = m_MainCamera->GetPosition();
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
		m_skybox = m_SkyBoxObj;
		/*******************************************************************************************************/
		/*******************************************************************************************************/





	}

	virtual ~RayTracingLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtimeSecond = (currentTimeMs.count() - m_StartTimeMs.count()) / 1000.0f;
	



		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_Y)) {

			m_skybox = m_SkyBoxObj;
			GE_CORE_INFO("m_skybox = m_SkyBoxObj");
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_N)) {

			m_skybox = nullptr;
			GE_CORE_INFO("m_skybox = nullptr");

		}

		m_RayTracingRenderer->RenderSpheres(m_MainCamera);
	//	m_RayTracingRenderer->Render();


	

	}

	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_QuadObj;

	BlackPearl::Object* m_SkyBoxObj;
	BlackPearl::Object* m_skybox = nullptr;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };


	BlackPearl::RayTracingRenderer* m_RayTracingRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;


};
