#pragma once
#include"BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Timestep/Timestep.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace BlackPearl {

	class ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name)
			:Layer(name) {}
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImguiRender() override;


		void Begin();
		void End();


	};

}