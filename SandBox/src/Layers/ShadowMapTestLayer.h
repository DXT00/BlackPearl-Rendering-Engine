#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const GLuint SCR_WIDTH = 960, SCR_HEIGHT = 540;
GLuint quadVAO = 0;
GLuint quadVBO = 0;
GLuint cubeVAO = 0;
GLuint cubeVBO = 0;
class ShadowMapTestLayer :public BlackPearl::Layer {

public:

	ShadowMapTestLayer(const std::string& name, BlackPearl::ObjectManager *objectManager)
		: Layer(name, objectManager)
	{

		m_CameraObj = CreateCamera();
		auto cameraComponent = m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>();
		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();

		BlackPearl::Renderer::Init();

		//	m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(960, 540, {BlackPearl::FrameBuffer::Attachment::DepthTexture },false));

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

		// 设置OpenGL可选项
		glEnable(GL_DEPTH_TEST); // 开启深度测试

			//Shader reset
		m_Shader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/directLight/ShadowMaping_ShadowMapLayer.glsl"));
		m_SimpleDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/directLight/DepthShader_ShadowMapLayer.glsl"));
		m_QuadDepthShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/shadowMap/directLight/Quad_ShadowMapLayer.glsl"));


		m_Sun = CreateLight(BlackPearl::LightType::ParallelLight);
		

		BlackPearl::Object* Plane = CreatePlane();
		Plane->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Plane->GetComponent<BlackPearl::Transform>()->SetScale({ 25.0f, 0.5f, 25.0f });

		BlackPearl::Object* Cube = CreateCube();
		Cube->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube->GetComponent<BlackPearl::Transform>()->SetPosition({ 0.0f, 1.5f, 0.0f });

		BlackPearl::Object* Cube1 = CreateCube();
		Cube1->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube1->GetComponent<BlackPearl::Transform>()->SetPosition({ 2.0f, 0.0f, 1.0f });


		BlackPearl::Object* Cube2 = CreateCube();
		Cube2->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		Cube2->GetComponent<BlackPearl::Transform>()->SetPosition({ -1.0f, 0.0f, 2.0 });

		Quad = CreateQuad();
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer.GetShadowMapRenderer().GetFrameBuffer()->GetDepthTexture());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_QuadDepthShader);
		
	

		for (BlackPearl::Object* obj : m_ObjectsList) {
			if(obj->HasComponent<BlackPearl::MeshRenderer>())
			obj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_MasterRenderer.GetShadowMapRenderer().GetFrameBuffer()->GetDepthTexture());
		}
	
		

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);



		




	}

	virtual ~ShadowMapTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		//// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());


		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj!=Quad&&obj->HasComponent<BlackPearl::MeshRenderer>())
				obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_SimpleDepthShader);
		}


		m_MasterRenderer.RenderShadowMap(m_ObjectsList, m_Sun->GetComponent<BlackPearl::ParallelLight>(), {Quad});
		// 2. Render scene as normal 
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Shader->Bind();
		m_Shader->SetUniformVec3f("u_LightPos", m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection());
		m_Shader->SetUniformMat4f("u_LightProjectionViewMatrix", m_MasterRenderer.GetShadowMapLightProjectionMatrx());
		for (BlackPearl::Object* obj : m_ObjectsList) {
			if (obj != Quad && obj->HasComponent<BlackPearl::MeshRenderer>())
			obj->GetComponent<BlackPearl::MeshRenderer>()->SetShaders(m_Shader);
		}
		
		m_MasterRenderer.RenderSceneExcept(m_ObjectsList, Quad, GetLightSources() );

		glViewport(0, 0, 240, 135);
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->Bind();
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->SetUniformVec3f("u_LightPos", m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection());
		Quad->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes()[0].GetMaterial()->GetShader()->SetUniformMat4f("u_LightProjectionViewMatrix", m_MasterRenderer.GetShadowMapLightProjectionMatrx());


		m_MasterRenderer.RenderObject(Quad);


	}


	
	void OnImguiRender() override {


		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","ParallelLight","PointLight","SpotLight","IronMan","Cube","Plane" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 7))
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
					Layer::CreateModel("assets/models/IronMan/IronMan.obj", "assets/shaders/IronMan.glsl");
					break;
				case 5:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				case 6:
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
				ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 7);

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


		float pos[] = { m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().x, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().y, m_Sun->GetComponent<BlackPearl::ParallelLight>()->GetDirection().z };
		ImGui::DragFloat3("m_LightPos", pos, 0.1f, -100.0f, 100.0f, "%.3f ");
		m_Sun->GetComponent<BlackPearl::ParallelLight>()->SetDirection({ pos[0],pos[1],pos[2] });

		
		ImGui::DragFloat("near_plane", &BlackPearl::ShadowMapRenderer::s_NearPlane, 0.5f, -50.0f, 100.0f, "%.3f ");
		ImGui::DragFloat("far_plane", &BlackPearl::ShadowMapRenderer::s_FarPlane, 0.5f, -50.0f, 100.0f, "%.3f ");

		if (currentObj != nullptr) {

			std::unordered_map<BlackPearl::BaseComponent::Family, std::shared_ptr<BlackPearl::BaseComponent>> componentList = currentObj->GetComponentList();

			for (auto pair : componentList) {
				auto component = pair.second;
				if (component != nullptr) {

					switch (component->GetType()) {
					case BlackPearl::BaseComponent::Type::MeshRenderer: {
						std::shared_ptr<BlackPearl::MeshRenderer> comp = std::dynamic_pointer_cast<BlackPearl::MeshRenderer>(component);
						ShowMeshRenderer(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Transform: {
						std::shared_ptr<BlackPearl::Transform> comp = std::dynamic_pointer_cast<BlackPearl::Transform>(component);
						ShowTransform(comp);
						break;
					}
					case BlackPearl::BaseComponent::Type::Light: {

						std::shared_ptr<BlackPearl::Light> comp = std::dynamic_pointer_cast<BlackPearl::Light>(component);
						ShowLight(comp);
						break;
					}
					default:
						break;
					}

				}

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
	BlackPearl::Object* m_Sun;

	BlackPearl::Object* Quad;
	BlackPearl::MasterRenderer m_MasterRenderer;

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
