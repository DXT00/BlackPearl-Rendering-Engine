#pragma once

#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
class IBLRenderingLayer :public BlackPearl::Layer {
public:

	IBLRenderingLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		
		//Shader
		//m_BackGroundShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/ibl/background.glsl"));
	//	m_DebugQuadShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/QuadDebug.glsl"));
		m_LightProbeShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/lightProbes/lightProbe.glsl"));

		//Scene
	
		//skybox and quad for BrdfLUTMap
		m_CubeObj = CreateCube();
		m_CubeObj->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2,0.2,0.2 });
		//m_CubeProbeObj = CreateCube();
		m_BrdfLUTQuadObj = CreateQuad();
		//m_DebugQuad = CreateQuad();


		BlackPearl::Renderer::Init();
		//glDepthFunc(GL_LEQUAL);
		//Renderer
		m_BasicRenderer = DBG_NEW BlackPearl::BasicRenderer();
		//m_PBRRenderer = DBG_NEW BlackPearl::PBRRenderer();
		m_IBLRenderer = DBG_NEW BlackPearl::IBLRenderer();
		m_ShadowMapPointLightRenderer = DBG_NEW BlackPearl::ShadowMapPointLightRenderer();

		m_SkyBoxObj = CreateSkyBox(
			{ "assets/skybox/skybox/right.jpg",
			 "assets/skybox/skybox/left.jpg",
			 "assets/skybox/skybox/top.jpg",
			 "assets/skybox/skybox/bottom.jpg",
			 "assets/skybox/skybox/front.jpg",
			 "assets/skybox/skybox/back.jpg",
			});

		LoadSpheresScene();
		LoadCornellScene();
		//LoadChurchScene();
		

	//	LoadStaticBackGroundObject("Sphere");

		/*Draw CubeMap from hdrMap and Create environment IrrdianceMap*/
		m_IBLRenderer->Init(m_CubeObj, m_BrdfLUTQuadObj,m_SkyBoxObj,m_BackGroundObjsList , GetLightSources());

		//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

	}

	virtual ~IBLRenderingLayer() {

		DestroyObjects();
		delete m_BasicRenderer;
		delete m_IBLRenderer;
		//delete m_PBRRenderer;

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		/*render scene*/
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		/*Draw Lights*/
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_U)) {

			GE_CORE_INFO("cube map updating......")
			m_IBLRenderer->Init(m_CubeObj, m_BrdfLUTQuadObj, m_SkyBoxObj, m_BackGroundObjsList, GetLightSources());

		}

		
		/*IBL rendering*/
		//m_IBLRenderer->RenderSpheres(m_SphereObj);

		/*m_IBLRenderer->RenderTextureSphere(m_SphereObjIron);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjRust);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjStone);
		m_IBLRenderer->RenderTextureSphere(m_SphereObjPlastic);*/
		//m_IBLRenderer->RenderTextureSphere(m_Sword);
		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();
		
		m_ShadowMapPointLightRenderer->RenderCubeMap(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f, GetLightSources());
		m_IBLRenderer->RenderTextureSphere(m_BackGroundObjsList,GetLightSources());



		/*Draw SkyBox*/
		//m_BackGroundShader->Bind();
		//m_BackGroundShader->SetUniform1i("cubeMap", 0);
		//glActiveTexture(GL_TEXTURE0);
		////m_IBLRenderer->GetHdrCubeMap()->Bind();
		//m_IBLRenderer->GetSkyBoxCubeMap()->Bind();
		//m_BasicRenderer->DrawObject(m_CubeObj, m_BackGroundShader);
		//




		glDepthFunc(GL_LEQUAL);

		m_BasicRenderer->DrawObject(m_SkyBoxObj);
		glDepthFunc(GL_LESS);

		m_BasicRenderer->DrawLightSources(GetLightSources());


		//m_LightProbeShader->Bind();
		////BlackPearl::Object* cube = CreateCube();
		//m_LightProbeShader->SetUniform1i("u_Material.cube", 2);
		//m_LightProbeShader->SetUniform1i("uProbeType", 1);

		//glActiveTexture(GL_TEXTURE2);
		////m_IBLRenderer->GetSpecularCubeMap()->Bind();
		//m_IBLRenderer->GetIrradianceCubeMap()->Bind();
		//m_BasicRenderer->DrawObject(m_CubeProbeObj, m_LightProbeShader);
	



		///*draw BRDFLUTTextureID in a quad*/
		//glViewport(0, 0, 240, 240);
		//m_DebugQuadShader->Bind();
		//m_DebugQuadShader->SetUniform1i("u_BRDFLUTMap",5);
		//glActiveTexture(GL_TEXTURE0 + 5);
		//glBindTexture(GL_TEXTURE_2D, m_IBLRenderer->GetBRDFLUTTextureID());
		//m_BasicRenderer->DrawObject(m_DebugQuad, m_DebugQuadShader);


		//m_IBLRenderer->DrawBRDFLUTMap();

	}

	void OnAttach() override {


	}

private:
	//Scene
	BlackPearl::Object* m_CubeObj = nullptr;
	BlackPearl::Object* m_DebugQuad = nullptr;
	BlackPearl::Object* m_SphereObj = nullptr;
	BlackPearl::Object* m_SphereObjIron = nullptr;
	BlackPearl::Object* m_SphereObjRust = nullptr;
	BlackPearl::Object* m_SphereObjStone = nullptr;
	BlackPearl::Object* m_SphereObjPlastic = nullptr;
	BlackPearl::Object* m_Sword=nullptr;
	BlackPearl::Object* m_SkyBoxObj = nullptr;

	BlackPearl::Object* m_BrdfLUTQuadObj = nullptr;
	int m_Rows = 4;
	int m_Colums = 4;
	float m_Spacing = 1.5;

	//Shader
	std::shared_ptr<BlackPearl::Shader> m_BackGroundShader;
	std::shared_ptr<BlackPearl::Shader> m_DebugQuadShader;
	std::shared_ptr<BlackPearl::Shader> m_LightProbeShader = nullptr;
	//Renderer
	//BlackPearl::PBRRenderer* m_PBRRenderer;
	BlackPearl::IBLRenderer* m_IBLRenderer;
	BlackPearl::BasicRenderer* m_BasicRenderer;
	BlackPearl::ShadowMapPointLightRenderer* m_ShadowMapPointLightRenderer;

	//lightProbe
	BlackPearl::Object* m_CubeProbeObj = nullptr;


};
