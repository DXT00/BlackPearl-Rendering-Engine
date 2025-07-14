#include "pch.h"
//#include <glad/glad.h>
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLThirdParty.h"

//#include <GLFW/glfw3.h>
#include "Application.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BlackPearl/RHI/RHITexture.h"
#ifdef	GE_API_D3D12
#include "BlackPearl/RHI/D3D12RHI/D3D12ModelLoader.h"
#endif
#include "Component/CameraComponent/PerspectiveCamera.h"
#include "Renderer/Renderer.h"
//#include "Renderer/Buffer/Buffer.h"
#include "Core.h"
#include "Input.h"
#include "Event/Event.h"
#include "Event/MouseEvent.h"
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/LightComponent/SpotLight.h"
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/LayerScene/LayerManager.h"
#include "BlackPearl/Renderer/CullingManager.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Log.h"
#include "BlackPearl/Renderer/Model/ModelLoader.h"
#include <BlackPearl/Luanch/Luanch.h>
#include "BlackPearl/Config.h"
#include "BlackPearl/UI/UIManager.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/SystemTextures.h"
#ifdef GE_PLATFORM_ANDROID
#include "Luanch/Android/AndroidEventManager.h"
#endif
#include "Timestep/TimeCounter.h"
#include "Map/MapManager.h"
#include "Renderer/GIManager.h"
#include "FBX/FBXLoader.h"
namespace BlackPearl {

	Log* g_Log = nullptr;
	ObjectManager*   g_objectManager = DBG_NEW ObjectManager();
	EntityManager*   g_entityManager = DBG_NEW EntityManager();
	MaterialManager* g_materialManager = DBG_NEW MaterialManager();
	DeviceManager*   g_deviceManager = nullptr;
	CullingManager* g_cullingManager = DBG_NEW CullingManager();
	ModelLoader* g_modelLoader = nullptr;
#ifdef USE_FBX
    FBXLoader* g_fbxLoader = nullptr;
#endif
	RootFileSystem* g_rootFileSystem = DBG_NEW RootFileSystem();
	UIManager* g_uiManager = nullptr;
	ShaderFactory* g_shaderFactory = nullptr;
    MapManager* g_mapManager = DBG_NEW MapManager(Configuration::MapSize, Configuration::AreaSize);
    GIManager* g_GIManager = DBG_NEW GIManager();
	double Application::s_AppFPS = 0.0f;
	double Application::s_AppAverageFPS = 0.0f;

	Application* Application::s_Instance = nullptr;
	extern bool g_shouldEngineExit;
	extern DynamicRHI* g_DynamicRHI;
	long long Application::s_TotalFrameNum = 0;
    long long Application::s_CurrentFrameNum = 0;

	Application::Application(INSTANCE_HANDLE hInstance, int nShowCmd, DynamicRHI::Type rhiType, AppVersion version)
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
		//m_AppConf.renderer = renderer;
		m_AppConf.version = version;
		m_AppConf.rhiType = rhiType;
		//Init();
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

		if (m_AppConf.version >= AppVersion::VERSION_1_0) {
#define APP_VERSION VERSION_1_0
			g_deviceManager->Init(deviceParams);
		}
		else {
#define APP_VERSION VERSION_0_0

		}
       // printf("current context after init g_deviceManager = %p\n", wglGetCurrentContext());

	#ifdef	GE_API_D3D12
			g_modelLoader = DBG_NEW D3D12ModelLoader();
	#else
			g_modelLoader = DBG_NEW ModelLoader();
#ifdef USE_FBX
            g_fbxLoader = DBG_NEW FBXLoader();
#endif
    #endif
        //GE_ERROR_JUDGE();
			g_modelLoader->RegisterDeviceManager(g_deviceManager);
#ifdef USE_FBX
            g_fbxLoader->RegisterDeviceManager(g_deviceManager);
#endif
            // GE_ERROR_JUDGE();
			m_LayerManager = DBG_NEW LayerManager();
			m_LayerManager->RegisterDeviceManager(g_deviceManager);
       // GE_ERROR_JUDGE();
			g_objectManager->RegisterDeviceManager(g_deviceManager);
       // GE_ERROR_JUDGE();
			g_uiManager = DBG_NEW UIManager();
      //  GE_ERROR_JUDGE();
			g_shaderFactory = DBG_NEW ShaderFactory(g_deviceManager->GetDevice(), g_rootFileSystem, "assets/shaders/spv");
       // GE_ERROR_JUDGE();
			g_materialManager->RegisterDeviceManager(g_deviceManager);
			g_materialManager->Init();


        ///GE_ERROR_JUDGE();
		m_StartTimeMs = 0;// duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	}

	void Application::Run()
	{
		while (!ShouldEngineExit()) {
            SCOPE_TIME_COUNTER(FPS)
			double currentTimeMs = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
			double runtimeSecond = (currentTimeMs - m_StartTimeMs) / 1000.0f;
            s_CurrentFrameNum = m_FrameNum;

			m_FrameNum++;
			if (runtimeSecond > 1.0f) {

				//s_AppFPS = 1000.0f/Count_FPS.GetRuntime();
				//GE_CORE_INFO("FPS = " + std::to_string(s_AppFPS) );
				s_TotalFrameNum += m_FrameNum;

				m_FrameNum = 0;
				m_StartTimeMs = currentTimeMs;
				m_TotalSecond++;
				s_AppAverageFPS = (double)s_TotalFrameNum / m_TotalSecond;

#ifdef GE_PLATFORM_WINDOWS
                GE_CORE_INFO("AverageFPS = {0}", std::to_string(s_AppAverageFPS).c_str());
#else
				GE_CORE_INFO("AverageFPS = %s" ,std::to_string(s_AppAverageFPS).c_str());
#endif
			}


			//GE_ASSERT(m_FrameNum < MAXLONGLONG, "m_FrameNum out of range!");
			//m_FPS = (double)m_FrameNum / runtimeSecond;

			//float time = (float)glfwGetTime();
			double curFrameTime = currentTimeMs / 1000.0f;
			double ts = curFrameTime - m_LastFrameTime;
			m_LastFrameTime = curFrameTime;
			ShouldCloseWindow();

			BasicRenderer::s_DrawCallCnt = 0;
#ifdef GE_PLATFORM_ANDROID

            FAppEventManager::GetInstance()->Tick();
#endif
			m_Window->OnUpdate();

            m_LayerManager->OnUpdateLayers(ts);

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
#ifdef GE_PLATFORM_ANDROID
        return FAppEventManager::GetInstance()->IsGamePaused();
#endif
		return false;
	}

	void Application::EngineExit() {
		RHIEngineExit();
	}

	void Application::_InitFileSystem()
	{

#ifdef	GE_API_VULKAN
		std::filesystem::path shaderPath = g_rootFileSystem->GetExeDir()/"assets/shaders"/Configuration::GetShaderTypeName();
		//std::filesystem::path appShaderPath = g_rootFileSystem->GetExeDir()/"shaders/pt_sdk"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
		//std::filesystem::path nrdShaderPath = g_rootFileSystem->GetExeDir()/"shaders/nrd"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
		//std::filesystem::path ommShaderPath = g_rootFileSystem->GetExeDir()/"shaders/omm"/Configuration::GetShaderTypeName(DynamicRHI::g_RHIType);
	
		g_rootFileSystem->mount("assets/shaders/spv", shaderPath);
		//g_rootFileSystem->mount("/shaders/app", appShaderPath);
		//g_rootFileSystem->mount("/shaders/nrd", nrdShaderPath);
		//g_rootFileSystem->mount("/shaders/omm", ommShaderPath);
#endif


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