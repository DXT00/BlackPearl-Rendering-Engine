#pragma once

#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <thread>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>

using namespace std::chrono;

class IBLProbesRenderingLayer :public BlackPearl::Layer {
public:

	IBLProbesRenderingLayer(const std::string& name)
		: Layer(name)
	{

		m_MainCamera->SetPosition(math::float3(-2.0f, 0.0f, 14.0f));


		/* MapManager */
		m_MapManager = DBG_NEW BlackPearl::MapManager(BlackPearl::Configuration::MapSize, BlackPearl::Configuration::AreaSize);
		m_MapRenderer = DBG_NEW BlackPearl::MapRenderer(m_MapManager);

	

		/* probe's CubeObj and quad for BrdfLUTMap */
		m_BrdfLUTQuadObj = CreateQuad("assets/shaders/ibl/brdf.glsl", "");
		//m_DebugQuad = CreateQuad("assets/shaders/QuadDebug.glsl","");


		/*gBuffer*/
		m_GBufferScreenQuad = CreateQuad();
		m_GBufferDebugQuad = CreateQuad();
		m_SurroundSphere = CreateSphere(1.0, 128, 128);
		m_GIQuad = CreateQuad();
		/* create probes */

		m_DiffuseLightProbeGrid = CreateProbeGrid(m_MapManager, BlackPearl::ProbeType::DIFFUSE_PROBE, 
			math::float3(4, 2, 4), math::float3(2.0, 2.0, 6.7), 5);
	/*	m_ReflectLightProbeGrid = CreateProbeGrid(m_MapManager, BlackPearl::ProbeType::REFLECTION_PROBE,
			math::float3(2, 1, 1), math::float3(2.0, 6.0, 6.7), 6);*/

		
		BlackPearl::Renderer::Init();
		glEnable(GL_DEPTH_TEST);

		/*Renderer*/
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		m_IBLProbesRenderer = DBG_NEW BlackPearl::IBLProbesRenderer();
		m_AnimatedModelRenderer = DBG_NEW BlackPearl::AnimatedModelRenderer();
		m_GBufferRenderer = DBG_NEW BlackPearl::GBufferRenderer();
		m_ShadowMapPointLightRenderer = DBG_NEW BlackPearl::ShadowMapPointLightRenderer();

		/*create skybox */
		/*notice: draw skybox before anything else!*/
		/*m_SkyBoxObj1 = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});*/
		/*m_SkyBoxObj1 = CreateSkyBox(
			{ "assets/skybox/skybox1/SkyBrightMorning_Right.png",
			 "assets/skybox/skybox1/SkyBrightMorning_Left.png",
			 "assets/skybox/skybox1/SkyBrightMorning_Top.png",
			 "assets/skybox/skybox1/SkyBrightMorning_Bottom.png",
			 "assets/skybox/skybox1/SkyBrightMorning_Front.png",
			 "assets/skybox/skybox1/SkyBrightMorning_Back.png",
			});
		*/
		m_SkyBoxObj1 = CreateSkyBox(
			{ "assets/skybox/skybox1/SkyMorning_Right.png",
			 "assets/skybox/skybox1/SkyMorning_Left.png",
			 "assets/skybox/skybox1/SkyMorning_Top.png",
			 "assets/skybox/skybox1/SkyMorning_Bottom.png",
			 "assets/skybox/skybox1/SkyMorning_Front.png",
			 "assets/skybox/skybox1/SkyMorning_Back.png",
			});

		//LoadStaticBackGroundObject("SphereIron");
	//	LoadScene("SpheresScene");
		LoadSpheresSpecularProbeScene();
	//	LoadScene("Church");
		LoadScene("CornellScene");//SpheresScene
	/*	
		BlackPearl::Object* bot = LoadDynamicObject("Robot");
		BlackPearl::Object* specularProbe = CreateLightProbe(BlackPearl::ProbeType::REFLECTION_PROBE);
		specularProbe->GetComponent<BlackPearl::Transform>()->SetInitPosition(bot->GetComponent<BlackPearl::Transform>()->GetPosition());
		bot->AddChildObj(specularProbe);
		specularProbe->GetComponent<BlackPearl::LightProbe>()->AddExcludeObjectId(bot->GetId().id);*/
		//LoadDynamicObject("Boy");

		m_ProbeCamera = CreateCamera("ProbeCamera");

		/*Draw CubeMap from hdrMap and Create environment IrrdianceMap*/
		m_IBLProbesRenderer->Init(m_ProbeCamera, m_BrdfLUTQuadObj, *GetLightSources(), m_BackGroundObjsList);
		//m_IBLProbesRenderer->Render(GetLightSources(), m_BackGroundObjsList, m_DiffuseLightProbes, m_ReflectionLightProbes, m_SkyBoxObj1);
		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		m_GBufferRenderer->Init(m_GBufferScreenQuad, m_SurroundSphere, m_GIQuad);
	}

	virtual ~IBLProbesRenderingLayer() {

		DestroyObjects();
		delete m_BasicRenderer;
		delete m_IBLProbesRenderer;
		/*for (auto* probe : m_DiffuseLightProbes)
			GE_SAVE_DELETE(probe);
		for (auto* probe : m_ReflectionLightProbes)
			GE_SAVE_DELETE(probe);*/
	}
	void Hello() {
		GE_CORE_INFO("Hello");
	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


	

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();
		
		if (BlackPearl::Input::IsKeyPressed(BlackPearl::KeyCodes::Get(BP_KEY_U))) {
			BlackPearl::TimeCounter::Start();
			//GE_CORE_INFO("updating diffuse probes' area...!")
				m_MapManager->UpdateProbesArea(m_DiffuseLightProbes);
			//	m_IBLProbesRenderer->RenderDiffuseProbeMap(GetLightSources(), m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,
				//	m_DiffuseLightProbes, m_SkyBoxObj1);
			//GE_CORE_INFO("light probe updating......")
				BlackPearl::TimeCounter::End("update Probe area ");

		}
		if (m_FrameIdx % 2 == 0) {
			m_IBLProbesRenderer->RenderDiffuseProbeMap(m_ProbeIdx, GetLightSources(), m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,
						m_DiffuseLightProbes, m_SkyBoxObj1);
			m_ProbeIdx++;
			if (m_ProbeIdx == m_DiffuseLightProbes.size())
					m_ProbeIdx = 0;
		}
		

		bool isSceneChanged = m_ShadowMapPointLightRenderer->JudgeUpdate(GetLightSources(), m_BackGroundObjsList, m_DynamicObjsList);
		if (isSceneChanged || m_loopIndex == 0) {
			BlackPearl::TimeCounter::Start();

#ifdef TIME_DEBUG
			BlackPearl::TimeCounter::Start();
#endif
			m_ShadowMapPointLightRenderer->RenderCubeMap(m_ShadowObjsList, m_DynamicObjsList, runtime / 1000.0f, GetLightSources());
			if (m_loopIndex == 0)
				m_loopIndex++;
#ifdef TIME_DEBUG
			BlackPearl::TimeCounter::End("Render ShadowMap");
#endif
			BlackPearl::TimeCounter::End("Render ShadowMap");

		}
		isSceneChanged = isSceneChanged || (m_FrameIdx % 5 == 0);
		m_IBLProbesRenderer->RenderSpecularProbeMap(isSceneChanged,GetLightSources(), m_BackGroundObjsList,m_DynamicObjsList, runtime / 1000.0f,
		m_ReflectionLightProbes, m_SkyBoxObj1);



		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//m_AnimatedModelRenderer->Render(m_AnimatedModelBoy, runtime / 1000.0f);
		/*m_IBLProbesRenderer->RenderProbes(m_LightProbes);

		m_AnimatedModelRenderer->Render(m_AnimatedModelBoy, runtime / 1000.0f);
		m_AnimatedModelRenderer->Render(m_AnimatedModelCleaner, runtime / 1000.0f);
		m_AnimatedModelRenderer->Render(m_AnimatedModelFrog, runtime / 1000.0f);






;*/

		//	m_GBufferRenderer->Render(m_SphereObjsList,m_GBufferDebugQuad, GetLightSources());//m_BackGroundObjsList
		/*bool updateShadowMap = false;

		for (auto pointlight : GetLightSources()->GetPointLights()) {
			math::float3 position = pointlight->GetComponent<BlackPearl::Transform>()->GetPosition();
			math::float3 lastPosition = pointlight->GetComponent<BlackPearl::Transform>()->GetLastPosition();
			if (position != lastPosition) {
				updateShadowMap = true;
				break;
			}
		}*/


		
	/*	if (m_loopIndex == 0) {

			BlackPearl::TimeCounter::Start();
			m_ShadowMapPointLightRenderer->RenderCubeMap(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f, GetLightSources());
			m_loopIndex++;
			BlackPearl::TimeCounter::End("Render ShadowMap");
		}*/
		m_GBufferRenderer->RenderSceneWithGBufferAndProbes(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,
			m_BackGroundObjsList, m_GBufferDebugQuad, GetLightSources(), m_DiffuseLightProbes, m_ReflectionLightProbes,
			m_IBLProbesRenderer->GetSpecularBrdfLUTTexture(), m_SkyBoxObj1, m_MapManager, nullptr, false);

		if (BlackPearl::Input::IsKeyPressed(BlackPearl::KeyCodes::Get(BP_KEY_L))) {
			m_ShowLightProbe = !m_ShowLightProbe;
		}
		if (BlackPearl::Input::IsKeyPressed(BlackPearl::KeyCodes::Get(BP_KEY_M))) {
			m_ShowMap = !m_ShowMap;
		}
		if (m_ShowLightProbe) {
			m_IBLProbesRenderer->RenderProbes(m_DiffuseLightProbes, 0);
			m_IBLProbesRenderer->RenderProbes(m_ReflectionLightProbes, 1);

		}
		if (m_ShowMap)
			m_MapRenderer->Render(m_MapManager);

		//	

			/*draw BRDFLUTTextureID in a quad*/
			/*glViewport(0, 0, 120, 120);

			m_DebugQuadShader->Bind();
			m_DebugQuad->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_DebugQuadShader);
			m_DebugQuadShader->SetUniform1i("u_BRDFLUTMap", 6);
			glActiveTexture(GL_TEXTURE0 + 6);
			glBindTexture(GL_TEXTURE_2D, m_IBLProbesRenderer->GetSpecularBrdfLUTTexture()->GetRendererID());
			m_BasicRenderer->DrawObject(m_DebugQuad, m_DebugQuadShader);
			m_IBLProbesRenderer->GetSpecularBrdfLUTTexture()->UnBind();

			glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);*/

		m_FrameIdx++;
	}

	void OnAttach() override {


	}

private:
	//Scene

	//BlackPearl::Object* m_DebugQuad = nullptr;
	/* objects */
	//BlackPearl::Object* m_SphereObj = nullptr;
	//BlackPearl::Object* m_SphereObjIron = nullptr;
	//BlackPearl::Object* m_SphereObjRust = nullptr;
	//BlackPearl::Object* m_SphereObjStone = nullptr;
	//BlackPearl::Object* m_SphereObjPlastic = nullptr;

	int m_loopIndex = 0;

	BlackPearl::Object* m_SkyBoxObj1 = nullptr;

	BlackPearl::Object* m_BrdfLUTQuadObj = nullptr;
	//BlackPearl::Object* m_SHImageQuadObj = nullptr;
	BlackPearl::Object* m_DebugQuad = nullptr;


	/* objects list*/
	//std::vector<BlackPearl::Object*> m_SphereObjsList;

	/*gBuffer*/
	BlackPearl::Object* m_GBufferScreenQuad = nullptr;
	BlackPearl::Object* m_GBufferDebugQuad = nullptr;
	BlackPearl::Object* m_GIQuad = nullptr;
	BlackPearl::Object* m_SurroundSphere = nullptr;
	bool m_ShowLightProbe = true;
	bool m_ShowMap = true;

	/*Animation model*/
	//BlackPearl::Object* m_AnimatedModelBoy = nullptr;
	//BlackPearl::Object* m_AnimatedModelCleaner = nullptr;
//	BlackPearl::Object* m_AnimatedModelFrog = nullptr;
	//BlackPearl::Object* m_AnimatedModelRobot = nullptr;
	int m_Rows = 4;
	int m_Colums = 4;
	float m_Spacing = 1.5;



	//Shader
	std::shared_ptr<BlackPearl::Shader> m_BackGroundShader;
	std::shared_ptr<BlackPearl::Shader> m_DebugQuadShader;
	//Renderer
	BlackPearl::IBLProbesRenderer* m_IBLProbesRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	BlackPearl::AnimatedModelRenderer* m_AnimatedModelRenderer;
	BlackPearl::GBufferRenderer* m_GBufferRenderer;
	BlackPearl::ShadowMapPointLightRenderer* m_ShadowMapPointLightRenderer;
	BlackPearl::MapRenderer* m_MapRenderer;
	//Map
	BlackPearl::MapManager* m_MapManager;

	//Light probe 
	BlackPearl::Object* m_DiffuseLightProbeGrid;
	BlackPearl::Object* m_ReflectLightProbeGrid;
	int m_ProbeIdx = 0;
	int m_FrameIdx = 0;

	std::thread m_Threads[10];
	BlackPearl::MainCamera* m_ProbeCamera;

};
