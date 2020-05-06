#include "pch.h"
#include "Scene.h"



namespace BlackPearl {

	void Scene::OnUpdateLayers(Timestep ts)
	{
		for (Layer* layer : m_LayerStack) {

			layer->OnUpdate(ts);

		}
		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImguiRender();
		m_ImGuiLayer->End();
	}

	void Scene::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Scene::PushOverLayer(Layer * overlay)
	{
		m_LayerStack.PushOverLay(overlay);
		overlay->OnAttach();
	}

	
}

