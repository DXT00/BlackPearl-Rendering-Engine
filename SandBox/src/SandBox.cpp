#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"

#include "imgui/imgui.h"

#include <glm/gtc/type_ptr.hpp>
#include <string>
//#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Application.h"

class EntityTestLayer :public BlackPearl::Layer {
public:

	EntityTestLayer(const std::string& name, BlackPearl::EntityManager *entityManager)
		: Layer(name, entityManager)
	{
		BlackPearl::Entity *entity = entityManager->CreateEntity();
		m_CameraObj = new BlackPearl::Object(entity->GetEntityManager(), entity->GetId());
		std::shared_ptr<BlackPearl::Camera> cameraComponent(m_CameraObj->AddComponent<BlackPearl::PerspectiveCamera>());


		cameraComponent->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = cameraComponent->GetPosition();
		m_CameraRotation.Yaw = cameraComponent->Yaw();
		m_CameraRotation.Pitch = cameraComponent->Pitch();


		//glm::vec3 pointLightPositions[] = {
		//glm::vec3(2.3f,  0.2f,  2.0f),
		//glm::vec3(2.3f, -3.3f, -4.0f),
		//glm::vec3(-2.7f,  3.0f, -7.5f),
		//glm::vec3(-4.0f,  2.0f, -12.0f),
		//};
		//std::vector<BlackPearl::Light::Props> pointLightProps = {
		//{{0.2f, 0.2f, 0.2f},{ 1.0f, 0.2f, 0.1f},{1.0f, 1.0f, 1.0f}},
		//{{0.2f, 0.2f, 0.2f},{ 0.2f, 1.0f, 0.1f},{1.0f, 1.0f, 1.0f}},
		//{{0.2f, 0.2f, 0.2f},{ 0.1f, 0.2f, 1.0f},{1.0f, 1.0f, 1.0f}},
		//{{0.2f, 0.2f, 0.2f},{ 0.1f, 0.5f, 0.5f},{1.0f, 1.0f, 1.0f}}

		//};
		//m_LightObjs.resize(pointLightProps.size());
		//for (int i = 0; i < pointLightProps.size(); i++)
		//{
		//	BlackPearl::Entity* lightEntity = entityManager->CreateEntity();
		//	m_LightObjs[i] = new BlackPearl::Object(lightEntity->GetEntityManager(), lightEntity->GetId());


		//	std::shared_ptr<BlackPearl::Light> lightComponent = m_LightObjs[i]->AddComponent<BlackPearl::PointLight>();

		//	//lightComponent.reset();

		//	lightComponent->SetProps(pointLightProps[i]);
		//	std::dynamic_pointer_cast<BlackPearl::PointLight>(lightComponent)->SetPosition(pointLightPositions[i]);
		//	std::dynamic_pointer_cast<BlackPearl::PointLight>(lightComponent)->SetAttenuation(BlackPearl::PointLight::Attenuation(3250));

		//	m_LightSources.AddLight(std::move(lightComponent));
		//}



		/*std::shared_ptr<BlackPearl::Shader>IronManShader(new BlackPearl::Shader("assets/shaders/IronMan.glsl"));
		IronManShader->Bind();
		std::shared_ptr<BlackPearl::Model> IronManModel(new BlackPearl::Model("assets/models/IronMan/IronMan.obj", IronManShader));
		BlackPearl::Entity *ironManEntity = entityManager->CreateEntity();
		m_IronManObj = new BlackPearl::Object(ironManEntity->GetEntityManager(), ironManEntity->GetId());

		std::shared_ptr<BlackPearl::Transform> TransformComponent(m_IronManObj->AddComponent<BlackPearl::Transform>());
		TransformComponent->SetPosition({ 0.0f, -1.75f, 0.0f });
		TransformComponent->SetRotation({ 0.0,180.0,0.0 });
		TransformComponent->SetScale({ 0.01f, 0.01f, 0.01f });

		std::shared_ptr<BlackPearl::MeshRenderer> meshRendererComponent(m_IronManObj->AddComponent<BlackPearl::MeshRenderer>(IronManModel, TransformComponent->GetTransformMatrix()));
*/


		BlackPearl::Renderer::Init();


	}

	virtual ~EntityTestLayer() {

		DestroyObjects();

	}
	void OnUpdate(BlackPearl::Timestep ts) override {


		InputCheck(ts);

		// render
		BlackPearl::RenderCommand::SetClearColor(m_BackgroundColor);
		BlackPearl::Renderer::BeginScene(*(m_CameraObj->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
		//m_IronManObj->GetComponent<BlackPearl::MeshRenderer>()->DrawModel();
		DrawObjects();
	}

	void OnImguiRender() override {


		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Suqare Color", glm::value_ptr(m_BackgroundColor));
		ImGui::End();

		static BlackPearl::Object* currentObj = nullptr;//TODO::注意内存泄漏

		if (ImGui::CollapsingHeader("Create")) {

			const char* const entityItems[] = { "Empty","PointLight","SpotLight","IronMan","Cube" };
			static int entityIdx = -1;
			if (ImGui::Combo("CreateEntity", &entityIdx, entityItems, 5))
			{
				switch (entityIdx)
				{
				case 0:
					GE_CORE_INFO("Creating Empty...");
					Layer::CreateEmpty();
					break;
				case 1:
					GE_CORE_INFO("Creating PointLight...");
					Layer::CreateLight(BlackPearl::LightType::PointLight);
					break;
				case 2:
					GE_CORE_INFO("Creating SpotLight ...");
					Layer::CreateLight(BlackPearl::LightType::SpotLight);
					break;
				case 3:
					GE_CORE_INFO("Creating IronMan ...");
					Layer::CreateModel();
					break;
				case 4:
					GE_CORE_INFO("Creating Cube ...");
					Layer::CreateCube();
					break;
				}
			}
		}
		if (ImGui::CollapsingHeader("Scene")) {

			std::vector<BlackPearl::Object*> objsList = GetObjects();		//TODO::
			ImGui::ListBoxHeader("CurrentEntities", (int)objsList.size(), 6);

			for (int n = 0; n < objsList.size(); n++) {
				//ImGui::Text("%s", objsList[n].c_str());
				bool is_selected = (currentObj!=nullptr && currentObj->ToString() == objsList[n]->ToString());
				if (ImGui::Selectable(objsList[n]->ToString().c_str(), is_selected)) {
					currentObj = objsList[n];
					GE_CORE_INFO(objsList[n]->ToString() + "is selected")
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::ListBoxFooter();
		}

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

					default:
						break;
					}

				}

			}
		}
		


		ImGui::End();
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
			m_CameraPosition -= cameraComponent->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition += cameraComponent->Up() * m_CameraMoveSpeed * ts;
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
	BlackPearl::Object *m_CameraObj;
	BlackPearl::Object *m_IronManObj;


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


};

class SandBox :public BlackPearl::Application {
public:
	SandBox() {
		//PushLayer(new ExampleLayer("ExampleLayer"));
		BlackPearl::EntityManager * entityManager = DBG_NEW BlackPearl::EntityManager();
		BlackPearl::Layer* layer = DBG_NEW EntityTestLayer("Entity Layer", entityManager);
		GetScene()->PushLayer(layer);
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication() {

	return DBG_NEW SandBox();

}

