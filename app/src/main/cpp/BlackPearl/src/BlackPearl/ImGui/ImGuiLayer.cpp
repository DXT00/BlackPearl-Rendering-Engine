#include "pch.h"
#include "ImGuiLayer.h"

#include "BlackPearl/Application.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLWindow.h"
//#define IMGUI_IMPL_OPENGL_LOADER_GLAD
//
//#include "examples/imgui_impl_opengl3.cpp"
//#include "examples/imgui_impl_glfw.cpp"
namespace BlackPearl {

	void ImGuiLayer::OnAttach()
	{
#ifdef GE_PLATFORM_WINDOWS

        IMGUI_CHECKVERSION();
		ImGui::CreateContext();
	

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(static_cast<OpenGLWindow*>(&app.GetWindow())->GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
#endif
	}

	void ImGuiLayer::OnDetach()
	{
#ifdef GE_PLATFORM_WINDOWS

        ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
#endif

    }

	void ImGuiLayer::OnUpdate(Timestep ts)
	{
		//OnImguiRender();
	}

	void ImGuiLayer::OnImguiRender()
	{
#ifdef GE_PLATFORM_WINDOWS

        static bool show = true;
		ImGui::ShowDemoWindow(&show);
#endif



    }

	void ImGuiLayer::Begin()
	{
#ifdef GE_PLATFORM_WINDOWS

        ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
#endif

    }

	void ImGuiLayer::End()
	{
#ifdef GE_PLATFORM_WINDOWS

        ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif

    }
}