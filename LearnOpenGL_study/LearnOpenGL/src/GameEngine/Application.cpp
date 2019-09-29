#include "pch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Texture/Texture.h"
#include "Renderer/Camera/PerspectiveCamera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Core.h"
#include "Input.h"
#include "Event/Event.h"
#include "Event/MouseEvent.h"
#include "Renderer/Lighting/ParallelLight.h"
#include "Renderer/Lighting/PointLight.h"
#include "Renderer/Lighting/SpotLight.h"

Application* Application::s_Instance = nullptr;
Application::Application()
{
	GE_ASSERT(!s_Instance, "Application's Instance already exist!")
	s_Instance = this;
	m_Window.reset(new Window());
	m_Window->SetCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	m_Camera.reset(Camera::Create(Camera::Perspective, { 45.0f, 800.0f, 600.0f, 0.1f, 100.0f }));
	
	std::shared_ptr<Light> parallelLight;
	parallelLight.reset(Light::Create(LightType::ParallelLight));
	parallelLight->SetProps({ {0.2f, 0.2f, 0.2f},{1.0f, 0.2f, 0.1f},{1.0f, 1.0f, 1.0f} });
	m_LightSources.AddLight(parallelLight);
	//set light sources
	//注意：如果时SpotLight就要更新相机位置！
	std::shared_ptr<Light> spotLight;
	spotLight.reset(Light::Create(LightType::SpotLight,m_Camera->GetPosition(),m_Camera->Front(),glm::cos(glm::radians(8.0f)), glm::cos(glm::radians(10.0f))));
	std::dynamic_pointer_cast<SpotLight>(spotLight)->SetAttenuation(SpotLight::Attenuation(200));
	m_LightSources.AddLight(spotLight);
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),

		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-2.7f,  3.0f, -7.5f),
		//glm::vec3(-4.0f,  2.0f, -12.0f),
	};
	std::vector<Light::Props> pointLightProps= {
	{{0.2f, 0.2f, 0.2f},{ 1.0f, 0.2f, 0.1f},{1.0f, 1.0f, 1.0f}},
	{{0.2f, 0.2f, 0.2f},{ 0.2f, 1.0f, 0.1f},{1.0f, 1.0f, 1.0f}},
	{{0.2f, 0.2f, 0.2f},{ 0.1f, 0.2f, 1.0f},{1.0f, 1.0f, 1.0f}}
	};
	for (int i = 0; i < 3; i++)
	{
		std::shared_ptr<Light> pointLight;
		
		pointLight.reset(Light::Create(LightType::PointLight, pointLightPositions[i], {}, 0, 0, pointLightProps[i]));
		std::dynamic_pointer_cast<PointLight>(pointLight)->SetAttenuation(PointLight::Attenuation(3250));

		m_LightSources.AddLight(pointLight);
	}
	Renderer::Init();

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
	m_Shader.reset(new Shader("assets/shaders/NanoMode.glsl"));
	m_Shader->Bind();


	m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 8.0f));
	m_CameraPosition = m_Camera->GetPosition();
	m_CameraRotation.Yaw = m_Camera->Yaw();
	m_CameraRotation.Pitch = m_Camera->Pitch();


	m_Model.reset(new Model("assets/models/nanosuit/nanosuit.obj"));
	//m_Model.reset(new Model("assets/models/IronMan/IronMan.obj"));



}

Application::~Application()
{

}

void Application::Run()
{
	glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//render loop
	while (!glfwWindowShouldClose(m_Window->GetNativeWindow())) {

		float time = (float)glfwGetTime();
		Timestep ts = time - m_LastFrameTime;
		m_LastFrameTime = time;

		InputCheck(ts);
		if (glfwGetKey(m_Window->GetNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(m_Window->GetNativeWindow(), true);
		// render
		// ------

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_Shader->Bind();


		Renderer::BeginScene(*m_Camera, m_LightSources);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f,-1.75f,0.0f));
		model = glm::rotate(model,glm::radians(180.0f) ,glm::vec3(0.0f,1.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

	
		
		m_Model->Draw(m_Shader,model, m_LightSources);
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

		
		glfwSwapBuffers(m_Window->GetNativeWindow());
		glfwPollEvents();
	}



	glfwTerminate();
}

void Application::InputCheck(float ts)
{
	if (Input::IsKeyPressed(GLFW_KEY_W)) {
		m_CameraPosition += m_Camera->Front() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_S)) {
		m_CameraPosition -= m_Camera->Front()* m_CameraMoveSpeed * ts;
	}
	if (Input::IsKeyPressed(GLFW_KEY_A)) {
		m_CameraPosition -= m_Camera->Right() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_D)) {
		m_CameraPosition += m_Camera->Right() * m_CameraMoveSpeed * ts;

	}
	if (Input::IsKeyPressed(GLFW_KEY_E)) {
		m_CameraPosition -= m_Camera->Up() * m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_Q)) {
		m_CameraPosition += m_Camera->Up() * m_CameraMoveSpeed * ts;

	}
	// ---------------------Rotation--------------------------------------

	float posx = Input::GetMouseX();
	float posy = Input::GetMouseY();
	if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT)) {


		if (Input::IsFirstMouse()) {
			Input::SetFirstMouse(false);
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

void Application::OnEvent(Event& event)
{
	EventDispacher dispacher(event);

	dispacher.Dispatch<MouseMovedEvent>(std::bind(&Application::OnCameraRotate, this, std::placeholders::_1));



}

bool Application::OnCameraRotate(MouseMovedEvent & e)
{
	float posx = e.GetMouseX();
	float posy = e.GetMouseY();

	return true;
}
