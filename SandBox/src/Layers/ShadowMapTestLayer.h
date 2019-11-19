#pragma once
#pragma once
#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imfilebrowser.h"
#include <glm/gtc/type_ptr.hpp>
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

		m_FrameBuffer.reset(DBG_NEW BlackPearl::FrameBuffer(960, 540, {BlackPearl::FrameBuffer::Attachment::DepthTexture },true));

		BlackPearl::Renderer::Init();
		m_Shader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/DepthShader_ShadowMapLayer.glsl"));
		
		CreateLight(BlackPearl::LightType::PointLight);
		m_CubeObj = CreateCube("assets/shaders/Cube.glsl", "assets/texture/wood.png");
		m_CubeObj1 = CreateCube();

		auto meshes = m_CubeObj->GetComponent<BlackPearl::MeshRenderer>()->GetMeshes();
		for (auto mesh : meshes)
			mesh.GetMaterial()->SetShader(m_Shader);

		m_QuadObj = CreateQuad("assets/shaders/Quad_ShadowMapLayer.glsl","");
		m_QuadObj->GetComponent<BlackPearl::MeshRenderer>()->SetTextures(m_FrameBuffer->GetDepthTexture());
	}

	virtual ~ShadowMapTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render

		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());

	
		glm::mat4 lightProjection, lightView;
		//glm::mat4 lightProjectionViewMatrix;
		//GLfloat nearPlane = -1.0f, farPlane = 1.0f;
		//lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
		//lightView = glm::lookAt(m_LightPos, glm::vec3(0.0f), glm::vec3{ 0.0,1.0,0.0 });
		//lightProjectionViewMatrix = lightProjection * lightView;
		//m_Shader->Bind();
		//m_Shader->SetUniformMat4f("lightProjectionViewMatrix", lightProjectionViewMatrix);
		////m_Shader->SetUniformMat4f("model", glm::mat4(1.0f));

		m_FrameBuffer->Bind(960, 540);
		glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT );

		DrawObjectsExcept(m_QuadObj);
		m_FrameBuffer->UnBind();

		glClearColor(0.0f, 0.1f, 0.1f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, 960, 540);
		DrawObjectsExcept(m_QuadObj);

		glViewport(0, 0, 480, 270);

		DrawObject(m_QuadObj);
		//DrawObjects();
		//DrawObjectsExcept(objs);
		//小于等于当前深度缓冲的fragment才被绘制
		//glDepthFunc(GL_LEQUAL);
		//DrawObject(m_SkyBoxObj);

		//glDepthFunc(GL_LESS);



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
	BlackPearl::Object* m_IronManObj;
	BlackPearl::Object* m_QuadObj;
	BlackPearl::Object* m_PlaneObj;
	BlackPearl::Object* m_CubeObj;
	BlackPearl::Object* m_CubeObj1;
	BlackPearl::Object* m_SkyBoxObj;
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




	std::shared_ptr<BlackPearl::FrameBuffer> m_FrameBuffer;
	glm::vec3 m_LightPos={ -2.0f, 4.0f, -1.0f };
	std::shared_ptr<BlackPearl::Shader> m_Shader;
};
