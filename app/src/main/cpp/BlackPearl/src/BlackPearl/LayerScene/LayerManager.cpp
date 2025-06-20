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

	}
	LayerManager::~LayerManager() {


	};

    /*
    
    layer-->update
        deviceManager-->update

        graph1-->update
        graph2-->update

        ui graph-->update
    
    */
	//extern DynamicRHI::Type g_RHIType;
	void LayerManager::OnUpdateLayers(Timestep ts)
	{
		g_deviceManager->BeginFrame();
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(ts);
		}
#ifdef USE_IMGUI
        if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {

            m_ImGuiLayer->Begin();
            for (Layer* layer : m_LayerStack)
                layer->OnImguiRender();
           m_ImGuiLayer->End();
        }
#endif

		g_deviceManager->Present();
        ++g_deviceManager->m_FrameIndex;

		//TODO::需要写到 g_deviceManager->Run()里
		
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
        overlay->SetDeviceManager(m_DeviceManager);
        overlay->OnSetup();
		overlay->OnAttach();
	}

	void LayerManager::RegisterDeviceManager(DeviceManager* deviceManager)
	{
		m_DeviceManager = deviceManager;

#ifdef USE_IMGUI
        m_ImGuiLayer = DBG_NEW ImGuiLayer("ImGuiLayer");
        PushOverLayer(m_ImGuiLayer);
#endif

	}

	
}

