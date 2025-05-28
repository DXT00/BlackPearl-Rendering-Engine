#include "pch.h"
#include "LayerScene/LayerManager.h"
#include "BlackPearl/RHI/DynamicRHI.h"

#include "ImGui/ImGuiLayer.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_win32.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
namespace BlackPearl {

	LayerManager::LayerManager() {
#ifdef USE_IMGUI
		m_ImGuiLayer = DBG_NEW ImGuiLayer("ImGuiLayer");
		PushOverLayer(m_ImGuiLayer);
#endif

	}
	LayerManager::~LayerManager() {


	};


	//extern DynamicRHI::Type g_RHIType;
	void LayerManager::OnUpdateLayers(Timestep ts)
	{
		//g_deviceManager->BeginFrame();
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(ts);
		}
		//g_deviceManager->Present();
		//TODO:: D3D12 support imgui
		if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
#ifdef USE_IMGUI
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImguiRender();
			m_ImGuiLayer->End();

            //m_ImGuiLayer->m_hBackupDC = wglGetCurrentDC();
            //m_ImGuiLayer->m_hBackupRC = wglGetCurrentContext();
            //// === 2. 渲染 ImGui 窗口 ===
            //wglMakeCurrent(m_ImGuiLayer->m_hImGuiDC, m_ImGuiLayer->m_hImGuiRC);

            //MSG msg = {};
            //while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            //    TranslateMessage(&msg);
            //    DispatchMessage(&mcsg);
            //}

            //ImGui_ImplOpenGL3_NewFrame();
            //ImGui_ImplWin32_NewFrame();
            //ImGui::NewFrame();
            //bool show_demo_window = true;
            //if (show_demo_window)
            //    ImGui::ShowDemoWindow(&show_demo_window);

            //bool show_another_window = true;
            //ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            //ImGui::Text("Hello from another window!");
            //if (ImGui::Button("Close Me"))
            //    show_another_window = false;
            //ImGui::End();
     
            //ImGui::EndFrame();
            //ImGui::Render();
            //glViewport(0, 0, (int)m_ImGuiLayer->m_IO->DisplaySize.x, (int)m_ImGuiLayer->m_IO->DisplaySize.y);
            //glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // 灰色背景
            //glClear(GL_COLOR_BUFFER_BIT);
            //ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            //if (m_ImGuiLayer->m_IO->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            //{

            //    //GLFWwindow* backup_current_context = glfwGetCurrentContext();
            //    ImGui::UpdatePlatformWindows();
            //    ImGui::RenderPlatformWindowsDefault();
            //   

            //    // 恢复 OpenGL 上下文
            //    //glfwMakeContextCurrent(backup_current_context);

            //}
            //SwapBuffers(m_ImGuiLayer->m_hImGuiDC);
            //wglMakeCurrent(m_ImGuiLayer->m_hBackupDC, m_ImGuiLayer->m_hBackupRC);

#endif
		}
	}

	void LayerManager::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->SetDeviceManager(m_DeviceManager);
		layer->OnSetup();
		layer->OnAttach();
	
	}

	void LayerManager::PushOverLayer(Layer * overlay)
	{
		m_LayerStack.PushOverLay(overlay);
		overlay->OnAttach();
	}

	void LayerManager::RegisterDeviceManager(DeviceManager* deviceManager)
	{
		m_DeviceManager = deviceManager;
	}

	
}

