#include "pch.h"
#include "LayerManager.h"



namespace BlackPearl {

	void LayerManager::OnUpdateLayers(Timestep ts)
	{
		for (Layer* layer : m_LayerStack) {
			layer->OnUpdate(ts);
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

