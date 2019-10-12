#include <BlackPeral.h>
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "BlackPearl/Application.h"
#include "imgui/imgui.h"
class ExampleLayer :public BlackPearl::Layer{


public:
	ExampleLayer(const std::string& name):Layer(name) {
	
		m_Camera.reset(BlackPearl::Camera::Create(BlackPearl::Camera::Perspective, { 45.0f, 800.0f, 600.0f, 0.1f, 100.0f }));

		std::shared_ptr<BlackPearl::Light> parallelLight;
		parallelLight.reset(BlackPearl::Light::Create(BlackPearl::LightType::ParallelLight));
		parallelLight->SetProps({ {0.2f, 0.2f, 0.2f},{1.0f, 0.2f, 0.1f},{1.0f, 1.0f, 1.0f} });
		m_LightSources.AddLight(parallelLight);
		//set light sources
		//注意：如果时SpotLight就要更新相机位置！
		std::shared_ptr<BlackPearl::Light> spotLight;
		spotLight.reset(BlackPearl::Light::Create(BlackPearl::LightType::SpotLight, m_Camera->GetPosition(), m_Camera->Front(), glm::cos(glm::radians(20.0f)), glm::cos(glm::radians(30.0f))));
		std::dynamic_pointer_cast<BlackPearl::SpotLight>(spotLight)->SetAttenuation(BlackPearl::SpotLight::Attenuation(200));
		m_LightSources.AddLight(spotLight);
		// positions of the point lights
		glm::vec3 pointLightPositions[] = {
			glm::vec3(2.3f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-2.7f,  3.0f, -7.5f),
			//glm::vec3(-4.0f,  2.0f, -12.0f),
		};
		std::vector<BlackPearl::Light::Props> pointLightProps = {
		{{0.2f, 0.2f, 0.2f},{ 1.0f, 0.2f, 0.1f},{1.0f, 1.0f, 1.0f}},
		{{0.2f, 0.2f, 0.2f},{ 0.2f, 1.0f, 0.1f},{1.0f, 1.0f, 1.0f}},
		{{0.2f, 0.2f, 0.2f},{ 0.1f, 0.2f, 1.0f},{1.0f, 1.0f, 1.0f}}
		};
		for (int i = 0; i < 3; i++)
		{
			std::shared_ptr<BlackPearl::Light> pointLight;

			pointLight.reset(BlackPearl::Light::Create(BlackPearl::LightType::PointLight, pointLightPositions[i], {}, 0, 0, pointLightProps[i]));
			std::dynamic_pointer_cast<BlackPearl::PointLight>(pointLight)->SetAttenuation(BlackPearl::PointLight::Attenuation(3250));

			m_LightSources.AddLight(pointLight);
		}
		BlackPearl::Renderer::Init();

		float vertices[] = {
			// positions			// texture coords  //normal
			 -0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f, 0.0f, 1.0f,

			-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		-1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		-1.0f, 0.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		0.0f, 1.0f, 0.0f
		};
		float woodboxVertices[] = {
			// positions			 // texture coords				 // normals          
			-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f,	 0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,	 1.0f,  0.0f,	 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,	 1.0f,  1.0f,	 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,	 1.0f,  1.0f,	 0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,	 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f,	 0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,	 0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,	 0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,	 1.0f,  1.0f,	 0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,	 1.0f,  1.0f,	 0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,	 0.0f,  1.0f,	 0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,	 0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	-1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,	 1.0f,  1.0f,	-1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	-1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	-1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,	-1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	-1.0f,  0.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,	 1.0f,  1.0f,	 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,	 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	 1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,	 1.0f,  1.0f,	 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,	 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,	 1.0f,  0.0f,	 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,	 0.0f,  0.0f,	 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f,  1.0f,	 0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,	 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,	 1.0f,  1.0f,	 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	 0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,	 1.0f,  0.0f,	 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,	 0.0f,  0.0f,	 0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,	 0.0f,  1.0f,	 0.0f,  1.0f,  0.0f,
		};



		//Shader
		m_Shader.reset(new BlackPearl::Shader("assets/shaders/IronMan.glsl"));
		m_Shader->Bind();


		m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
		m_CameraPosition = m_Camera->GetPosition();
		m_CameraRotation.Yaw = m_Camera->Yaw();
		m_CameraRotation.Pitch = m_Camera->Pitch();


		//m_Model.reset(new Model("assets/models/nanosuit/nanosuit.obj"));
		m_Model.reset(new BlackPearl::Model("assets/models/IronMan/IronMan.obj"));

	
	
	
	
	
	
	
	
	};
	void OnAttach() override {


	}
	void OnImguiRender() override {
	
		ImGui::Begin("Settings");
		ImGui::Text("Hello World");
		ImGui::End();
	
	
	}
	void OnUpdate(BlackPearl::Timestep ts) override {
		

			InputCheck(ts);
	
			// render
			BlackPearl::RenderCommand::SetClearColor({0.0f,0.0f,0.0f,0.0f});
			m_Shader->Bind();


			BlackPearl::Renderer::BeginScene(*m_Camera, m_LightSources);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));



			m_Model->Draw(m_Shader, model, m_LightSources);
			//for (unsigned int i = 0; i < 10; i++)
			//{

			//	glm::mat4 model = glm::mat4(1.0f);
			//	model = glm::translate(model, cubePositions[i]);
			//	model = glm::rotate(model, (float)(glfwGetTime()), glm::vec3(1.0f, (float)i * 20, 0.0f));//(float)(glfwGetTime())
			//	m_Shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
			//	m_Shader->SetUniformVec3f("u_CameraViewPos", m_Camera->GetPosition());

			//	

			//	m_Shader->SetUniformVec3f("u_Material.ambient", glm::vec3(0.25,0.20725,0.20725));
			//	m_Shader->SetUniform1f("u_Material.shininess",64.0f);

			////	m_Model->Draw(m_Shader);


			//}


	}
	void InputCheck(float ts)
	{
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_W)) {
			m_CameraPosition += m_Camera->Front() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_S)) {
			m_CameraPosition -= m_Camera->Front()* m_CameraMoveSpeed * ts;
		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_A)) {
			m_CameraPosition -= m_Camera->Right() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_D)) {
			m_CameraPosition += m_Camera->Right() * m_CameraMoveSpeed * ts;

		}
		if (BlackPearl::Input::IsKeyPressed(BP_KEY_E)) {
			m_CameraPosition -= m_Camera->Up() * m_CameraMoveSpeed * ts;
		}
		else if (BlackPearl::Input::IsKeyPressed(BP_KEY_Q)) {
			m_CameraPosition += m_Camera->Up() * m_CameraMoveSpeed * ts;

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

			m_Camera->SetRotation(m_CameraRotation.Yaw, m_CameraRotation.Pitch);

		}
		else {

			m_LastMouseX = posx;//lastMouse时刻记录当前坐标位置，防止再次点击右键时，发生抖动！
			m_LastMouseY = posy;
		}


		m_Camera->SetPosition(m_CameraPosition);
	}

private:
	std::unique_ptr<BlackPearl::Camera> m_Camera;

	std::shared_ptr<BlackPearl::Shader> m_Shader;
	std::shared_ptr<BlackPearl::Texture> m_Texture1;
	std::shared_ptr<BlackPearl::Texture> m_Texture2;
	std::shared_ptr<BlackPearl::Texture> m_DiffuseMap;
	std::shared_ptr<BlackPearl::Texture> m_SpecularMap;
	std::shared_ptr<BlackPearl::Texture> m_EmissionMap;

	//	unsigned int m_VertexArrayID, m_IndexBufferID, m_VertexBufferID;

		//std::shared_ptr<VertexArray> m_VertexArray;

	std::shared_ptr<BlackPearl::Mesh> m_Mesh;

	std::shared_ptr<BlackPearl::Model> m_Model;
	//std::shared_ptr<Light> m_SpotLightSource;
	BlackPearl::LightSources m_LightSources;
	float m_CameraMoveSpeed = 5.0f;
	float m_CameraRotateSpeed = 9.0f;
	glm::vec3 m_CameraPosition = { 0.0f,0.0f,0.0f };
	struct CameraRotation {
		float Yaw;
		float Pitch;

	};
	CameraRotation m_CameraRotation;
	float m_LastMouseX;
	float m_LastMouseY;





};


class SandBox :public BlackPearl::Application{
public:
	SandBox() {
		
		PushLayer(new ExampleLayer("ExampleLayer"));
	}
	virtual ~SandBox() = default;

};

BlackPearl::Application* BlackPearl::CreateApplication() {
	
	return new SandBox();

}

