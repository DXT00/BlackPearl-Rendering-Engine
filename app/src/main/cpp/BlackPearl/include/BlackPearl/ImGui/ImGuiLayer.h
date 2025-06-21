#pragma once

#ifdef USE_IMGUI
#include"BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Timestep/Timestep.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

//#include "imgui/imgui.h"
//#include "imgui_impl_glfw.h"

namespace BlackPearl {
    class FPlatformOpenGLContext;
	class ImGuiLayer :public Layer
	{
	public:
		ImGuiLayer(const std::string& name)
			:Layer(name) {}
		virtual ~ImGuiLayer() = default;
        virtual void OnSetup() override;
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImguiRender() override;


		void Begin();
		void End();



		void ShowMeshRenderer(MeshRenderer* comp);
		void ShowTransform(Transform* comp, Object* obj);
		void ShowLightProbe(LightProbe* probe, Object* obj);
		void ShowPointLight(PointLight* pointLight);
		void ShowParallelLight(DirectionLight* parallelLight);
		void ShowCamera(PerspectiveCamera* perspectiveCamera);
		void ShowCamera(MainCamera* mainCamera);
		void ShowTerrian(Object* obj);

		void ShowShader(std::string imguiShaders, int meshIndex, int& itemIndex, int offset);
		void ShowTextures(std::string imguiShaders, int meshIndex, int& itemIndex, TextureType textureType, TextureType& type, int offset);
		void ShowMaterialProps(Props& imGuiProps);
        HWND GetNativeWindow() const { return m_hImGuiWnd; }
        ImGuiIO* GetIO() { return m_IO; }
        HWND m_hImGuiWnd;
        HDC         m_hImGuiDC;
        HGLRC       m_hImGuiRC;
        ImGuiIO* m_IO;
        void InitImGUI();
        void CreateImguiWindow();
        HDC         m_hBackupDC;
        HGLRC       m_hBackupRC;
        GLuint vao;
    private:

        FPlatformOpenGLContext* m_ImGuiContext;

        CommandListHandle       m_CommandList = nullptr;
        //HWND m_hImGuiWnd;
        //HDC         m_hImGuiDC;
        //HGLRC       m_hImGuiRC;
        //ImGuiIO* m_IO;
        //void InitImGUI();
        //void CreateImguiWindow();
        //HDC         m_hBackupDC;
        //HGLRC       m_hBackupRC;
 
	};

}
#endif