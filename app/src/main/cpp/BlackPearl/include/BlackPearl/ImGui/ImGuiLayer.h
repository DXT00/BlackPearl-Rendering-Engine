#pragma once

#ifdef USE_IMGUI
#include"BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Timestep/Timestep.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//#include "imgui/imgui.h"
//#include "imgui_impl_glfw.h"

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


		//Editor Functions

		void ShowMeshRenderer(MeshRenderer* comp);
		void ShowTransform(Transform* comp, Object* obj);
		void ShowLightProbe(LightProbe* probe, Object* obj);
		void ShowPointLight(PointLight* pointLight);
		void ShowParallelLight(ParallelLight* parallelLight);
		void ShowCamera(PerspectiveCamera* perspectiveCamera);
		void ShowCamera(MainCamera* mainCamera);
		void ShowTerrian(Object* obj);

		void ShowShader(std::string imguiShaders, int meshIndex, int& itemIndex, int offset);
		void ShowTextures(std::string imguiShaders, int meshIndex, int& itemIndex, TextureType textureType, TextureType& type, int offset);
		void ShowMaterialProps(Props& imGuiProps);


	};

}
#endif