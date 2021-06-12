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

	ShadowMapPointLightLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		
		m_StartTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

		//m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_MainCamera->GetObj());


		m_ShadowMapPointLightRenderer = DBG_NEW BlackPearl::ShadowMapPointLightRenderer();
		m_BacisRender = DBG_NEW BlackPearl::BasicRenderer();

		BlackPearl::Renderer::Init();


		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试
		glEnable(GL_CULL_FACE);
			//Shader reset
		//m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/pointLight/Quad_ShadowMapLayer.glsl"));


		BlackPearl::Object* light1 = CreateLight(BlackPearl::LightType::PointLight);
		BlackPearl::Object* light2 = CreateLight(BlackPearl::LightType::PointLight);
		light2->GetComponent<BlackPearl::Transform>()->SetPosition({ 5.0f,0.0f,0.0f });
		light2->GetComponent<BlackPearl::PointLight>()->SetDiffuse({ 1.0f,0.0f,0.0f });


		Room = CreateCube();
		//std::shared_ptr<BlackPearl::Texture> RoomTexture;
		//RoomTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png", GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
		//Room->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RoomTexture);
		//Room->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetEnableCullFace(false);
		Room->GetComponent<BlackPearl::Transform>()->SetScale({ 10.0f,0.5f,10.0f });
		Room->GetComponent<BlackPearl::Transform>()->SetPosition({0.0f,-7.0f,0.0f });

		m_BackGroundObjsList.push_back(Room);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);

		m_AnimatedModelBoy = CreateModel("assets/models-animation/people/character Texture.dae", "assets/shaders/animatedModel/animatedModel.glsl", true, "Boy");
		m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetScale({ 0.2f,0.2f,0.2f });
		m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetRotation({ -90.0f,0.0f,0.0f });
		m_AnimatedModelBoy->GetComponent<BlackPearl::Transform>()->SetPosition({ 3.0f,-1.6f,0.0f });
		
		m_DynamicObjsList.push_back(m_AnimatedModelBoy);

	/*	std::shared_ptr<BlackPearl::Texture> CubeTexture;
		CubeTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png", GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));

		std::shared_ptr<BlackPearl::Texture> CubeTexture1;
		CubeTexture1.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wall4.jpg", GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));*/

		BlackPearl::Object* Cube = CreateCube();
		//Cube->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, -3.5f, -0.0f });
		Cube->GetComponent<BlackPearl::Transform>()->SetRotation({ 10.0f, 70.0f, 6.0f });
	//	Cube->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture);
		m_BackGroundObjsList.push_back(Cube);
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetIsBackGroundObjects(true);

		/*BlackPearl::Object* Cube5 = CreateCube();
		Cube5->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube5->GetComponent<BlackPearl::Transform>()->SetPosition({ -2.0f, -3.5f, 3.0f });
		Cube5->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture1);*/

	/*	BlackPearl::Object* Cube1 = CreateCube();
		Cube1->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube1->GetComponent<BlackPearl::Transform>()->SetPosition({ 2.0f, 0.0f, 6.0f });
		Cube1->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture);


		BlackPearl::Object* Cube2 = CreateCube();
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube2->GetComponent<BlackPearl::Transform>()->SetPosition({ -2.0f, 0.0f, 6.0f });
		Cube2->GetComponent<BlackPearl::Transform>()->SetRotation({ 30.0f, 0.0f, 6.0f });
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture1);

		BlackPearl::Object* Cube3 = CreateCube();
		Cube3->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube3->GetComponent<BlackPearl::Transform>()->SetPosition({ 4.0f, 0.0f, -2.0f });
		Cube3->GetComponent<BlackPearl::Transform>()->SetRotation({ 0.0f, 50.0f, 60.0f });
		Cube3->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture);

		BlackPearl::Object* Cube4 = CreateCube();
		Cube4->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube4->GetComponent<BlackPearl::Transform>()->SetPosition({ -4.0f, 0.0f, -6.0f });*/

		/*BlackPearl::Object* Cube6 = CreateCube();
		Cube6->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube6->GetComponent<BlackPearl::Transform>()->SetPosition({ -4.0f, -18.0f, -6.0f });
		Cube6->GetComponent<BlackPearl::Transform>()->SetScale({ 2.0f,2.0f,2.0f });
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture1);*/

		Deer = CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false);
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

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);




		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		milliseconds currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		double runtime = currentTimeMs.count() - m_StartTimeMs.count();

	
		m_ShadowMapPointLightRenderer->RenderCubeMap(m_BackGroundObjsList, m_DynamicObjsList, runtime / 1000.0f,GetLightSources());

		m_ShadowMapPointLightRenderer->Render(m_BackGroundObjsList, GetLightSources());





	}



	//void OnImguiRender() override {


	//	ImGui::Begin("Settings");
	//	ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
	//	ImGui::End();

	//	static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

	//	if (ImGui::CollapsingHeader("Create")) {

	//		const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Deer","Cube","Plane" };
	//		static int entityIdx = -1;
	//		if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 8))
	//		{
	//			switch (entityIdx)
	//			{
	//			case 0:
	//				GE_CORE_INFO("Creating Empty...");
	//				Layer::CreateEmpty();
	//				break;
	//			case 1:
	//				GE_CORE_INFO("Creating PointLight...");
	//				Layer::CreateLight(BlackPearl::LightType::ParallelLight);
	//				break;
	//			case 2:
	//				GE_CORE_INFO("Creating PointLight...");
	//				Layer::CreateLight(BlackPearl::LightType::PointLight);
	//				break;
	//			case 3:
	//				GE_CORE_INFO("Creating SpotLight ...");
	//				Layer::CreateLight(BlackPearl::LightType::SpotLight);
	//				break;
	//			case 4:
	//				GE_CORE_INFO("Creating IronMan ...");
	//				//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
	//				//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
	//				//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
	//				Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/IronMan.glsl",false);

	//				break;
	//			case 5:
	//				GE_CORE_INFO("Creating Deer ...");
	//				//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
	//				//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
	//				//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
	//				Layer::CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl",false);

	//				break;
	//			case 6:
	//				GE_CORE_INFO("Creating Cube ...");
	//				Layer::CreateCube();
	//				break;
	//			case 7:
	//				GE_CORE_INFO("Creating Plane ...");
	//				Layer::CreatePlane();
	//				break;
	//			}
	//		}
	//	}
	//	if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
	//	{
	//		if (ImGui::BeginTabItem("Scene")) {
	//			std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
	//			ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 8);

	//			for (int n = 0; n < objsList.size(); n++) {
	//				//ImGui::Text("%s", objsList[n].c_str());
	//				bool is_selected = (currentObj != nullptr && currentObj->ToString() == objsList[n]->ToString());
	//				if (ImGui::Selectable(objsList[n]->ToString().c_str(), is_selected)) {
	//					currentObj = objsList[n];
	//					GE_CORE_INFO(objsList[n]->ToString() + "is selected")
	//				}

	//				if (is_selected)
	//					ImGui::SetItemDefaultFocus();
	//			}
	//			ImGui::ListBoxFooter();
	//			ImGui::EndTabItem();
	//		}
	//		//}
	//	}
	//	ImGui::EndTabBar();

	//	////////////////////Inspector/////////////////////////
	//	ImGui::Begin("Inspector");


	//	ImGui::DragFloat("far_plane", &BlackPearl::ShadowMapPointLightRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
	//	ImGui::DragFloat("pointLight_FOV", &BlackPearl::ShadowMapPointLightRenderer::s_FOV, 0.5f,  0.0f, 180.0f, "%.3f ");

	//	if (currentObj != nullptr) {

	//		//std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

	//		//for (auto pair : componentList) {
	//		//	auto component = pair.second;
	//		//	if (component != nullptr) {

	//		//		switch (component->GetType()) {
	//		//		case BlackPearl::BaseComponent::Type::MeshRenderer: {
	//		//			std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
	//		//			ShowMeshRenderer(comp);
	//		//			break;
	//		//		}
	//		//		case BlackPearl::BaseComponent::Type::Transform: {
	//		//			std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
	//		//			ShowTransform(comp);
	//		//			break;
	//		//		}
	//		//		case BlackPearl::BaseComponent::Type::Light: {


	//		//			std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
	//		//			if (comp->GetType() == BlackPearl::LightType::PointLight) {
	//		//			//	std::shared_ptr<BlackPearl::PointLight> pointlight = std::dynamic_pointer_cast<BlackPearl::PointLight>(comp);
	//		//				ShowPointLight(comp);

	//		//			}
	//		//			break;
	//		//		}
	//		//		default:
	//		//			break;
	//		//		}

	//		//	}

	//		//}
	//		if (currentObj->HasComponent< BlackPearl::Transform>()) {
	//			ShowTransform(currentObj->GetComponent<BlackPearl::Transform>());

	//		}
	//		if (currentObj->HasComponent< BlackPearl::MeshRenderer>()) {
	//			ShowMeshRenderer(currentObj->GetComponent<BlackPearl::MeshRenderer>());

	//		}
	//		if (currentObj->HasComponent < BlackPearl::PointLight>()) {
	//			ShowPointLight(currentObj->GetComponent<BlackPearl::PointLight>());
	//		}
	//	}

	//	ImGui::End();



	//	m_fileDialog.Display();




	//}
	//
	
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
	//BlackPearl::MasterRenderer* m_MasterRenderer;


	//BlackPearl::Object* m_IronMan;
//BlackPearl::Object* m_Quad;
//BlackPearl::Object* m_Plane;
//BlackPearl::Object* m_Cube;
//BlackPearl::Object* m_Cube1;
//BlackPearl::Object* m_Cube2;
	
	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };




	glm::vec3 m_LightPos = { 0.0f, 1.5f, 0.0f };
	std::shared_ptr<BlackPearl::Shader> m_QuadDepthShader;


	GLuint woodTexture;
	GLuint planeVAO;
	GLuint depthMapFBO;
	GLuint depthMap;
	GLfloat near_plane = 1.0f, far_plane = 7.5f;
};
