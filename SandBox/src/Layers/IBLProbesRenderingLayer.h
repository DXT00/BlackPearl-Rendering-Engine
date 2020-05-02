#pragma once

#include <BlackPeral.h>
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

	IBLProbesRenderingLayer(const std::string& name, BlackPearl::ObjectManager* objectManager)
		: Layer(name, objectManager)
	{

		m_MainCamera->SetPosition(glm::vec3(-2.0f, 0.0f, 14.0f));

	

	
		//Shader
		m_BackGroundShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/ibl/background.glsl"));
		m_DebugQuadShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/QuadDebug.glsl"));

		/* probe's CubeObj and quad for BrdfLUTMap */
		m_BrdfLUTQuadObj = CreateQuad("assets/shaders/ibl/brdf.glsl","");
		m_DebugQuad = CreateQuad("assets/shaders/QuadDebug.glsl","");


		/*gBuffer*/
		m_GBufferScreenQuad = CreateQuad();
		m_GBufferDebugQuad = CreateQuad();
		m_SurroundSphere = CreateSphere(1.0, 128, 128);
		m_GIQuad = CreateQuad();
		/* create probes */
		unsigned int xlen = 4 ,ylen = 2,zlen = 4,space =5;//424
		float offsetx =2.0f, offsety = 4.0f, offsetz = 6.7f;//offsetx = 0.0, offsety = 5.0f, offsetz = 1.0f;
		for (unsigned int x = 0; x < xlen; x++)
		{
			for (unsigned int y = 0; y < ylen; y++)
			{
				for (unsigned int  z = 0; z < zlen; z++)
				{
					BlackPearl::LightProbe* probe = CreateLightProbe(BlackPearl::LightProbe::Type::DIFFUSE);
					int xx = (x - xlen / 2) * space, yy = (y - ylen / 2) * space, zz = (z - zlen / 2) * space;
					
					probe->SetPosition({ offsetx+xx,offsety+yy,offsetz+zz});
					m_DiffuseLightProbes.push_back(probe);
					//delete probe;

				}
				
			}

		}
		xlen = 2, ylen = 1, zlen = 1, space = 5;//424
		offsetx = 2.0f, offsety = 4.0f, offsetz = 6.7f;//offsetx = 0.0, offsety = 5.0f, offsetz = 1.0f;
		for (unsigned int x = 0; x < xlen; x++)
		{
			for (unsigned int y = 0; y < ylen; y++)
			{
				for (unsigned int z = 0; z < zlen; z++)
				{
					BlackPearl::LightProbe* probe = CreateLightProbe(BlackPearl::LightProbe::Type::REFLECTION);
					int xx = (x - xlen / 2) * space, yy = (y - ylen / 2) * space, zz = (z - zlen / 2) * space;

					probe->SetPosition({ offsetx + xx,offsety + yy,offsetz + zz });
					m_ReflectionLightProbes.push_back(probe);
					//delete probe;

				}

			}

		}
	

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
		m_SkyBoxObj1 = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});
		  
		//LoadScene("Church");

		LoadScene("SpheresScene");
		LoadScene("CornellScene");//SpheresScene
		//LoadDynamicObject("Robot");


		/*Draw CubeMap from hdrMap and Create environment IrrdianceMap*/
		m_IBLProbesRenderer->Init(m_BrdfLUTQuadObj,  *GetLightSources(), m_BackGroundObjsList, m_DiffuseLightProbes);
		//m_IBLProbesRenderer->Render(GetLightSources(), m_BackGroundObjsList, m_LightProbes, m_SkyBoxObj1);

		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		m_GBufferRenderer->Init(m_GBufferScreenQuad, m_SurroundSphere, m_GIQuad);
	}

	virtual ~IBLProbesRenderingLayer() {

		DestroyObjects();
		delete m_BasicRenderer;
		delete m_IBLProbesRenderer;

	}
	void Hello() {
		GE_CORE_INFO("Hello");
	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {

			GE_CORE_INFO("light probe updating......")
			m_IBLProbesRenderer->Render(GetLightSources(), m_BackGroundObjsList, m_DiffuseLightProbes, m_ReflectionLightProbes,m_SkyBoxObj1);
			GE_CORE_INFO("light probe update finished!")

		}

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();





		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//m_AnimatedModelRenderer->Render(m_AnimatedModelBoy, runtime / 1000.0f);
		/*m_IBLProbesRenderer->RenderProbes(m_LightProbes);

		m_AnimatedModelRenderer->Render(m_AnimatedModelBoy, runtime / 1000.0f);
		m_AnimatedModelRenderer->Render(m_AnimatedModelCleaner, runtime / 1000.0f);
		m_AnimatedModelRenderer->Render(m_AnimatedModelFrog, runtime / 1000.0f);







		m_BasicRenderer->RenderScene(m_BackGroundObjsList, GetLightSources());
		glDepthFunc(GL_LEQUAL);

		m_IBLProbesRenderer->DrawObject(m_SkyBoxObj1);
		glDepthFunc(GL_LESS);

		m_IBLProbesRenderer->RenderSpecularObjects(GetLightSources(), m_SphereObjsList, m_LightProbes);*/

		//	m_GBufferRenderer->Render(m_SphereObjsList,m_GBufferDebugQuad, GetLightSources());//m_BackGroundObjsList
	/*	bool updateShadowMap = false;

		for (auto pointlight : GetLightSources()->GetPointLights()) {
			glm::vec3 position = pointlight->GetComponent<BlackPearl::Transform>()->GetPosition();
			glm::vec3 lastPosition = pointlight->GetComponent<BlackPearl::Transform>()->GetLastPosition();
			if (position != lastPosition) {
				updateShadowMap = true;
				break;
			}
		}
		if (updateShadowMap == true || m_loopIndex == 0) {
			m_ShadowMapPointLightRenderer->RenderCubeMap(m_ShadowObjsList, m_DynamicObjsList, runtime / 1000.0f, GetLightSources());
			if (m_loopIndex == 0)
				m_loopIndex++;
		}*/

		m_ShadowMapPointLightRenderer->RenderCubeMap(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f, GetLightSources());

		m_GBufferRenderer->RenderSceneWithGBufferAndProbes(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,
			m_BackGroundObjsList, m_GBufferDebugQuad, GetLightSources(), m_DiffuseLightProbes, m_ReflectionLightProbes,
			m_IBLProbesRenderer->GetSpecularBrdfLUTTexture(), m_SkyBoxObj1);

		if (BlackPearl::Input::IsKeyPressed(BP_KEY_L)) {
			m_ShowLightProbe = !m_ShowLightProbe;
		}
		if (m_ShowLightProbe) {
			m_IBLProbesRenderer->RenderProbes(m_DiffuseLightProbes,0);
			m_IBLProbesRenderer->RenderProbes(m_ReflectionLightProbes,1);

		}

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
	std::vector<BlackPearl::Object*> m_SphereObjsList;

	/*gBuffer*/
	BlackPearl::Object* m_GBufferScreenQuad = nullptr;
	BlackPearl::Object* m_GBufferDebugQuad = nullptr;
	BlackPearl::Object* m_GIQuad = nullptr;
	BlackPearl::Object* m_SurroundSphere = nullptr;
	bool m_ShowLightProbe = true;
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


	/* Probes */
	std::vector<BlackPearl::LightProbe*> m_DiffuseLightProbes;
	std::vector<BlackPearl::LightProbe*> m_ReflectionLightProbes;

	std::thread m_Threads[10];

	
};
