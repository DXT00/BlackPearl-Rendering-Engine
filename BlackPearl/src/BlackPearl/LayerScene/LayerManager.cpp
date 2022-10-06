#include "pch.h"
#include "LayerManager.h"
#include "BlackPearl/RHI/DynamicRHI.h"


namespace BlackPearl {
	extern DynamicRHI::Type g_RHIType;
	void LayerManager::OnUpdateLayers(Timestep ts)
	{
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(ts);
		}
		//TODO:: D3D12 support imgui
		if (g_RHIType == DynamicRHI::Type::OpenGL) {
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImguiRender();
			m_ImGuiLayer->End();
		}

	}

	void LayerManager::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void LayerManager::PushOverLayer(Layer * overlay)
	{
		m_LayerStack.PushOverLay(overlay);
		overlay->OnAttach();
	}

	
}

