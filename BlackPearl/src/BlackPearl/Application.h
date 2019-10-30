#pragma once
#include "BlackPearl/Renderer/Shader.h"
#include <memory>
#include "Window.h"
#include "Renderer/Material/Texture.h"
#include "Renderer/CameraComponent/Camera.h"
#include "Renderer/VertexArray.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "Event/MouseEvent.h"
#include "Renderer/LightComponent/Light.h"
#include "Renderer/LightComponent/LightSources.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Model/Model.h"
#include "Layer.h"
#include "LayerStack.h"
#include "BlackPearl/Event/Event.h"
#include "BlackPearl/ImGui/ImGuiLayer.h"
#include "BlackPearl/Scene/Scene.h"

namespace BlackPearl {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		inline static Application &Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }
		Scene* GetScene() { return m_CurrentScene; }
		void Run();
		void OnEvent(Event &event);

	

	private:
		bool OnCameraRotate(MouseMovedEvent&e);
	
	
	private:
		static Application* s_Instance; //TODO::可以不delete,或者改为 unique_ptr
		float m_LastFrameTime = 0.0f;

	private:
		std::unique_ptr<Window> m_Window;


		Scene* m_CurrentScene;

	};
	//To be define in a client
	Application * CreateApplication();
}