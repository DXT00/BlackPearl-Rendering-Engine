#pragma once
#include"BlackPearl/Renderer/Shader.h"
#include<memory>
#include"Window.h"
#include"Renderer/Texture/Texture.h"
#include"Renderer/CameraComponent/Camera.h"
#include"Renderer/VertexArray.h"
#include"BlackPearl/Timestep/Timestep.h"
#include "Event/MouseEvent.h"
#include "Renderer/LightComponent/Light.h"
#include"Renderer/LightComponent/LightSources.h"
#include "Renderer/Mesh/Mesh.h"
#include "Renderer/Model/Model.h"
#include "Layer.h"
#include "LayerStack.h"
#include "BlackPearl/Event/Event.h"
#include "BlackPearl/ImGui/ImGuiLayer.h"
namespace BlackPearl {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		inline static Application &Get() { return *s_Instance; }
		inline Window& GetWindow() { return *m_Window; }

		void Run();
		void OnEvent(Event &event);

		void PushLayer(Layer *layer);
		void PushOverLayer(Layer *overlay);

	private:
		bool OnCameraRotate(MouseMovedEvent&e);
	
	
	private:
		static Application* s_Instance; //TODO::可以不delete,或者改为 unique_ptr
		float m_LastFrameTime = 0.0f;

	private:
		LayerStack m_LayerStack;
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
	};
	//To be define in a client
	Application * CreateApplication();
}