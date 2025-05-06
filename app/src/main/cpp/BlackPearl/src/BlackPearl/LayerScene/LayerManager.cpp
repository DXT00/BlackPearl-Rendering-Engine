#include "pch.h"
#include "LayerScene/LayerManager.h"
#include "BlackPearl/RHI/DynamicRHI.h"


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

