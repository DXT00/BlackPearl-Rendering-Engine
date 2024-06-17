#pragma once

#include <BlackPearl.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BlackPearl/Component/LightComponent/Light.h"
#include <chrono>
using namespace std::chrono;

class ShadowMapPointLightLayer :public BlackPearl::Layer {

public:

	ShadowMapPointLightLayer(const std::string& name)
		: Layer(name)
	{

		
		m_StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		//m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());


		m_ShadowMapPointLightRenderer = DBG_NEW BlackPearl::ShadowMapPointLightRenderer();
		m_BacisRender = DBG_NEW BlackPearl::BasicRenderer();
		m_AnimatedModelRenderer = DBG_NEW BlackPearl::AnimatedModelRenderer();
		BlackPearl::Renderer::Init();


		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试
		glEnable(GL_CULL_FACE);
			//Shader reset
		//m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/pointLight/Quad_ShadowMapLayer.glsl"));


		BlackPearl::Object* light1 = CreateLight(BlackPearl::LightType::PointLight);
		BlackPearl::Object* light2 = CreateLight(BlackPearl::LightType::PointLight);
		light2->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f,0.0f,-2.4f });
		light2->GetComponent<BlackPearl::PointLight>()->SetDiffuse({ 1.0f,0.0f,0.0f });


		Room = CreateCube();
		//std::shared_ptr<BlackPearl::Texture> RoomTexture;
		//RoomTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png", GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
		//Room->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RoomTexture);
		//Room->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetEnableCullFace(false);
		Room->GetComponent<BlackPearl::Transform>()->SetScale({ 30.0f,0.5f,30.0f });
		Room->GetComponent<BlackPearl::Transform>()->SetPosition({0.0f,-7.0f,0.0f });

		m_BackGroundObjsList.push_back(Room);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);

		//m_AnimatedModelBoy = CreateModel("assets/models-animation/people/character Texture.dae", "assets/shaders/animatedModel/animatedModel.glsl", true, "Boy");
		//m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2f,0.2f,0.2f });
		//m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetRotation({ -90.0f,0.0f,0.0f });
		//m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetPosition({ 3.0f,-1.6f,0.0f });
		//
		//m_DynamicObjsList.push_back(m_AnimatedModelBoy);
		m_MainCamera->SetPosition({ 0.0,0.0,50.0 });
		m_CameraPosition = m_MainCamera->GetPosition();//cameraComponent->GetPosition();
		m_MainCamera->SetMoveSpeed(20.0);
	

		BlackPearl::Object* Cube = CreateCube();
		Cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, -3.5f, -0.0f });
		Cube->GetComponent<BlackPearl::Transform>()->SetRotation({ 10.0f, 70.0f, 6.0f });
		m_BackGroundObjsList.push_back(Cube);
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);
		

		Deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false, "Deer");
		Deer->GetComponent<BlackPearl::Transform>()->SetInitScale(glm::vec3(0.01));
		Deer->GetComponent<BlackPearl::Transform>()->SetInitPosition({ -0.5f,-3.8f,4.5f });
		Deer->GetComponent<BlackPearl::Transform>()->SetInitRotation({ 0.0f,68.0f,0.0f });

		m_BackGroundObjsList.push_back(Deer);
		Deer->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);

		m_Sword = CreateModel("assets/models/sword/OBJ/Big_Sword_OBJ.obj", "assets/shaders/pbr/PbrTexture.glsl", false, "Sword");
		std::shared_ptr<BlackPearl::Texture> SwordalbedoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/models/sword/textures/Big Sword_Base_Color_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordaoTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::AoMap, "assets/models/sword/textures/Big Sword_AO_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordroughnessTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::RoughnessMap, "assets/models/sword/textures/Big Sword_Roughness_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordmentallicTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::MentallicMap, "assets/models/sword/textures/Big Sword_Metalness.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordnormalTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::NormalMap, "assets/models/sword/textures/Big Sword_Normal_Map.jpg"));
		std::shared_ptr<BlackPearl::Texture> SwordemissionTexture(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::EmissionMap, "assets/models/sword/textures/Big Sword_Emission_Map.jpg"));

		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordalbedoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordaoTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordroughnessTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordmentallicTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordnormalTexture);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(SwordemissionTexture);

		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetPBRTextureSamples(true);
		m_Sword->GetComponent<BlackPearl::MeshRenderer>()->SetIsPBRObject(true);
		m_BackGroundObjsList.push_back(m_Sword);


		Quad = CreateQuad();
	/*	Quad->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer.GetShadowMapPointLightRenderer().GetFrameBuffer()->GetCubeMapDepthTexture());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/shadowMap/pointLight/QuadCubeMap.glsl");
*/


		/*for (BlackPearl::Object* obj : m_BackGroundObjsList) {
			if (obj->HasComponent<BlackPearl::MeshRenderer>()) {
				auto cubeMapBuffer = m_ShadowMapPointLightRenderer->GetFrameBuffer();
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(cubeMapBuffer->GetCubeMapDepthTexture());
			}
				
		}*/

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);


	}

	virtual ~ShadowMapPointLightLayer() {

		DestroyObjects();
		GE_SAVE_DELETE(m_ShadowMapPointLightRenderer);
		GE_SAVE_DELETE(m_BacisRender);
		GE_SAVE_DELETE(m_AnimatedModelRenderer);


	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();

	
		m_ShadowMapPointLightRenderer->RenderCubeMap(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,GetLightSources());

		m_ShadowMapPointLightRenderer->Render(m_BackGroundObjsList, GetLightSources());

		//m_AnimatedModelRenderer->Render(m_AnimatedModelBoy, runtime / 1000.0f);
	}


	
	void OnAttach() override {


	}

private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_PointLight;

	BlackPearl::Object* Quad;
	BlackPearl::Object* Room;
	BlackPearl::Object* Deer;

	BlackPearl::Object* m_SkyBox;
	BlackPearl::Object* m_Sword = nullptr;

	BlackPearl::Object* m_AnimatedModelBoy = nullptr;
	std::vector<BlackPearl::Object*> m_DynamicObjsList;
	/*Time*/
	std::chrono::milliseconds m_StartTimeMs;

	/*Renderer */
	BlackPearl::BasicRenderer* m_BacisRender;
	BlackPearl::ShadowMapPointLightRenderer* m_ShadowMapPointLightRenderer;
	BlackPearl::AnimatedModelRenderer* m_AnimatedModelRenderer;


	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };

	glm::vec3 m_LightPos = { 0.0f, 1.5f, 0.0f };
	std::shared_ptr<BlackPearl::Shader> m_QuadDepthShader;


	GLuint woodTexture;
	GLuint planeVAO;
	GLuint depthMapFBO;
	GLuint depthMap;
	GLfloat near_plane = 1.0f, far_plane = 7.5f;
};
