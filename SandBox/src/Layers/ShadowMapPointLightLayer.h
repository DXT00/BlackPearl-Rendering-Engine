#pragma once

#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "BlackPearl/Component/LightComponent/Light.h"

class ShadowMapPointLightLayer :public BlackPearl::Layer {

public:

	ShadowMapPointLightLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();

		m_MasterRenderer = DBG_NEW BlackPearl::MasterRenderer(m_CameraObj);

		BlackPearl::Renderer::Init();

		//	m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(960, 540, {BlackPearl::FrameBuffer::Attachment::DepthTexture },false));

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试
		glEnable(GL_CULL_FACE);
			//Shader reset
		m_Shader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/pointLight/CubeMapShadowMapping.glsl"));
		m_SimpleDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/pointLight/CubeMapDepthShader.glsl"));
		//m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/pointLight/Quad_ShadowMapLayer.glsl"));


		m_PointLight = CreateLight(BlackPearl::LightType::PointLight);

		Room = CreateCube();
		std::shared_ptr<BlackPearl::Texture> RoomTexture;
		RoomTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png"));
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(RoomTexture);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Room->GetComponent<BlackPearl::MeshRenderer>()->SetEnableCullFace(false);
		Room->GetComponent<BlackPearl::Transform>()->SetScale({ 50.0f,50.0f,50.0f });

		
		std::shared_ptr<BlackPearl::Texture> CubeTexture;
		CubeTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wood.png"));

		std::shared_ptr<BlackPearl::Texture> CubeTexture1;
		CubeTexture1.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::DiffuseMap, "assets/texture/wall4.jpg"));

		BlackPearl::Object* Cube = CreateCube();
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 2.0f, -3.5f, -4.0f });
		Cube->GetComponent<BlackPearl::Transform>()->SetRotation({ 10.0f, 70.0f, 6.0f });
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture);

		/*BlackPearl::Object* Cube5 = CreateCube();
		Cube5->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube5->GetComponent<BlackPearl::Transform>()->SetPosition({ -2.0f, -3.5f, 3.0f });
		Cube5->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture1);*/

		BlackPearl::Object* Cube1 = CreateCube();
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
		Cube4->GetComponent<BlackPearl::Transform>()->SetPosition({ -4.0f, 0.0f, -6.0f });

		/*BlackPearl::Object* Cube6 = CreateCube();
		Cube6->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		Cube6->GetComponent<BlackPearl::Transform>()->SetPosition({ -4.0f, -18.0f, -6.0f });
		Cube6->GetComponent<BlackPearl::Transform>()->SetScale({ 2.0f,2.0f,2.0f });
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(CubeTexture1);*/

		Deer=Layer::CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl");
		Quad = CreateQuad();
	/*	Quad->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer.GetShadowMapPointLightRenderer().GetFrameBuffer()->GetCubeMapDepthTexture());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/shadowMap/pointLight/QuadCubeMap.glsl");
*/


		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj->HasComponent<BlackPearl::MeshRenderer>()) {
				auto cubeMapBuffer = m_MasterRenderer->GetShadowMapPointLightRenderer().GetFrameBuffer();
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer->GetShadowMapPointLightRenderer().GetFrameBuffer()->GetCubeMapDepthTexture());
			}
				
		}



		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);








	}

	virtual ~ShadowMapPointLightLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		//// render
	/*	m_PointLight->GetComponent<BlackPearl::Transform>()->SetPosition({
			m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition().x,
			 m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition().y,
			 sin(glfwGetTime() * 0.5) * 3.0
			});*/
		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj != Quad &&obj!=Deer&& obj != Room && obj != m_PointLight && obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::Transform>()->SetRotation({
				obj->GetComponent<BlackPearl::Transform>()->GetRotation().x,
				 sin(glfwGetTime() * 0.5) * 90.0f,
				 obj->GetComponent<BlackPearl::Transform>()->GetRotation().z,
			
					});
		}
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

		

		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj != Quad && obj!= m_PointLight&&obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		}

		m_MasterRenderer->RenderPointLightShadowMap(m_ObjectsList, m_PointLight, { Quad,m_PointLight });
		// 2. Render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Shader->Bind();
		m_Shader->SetUniformVec3f("u_LightPos", m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition());

		m_Shader->SetUniform1f("u_FarPlane", BlackPearl::ShadowMapPointLightRenderer::s_FarPlane);
		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj != Quad &&obj!=m_PointLight&& obj->HasComponent<BlackPearl::MeshRenderer>()) {
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer->GetShadowMapPointLightRenderer().GetFrameBuffer()->GetCubeMapDepthTexture());
			}

		}
		m_PointLight->GetComponent<BlackPearl::MeshRenderer>()->SetShaders("assets/shaders/PointLight.glsl");
		
		glDisable(GL_CULL_FACE);
		m_Shader->SetUniformVec3f("u_LightPos", m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition());

		m_Shader->SetUniform1i("u_ReverseNormals", 1);
		m_MasterRenderer->RenderObject(Room);
		m_Shader->SetUniform1i("u_ReverseNormals", 0);

		glEnable(GL_CULL_FACE);
		m_Shader->Bind();

	
		m_Shader->SetUniform1f("u_FarPlane", BlackPearl::ShadowMapPointLightRenderer::s_FarPlane);
		m_MasterRenderer->RenderSceneExcept(m_ObjectsList, { Quad,m_PointLight }, GetLightSources());
		
		


	//	glViewport(0, 0, 240, 135);
		//Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->Bind();
	//	Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->SetUniformVec3f("u_LightPos", m_PointLight->GetComponent<BlackPearl::Transform>()->GetPosition());
	////	Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->SetUniformMat4f("u_LightProjectionViewMatrix", m_MasterRenderer.GetShadowMapLightProjectionMatrx());
	//	//TODO
	//	m_MasterRenderer.RenderObject(Quad);
	//	m_MasterRenderer.RenderObject(Quad);


	}



	void OnImguiRender() override {


		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Deer","Cube","Plane" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 8))
			{
				switch (entityIdx)
				{
				case 0:
					GE_CORE_INFO("Creating Empty...");
					Layer::CreateEmpty();
					break;
				case 1:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::ParallelLight);
					break;
				case 2:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::PointLight);
					break;
				case 3:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(BlackPearl::LightType::SpotLight);
					break;
				case 4:
					GE_CORE_INFO("Creating IronMan ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/OldHouse/Gost House/3D models/Gost House (5).obj", "assets/shaders/IronMan.glsl");

					break;
				case 5:
					GE_CORE_INFO("Creating Deer ...");
					//Layer::CreateModel("assets/models/u2k69vpbqpds-newbb8/BB8 New/bb8.obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/99-intergalactic_spaceship-obj/Intergalactic_Spaceship-(Wavefront).obj", "assets/shaders/IronMan.glsl");
					//Layer::CreateModel("assets/models/rc8c1qtjiygw-O/Organodron City/Organodron City.obj", "assets/shaders/IronMan.glsl");
					Layer::CreateModel("assets/models/deer/Deer.obj", "assets/shaders/IronMan.glsl");

					break;
				case 6:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				case 7:
					GE_CORE_INFO("Creating Plane ...");
					Layer::CreatePlane();
					break;
				}
			}
		}
		if (ImGui::BeginTabBar("TabBar 0", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Scene")) {
				std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
				ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 8);

				for (int n = 0; n < objsList.size(); n++) {
					//ImGui::Text("%s", objsList[n].c_str());
					bool is_selected = (currentObj != nullptr && currentObj->ToString() == objsList[n]->ToString());
					if (ImGui::Selectable(objsList[n]->ToString().c_str(), is_selected)) {
						currentObj = objsList[n];
						GE_CORE_INFO(objsList[n]->ToString() + "is selected")
					}

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::ListBoxFooter();
				ImGui::EndTabItem();
			}
			//}
		}
		ImGui::EndTabBar();

		////////////////////Inspector/////////////////////////
		ImGui::Begin("Inspector");


		ImGui::DragFloat("far_plane", &BlackPearl::ShadowMapPointLightRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
		ImGui::DragFloat("pointLight_FOV", &BlackPearl::ShadowMapPointLightRenderer::s_FOV, 0.5f,  0.0f, 180.0f, "%.3f ");

		if (currentObj != nullptr) {

			//std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

			//for (auto pair : componentList) {
			//	auto component = pair.second;
			//	if (component != nullptr) {

			//		switch (component->GetType()) {
			//		case BlackPearl::BaseComponent::Type::MeshRenderer: {
			//			std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
			//			ShowMeshRenderer(comp);
			//			break;
			//		}
			//		case BlackPearl::BaseComponent::Type::Transform: {
			//			std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
			//			ShowTransform(comp);
			//			break;
			//		}
			//		case BlackPearl::BaseComponent::Type::Light: {


			//			std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
			//			if (comp->GetType() == BlackPearl::LightType::PointLight) {
			//			//	std::shared_ptr<BlackPearl::PointLight> pointlight = std::dynamic_pointer_cast<BlackPearl::PointLight>(comp);
			//				ShowPointLight(comp);

			//			}
			//			break;
			//		}
			//		default:
			//			break;
			//		}

			//	}

			//}
			if (currentObj->HasComponent< BlackPearl::Transform>()) {
				ShowTransform(currentObj->GetComponent<BlackPearl::Transform>());

			}
			if (currentObj->HasComponent< BlackPearl::MeshRenderer>()) {
				ShowMeshRenderer(currentObj->GetComponent<BlackPearl::MeshRenderer>());

			}
			if (currentObj->HasComponent < BlackPearl::PointLight>()) {
				ShowPointLight(currentObj->GetComponent<BlackPearl::PointLight>());
			}
		}

		ImGui::End();



		m_fileDialog.Display();




	}
	void OnAttach() override {


	}
	void InputCheck(float ts)
	{
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_W)) {
			m_CameraPosition += cameraComponent->Front() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_S)) {
			m_CameraPosition -= cameraComponent->Front()* m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_A)) {
			m_CameraPosition -= cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_D)) {
			m_CameraPosition += cameraComponent->Right() * m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_E)) {
			m_CameraPosition += cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition -= cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		// ---------------------Rotation--------------------------------------

		float posx = BlackPearl::Input::GetMouseX();
		float posy = BlackPearl::Input::GetMouseY();
		if (BlackPearl::Input::IsMouseButtonPressed(BP_MOUSE_BUTTON_RIGHT)) {


			if (BlackPearl::Input::IsFirstMouse()) {
				BlackPearl::Input::SetFirstMouse(false);
				m_LastMouseX = posx;
				m_LastMouseY = posy;
			}
			float diffx = posx - m_LastMouseX;
			float diffy = -posy + m_LastMouseY;

			m_LastMouseX = posx;
			m_LastMouseY = posy;
			m_CameraRotation.Yaw += diffx * m_CameraRotateSpeed * ts;
			m_CameraRotation.Pitch += diffy * m_CameraRotateSpeed * ts;

			if (m_CameraRotation.Pitch > 89.0f)
				m_CameraRotation.Pitch = 89.0f;
			if (m_CameraRotation.Pitch < -89.0f)
				m_CameraRotation.Pitch = -89.0f;

			cameraComponent->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

		}
		else {

			m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
			m_LastMouseY = posy;
		}


		cameraComponent->SetPosition(m_CameraPosition);
	}
private:

	std::vector<BlackPearl::Object*> m_LightObjs;
	BlackPearl::Object* m_CameraObj;
	BlackPearl::Object* m_PointLight;

	BlackPearl::Object* Quad;
	BlackPearl::Object* Room;
	BlackPearl::Object* Deer;
	BlackPearl::MasterRenderer *m_MasterRenderer;

	BlackPearl::Object* m_SkyBox;




	//BlackPearl::Object* m_IronMan;
//BlackPearl::Object* m_Quad;
//BlackPearl::Object* m_Plane;
//BlackPearl::Object* m_Cube;
//BlackPearl::Object* m_Cube1;
//BlackPearl::Object* m_Cube2;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 9.0f;

	glm::vec4 m_BackgroundColor = { 0.0f,0.0f,0.0f,0.0f };




	glm::vec3 m_LightPos = { 0.0f, 1.5f, 0.0f };
	std::shared_ptr<BlackPearl::Shader> m_SimpleDepthShader;
	std::shared_ptr<BlackPearl::Shader> m_Shader;
	std::shared_ptr<BlackPearl::Shader> m_QuadDepthShader;


	GLuint woodTexture;
	GLuint planeVAO;
	GLuint depthMapFBO;
	GLuint depthMap;
	GLfloat near_plane = 1.0f, far_plane = 7.5f;
};
