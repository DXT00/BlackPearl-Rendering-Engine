#pragma once
#include "BlackPearl/Renderer/Shader/Shader.h"
#include <memory>
#include "Window.h"
#include "Renderer/Material/Texture.h"
#include "Component/CameraComponent/Camera.h"
#include "Renderer/VertexArray.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "Event/MouseEvent.h"
#include "Component/LightComponent/Light.h"
#include "Component/LightComponent/LightSources.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Model/Model.h"
#include "LayerScene/Layer.h"
#include "LayerStack.h"
#include "BlackPearl/Event/Event.h"
#include "BlackPearl/ImGui/ImGuiLayer.h"
#include "BlackPearl/LayerScene/LayerManager.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/Renderer/Material/MaterialManager.h"

#include <chrono>
using namespace std::chrono;
namespace BlackPearl {


	class Application
	{
	public:
		struct AppConf {
#ifdef GE_PLATFORM_WINDOWS
			HINSTANCE hInstance;
#endif
			int nShowCmd;
			std::string renderer;
			DynamicRHI::Type rhiType;
		};
#ifdef GE_PLATFORM_WINDOWS
		Application(HINSTANCE hInstance, int nShowCmd, DynamicRHI::Type rhiType, const std::string& renderer);
#else
        Application(int nShowCmd, DynamicRHI::Type rhiType, const std::string& renderer);
#endif
		virtual ~Application();

		inline static Application &Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		LayerManager* GetLayerManager() { return m_LayerManager; }
		AppConf GetAppConf() const { return m_AppConf; }
		void Init();
		void Run();
		void OnEvent(Event &event);
		static double s_AppFPS;
		static double s_AppAverageFPS;
		static long long s_TotalFrameNum;

		static bool IsFullscreen();
		static void SetWindowZorderToTopMost(bool setToTopMost);

	private:
		bool ShouldCloseWindow();
		bool OnCameraRotate(MouseMovedEvent&e);
		bool OnWindowClose();
		void EngineExit();
	
	
	private:
		static Application* s_Instance; //TODO::���Բ�delete,���߸�Ϊ unique_ptr
		double m_LastFrameTime = 0.0f;

	private:
		Window* m_Window;
		AppConf m_AppConf;

		double m_StartTimeMs;
		long long m_FrameNum = 0;
		LayerManager* m_LayerManager = nullptr;
		double m_TotalSecond = 0;

	};
	//To be define in a client
#ifdef GE_PLATFORM_WINDOWS
	Application * CreateApplication(HINSTANCE hInstance, int nShowCmd);
#endif

#ifdef GE_PLATFORM_ANDRIOD
    Application* CreateAndriodApplication(int nShowCmd);
#endif
}