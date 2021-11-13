#include "pch.h"
#include "Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Material/Texture.h"
#include "Component/CameraComponent/PerspectiveCamera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer.h"
#include "Core.h"
#include "Input.h"
#include "Event/Event.h"
#include "Event/MouseEvent.h"
#include "Component/LightComponent/ParallelLight.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/LightComponent/SpotLight.h"
#include "ImGui/ImGuiLayer.h"
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
namespace BlackPearl {
	ObjectManager* g_objectManager = DBG_NEW ObjectManager();
	EntityManager*  g_entityManager = DBG_NEW EntityManager();

	double Application::s_AppFPS = 0.0f;
	double Application::s_AppAverageFPS = 0.0f;

	Application* Application::s_Instance = nullptr;
	Application::Application()
	{
		GE_ASSERT(!s_Instance, "Application's Instance already exist!")
		s_Instance = this;
		m_Window.reset(DBG_NEW Window());
		m_Window->SetCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		m_CurrentScene = DBG_NEW Scene();
		m_StartTimeMs = 0;// duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	}

	Application::~Application()
	{
		/*if(m_CurrentScene!=nullptr)
		delete m_CurrentScene;*/
		GE_SAVE_DELETE(m_CurrentScene);
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window->GetNativeWindow())) {

			double currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			double runtimeSecond = (currentTimeMs - m_StartTimeMs) / 1000.0f;

			m_FrameNum++;

			if (runtimeSecond > 1.0f) {

				s_AppFPS = m_FrameNum;
			//	GE_CORE_INFO("FPS = " + std::to_string(s_AppFPS) );
				m_TotalFrameNum += m_FrameNum;

				m_FrameNum = 0;
				m_StartTimeMs = currentTimeMs;

				m_TotalSecond++;
				s_AppAverageFPS = (double)m_TotalFrameNum / m_TotalSecond;
				GE_CORE_INFO("AverageFPS = " + std::to_string(s_AppAverageFPS));


			}


			//GE_ASSERT(m_FrameNum < MAXLONGLONG, "m_FrameNum out of range!");
			//m_FPS = (double)m_FrameNum / runtimeSecond;

			float time = (float)glfwGetTime();
			Timestep ts = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (glfwGetKey(m_Window->GetNativeWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(m_Window->GetNativeWindow(), true);

			m_CurrentScene->OnUpdateLayers(ts);

			m_Window->OnUpdate();

		}


		//glm::vec3 cubePositions[] = {
		//glm::vec3(0.0f,  0.0f,  0.0f),
		//glm::vec3(2.0f,  5.0f, -15.0f),
		//glm::vec3(-1.5f, -2.2f, -2.5f),
		//glm::vec3(-3.8f, -2.0f, -12.3f),
		//glm::vec3(2.4f, -0.4f, -3.5f),
		//glm::vec3(-1.7f,  3.0f, -7.5f),
		//glm::vec3(1.3f, -2.0f, -2.5f),
		//glm::vec3(1.5f,  2.0f, -2.5f),
		//glm::vec3(1.5f,  0.2f, -1.5f),
		//glm::vec3(-1.3f,  1.0f, -1.5f)
		//};

		//render loop
		glfwTerminate();
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


}