#include "pch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GameEngine/Renderer/Texture/Texture.h"
#include "GameEngine/Renderer/Camera/PerspectiveCamera.h"
#include "Core.h"
#include "Input.h"
Application* Application::s_Instance = nullptr;
Application::Application()
{
	GE_ASSERT(!s_Instance, "Application's Instance already exist!")
	s_Instance = this;
	m_Window.reset(new Window());
	m_Camera.reset(Camera::Create(Camera::Perspective, { 45.0f, 800.0f, 600.0f, 0.1f, 100.0f }));

	float vertices[] = {
		// positions       // colors        // texture coords
		 0.5f, 0.5f,0.0f,  1.0f,0.0f,0.0f,  1.0f,1.0f,
		 0.5f,-0.5f,0.0f,  0.0f,1.0f,0.0f,  1.0f,0.0f,
		-0.5f,-0.5f,0.0f,  0.0f,0.0f,1.0f,  0.0f,0.0f,
		-0.5f, 0.5f,0.0f,  1.0f,1.0f,0.0f,  0.0f,1.0f,
	};

	unsigned int indices[] = {
		0,1,3,
		1,2,3
	};
	glGenVertexArrays(1, &m_VertexArrayID);
	glGenBuffers(1, &m_VertexBufferID);
	glGenBuffers(1, &m_IndexBufferID);

	glBindVertexArray(m_VertexArrayID);

	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// load and create a texture 
	m_Texture1.reset(new Texture("F:\\OpenGL\\LearnOpenGL\\LearnOpenGL\\src\\GameEngine\\Renderer\\Texture\\container.jpg"));
	m_Texture2.reset(new Texture("F:\\OpenGL\\LearnOpenGL\\LearnOpenGL\\src\\GameEngine\\Renderer\\Texture\\1.jpg"));


	//Shader
	const std::string vertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aColor;
		layout(location = 2) in vec2 aTexCoord;
		
		out vec3 ourColor;
		out vec2 TexCoord;
				
		uniform mat4 u_Model;
		uniform mat4 u_View;
		uniform mat4 u_Projection;

		void main()
		{
			gl_Position = u_Projection * u_View * u_Model * vec4(aPos,1.0);
			ourColor = aColor;
			TexCoord = vec2(aTexCoord.x,aTexCoord.y);
		}

	)";

	const std::string fragmentSrc = R"(
		#version 330 core
		out vec4 FragColor;
		
		in vec3 ourColor;
		in vec2 TexCoord;
		
		uniform sampler2D texture1;
		uniform sampler2D texture2;
		uniform float mixValue;
		void main(){
			// linearly interpolate between both textures (80% container, 20% awesomeface)
			FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), mixValue);
		}
	)";

	m_Shader.reset(new Shader(vertexSrc, fragmentSrc));
	m_Shader->Bind();
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniform1i("texture2", 1);
	m_Shader->SetUniform1f("mixValue", 0.2);
	m_Camera->SetPosition(glm::vec3(0.0f, 0.0f, 3.0f));
	m_CameraPosition = m_Camera->GetPosition();

}


Application::~Application()
{

}

void Application::Run()
{
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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		m_Texture1->Bind();
		glActiveTexture(GL_TEXTURE1);
		m_Texture2->Bind();

		m_Shader->Bind();

		glm::mat4 model	= glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	
		m_Shader->SetUniformMat4f("u_View", m_Camera->GetViewMatrix());
		m_Shader->SetUniformMat4f("u_Projection", m_Camera->GetProjectionMatrix());
		m_Shader->SetUniformMat4f("u_Model", model);
		m_Shader->SetUniform1f("mixValue", 0.5);

		glBindVertexArray(m_VertexArrayID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(m_Window->GetNativeWindow());
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &m_VertexArrayID);
	glDeleteBuffers(1, &m_VertexBufferID);
	glDeleteBuffers(1, &m_IndexBufferID);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
}

void Application::InputCheck(Timestep ts)
{
	if (Input::IsKeyPressed(GLFW_KEY_W)) {
		m_CameraPosition.z -= m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_S)) {
		m_CameraPosition.z += m_CameraMoveSpeed * ts;

	}
	if (Input::IsKeyPressed(GLFW_KEY_A)) {
		m_CameraPosition.x -= m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_D)) {
		m_CameraPosition.x += m_CameraMoveSpeed * ts;

	}
	if (Input::IsKeyPressed(GLFW_KEY_Q)) {
		m_CameraPosition.y -= m_CameraMoveSpeed * ts;
	}
	else if (Input::IsKeyPressed(GLFW_KEY_E)) {
		m_CameraPosition.y += m_CameraMoveSpeed * ts;

	}
	m_Camera->SetPosition(m_CameraPosition);
}
