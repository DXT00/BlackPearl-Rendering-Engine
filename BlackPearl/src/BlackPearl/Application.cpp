#include "pch.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Application.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer/Material/Texture.h"
#include "Component/CameraComponent/PerspectiveCamera.h"
#include "Renderer/Renderer.h"
#include "Renderer/Buffer/Buffer.h"
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
#include "BlackPearl/LayerScene/LayerManager.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Log.h"
#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include <BlackPearl/Luanch/Luanch.h>
#include "BlackPearl/Config.h"

namespace BlackPearl {

	Log* g_Log = nullptr;
	ObjectManager*   g_objectManager = DBG_NEW ObjectManager();
	EntityManager*   g_entityManager = DBG_NEW EntityManager();
	MaterialManager* g_materialManager = DBG_NEW MaterialManager();
	DeviceManager*   g_deviceManager = nullptr;
	CullingManager* g_cullingManager = DBG_NEW CullingManager();
	ModelLoader* g_modelLoader = nullptr;
	RootFileSystem* g_rootFileSystem = DBG_NEW RootFileSystem();

	double Application::s_AppFPS = 0.0f;
	double Application::s_AppAverageFPS = 0.0f;

	Application* Application::s_Instance = nullptr;
	extern bool g_shouldEngineExit;
	extern DynamicRHI* g_DynamicRHI;
	long long Application::s_TotalFrameNum = 0;
	Application::Application(HINSTANCE hInstance, int nShowCmd, DynamicRHI::Type rhiType, const std::string& renderer)
	{
		if (!g_DynamicRHI) {
			DynamicRHIInit(rhiType);
		}
		g_shouldEngineExit = false;
		g_Log = DBG_NEW Log();

		GE_ASSERT(!s_Instance, "Application's Instance already exist!");
		s_Instance = this;
		m_AppConf.hInstance = hInstance;
		m_AppConf.nShowCmd = nShowCmd;
		m_AppConf.renderer = renderer;
		m_AppConf.rhiType = rhiType;
		Init();
	}

	Application::~Application()
	{
		GE_SAVE_DELETE(m_LayerManager);
		GE_SAVE_DELETE(m_Window);
	}

	void Application::Init()
	{

		m_Window = RHIInitWindow();
		m_Window->SetAppCallBack(std::bind(&Application::OnEvent, this, std::placeholders::_1));

		/*file system*/
		_InitFileSystem();
		/*Render*/
		DeviceCreationParameters deviceParams;
		deviceParams.backBufferWidth = Configuration::WindowWidth;
		deviceParams.backBufferHeight = Configuration::WindowHeight;
		deviceParams.vsyncEnabled = Configuration::Vsync;
		deviceParams.swapChainBufferCount = Configuration::SwapchainCount;
		deviceParams.maxFramesInFlight = deviceParams.swapChainBufferCount;

		g_deviceManager = DeviceManager::Create(DynamicRHI::g_RHIType);
		g_deviceManager->Init(deviceParams);

#ifdef	GE_API_D3D12
		g_modelLoader = D3D12ModelLoader();
#else
		g_modelLoader = DBG_NEW ModelLoader();
#endif
		
		g_modelLoader->RegisterDeviceManager(g_deviceManager);
		m_LayerManager = DBG_NEW LayerManager();

		m_StartTimeMs = 0;// duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}

	void Application::Run()
	{
		while (!ShouldEngineExit()) {

			double currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			double runtimeSecond = (currentTimeMs - m_StartTimeMs) / 1000.0f;

			m_FrameNum++;

			if (runtimeSecond > 1.0f) {

				s_AppFPS = m_FrameNum;
				//GE_CORE_INFO("FPS = " + std::to_string(s_AppFPS) );
				s_TotalFrameNum += m_FrameNum;

				m_FrameNum = 0;
				m_StartTimeMs = currentTimeMs;
				m_TotalSecond++;
				s_AppAverageFPS = (double)s_TotalFrameNum / m_TotalSecond;
				GE_CORE_INFO("AverageFPS = " + std::to_string(s_AppAverageFPS));
			}


			//GE_ASSERT(m_FrameNum < MAXLONGLONG, "m_FrameNum out of range!");
			//m_FPS = (double)m_FrameNum / runtimeSecond;

			//float time = (float)glfwGetTime();
			double curFrameTime = currentTimeMs / 1000.0f;
			double ts = curFrameTime - m_LastFrameTime;
			m_LastFrameTime = curFrameTime;
			ShouldCloseWindow();

			BasicRenderer::s_DrawCallCnt = 0;
			m_LayerManager->OnUpdateLayers(ts);
			m_Window->OnUpdate();

		}
		//render loop
		EngineExit();
	}



	void Application::OnEvent(Event& event)
	{
		EventDispacher dispacher(event);

		dispacher.Dispatch<MouseMovedEvent>(std::bind(&Application::OnCameraRotate, this, std::placeholders::_1));

	}


	bool Application::ShouldCloseWindow()
	{
		if (m_Window->ShouldClose()) {
			RequestEngineExit();
			return true;
		}
		return false;
	}

	void Application::EngineExit() {
		RHIEngineExit();
	}

	void Application::_InitFileSystem()
	{
		std::filesystem::path shaderPath = g_rootFileSystem->GetExeDir()/"assets/shaders"/Configuration::GetShaderTypeName();
		//std::filesystem::path appShaderPath = g_rootFileSystem->GetExeDir()/"shaders/pt_sdk"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
		//std::filesystem::path nrdShaderPath = g_rootFileSystem->GetExeDir()/"shaders/nrd"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
		//std::filesystem::path ommShaderPath = g_rootFileSystem->GetExeDir()/"shaders/omm"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
	
		g_rootFileSystem->mount("assets/shaders/spv", shaderPath);
		//g_rootFileSystem->mount("/shaders/app", appShaderPath);
		//g_rootFileSystem->mount("/shaders/nrd", nrdShaderPath);
		//g_rootFileSystem->mount("/shaders/omm", ommShaderPath);


	}

	bool Application::OnCameraRotate(MouseMovedEvent& e)
	{
		float posx = e.GetMouseX();
		float posy = e.GetMouseY();

		return true;
	}

	bool Application::OnWindowClose()
	{
		return false;
	}
	// TODO::
	bool Application::IsFullscreen()
	{
		return false;
	}
	//TODO::
	void Application::SetWindowZorderToTopMost(bool setToTopMost)
	{
		return;
	}
}