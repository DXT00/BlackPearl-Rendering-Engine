#include "pch.h"
#include "LayerStack.h"
namespace BlackPearl {

	LayerStack::LayerStack()
	{

	}


	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers) {
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer * layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		//layer->OnAttach();  //有OnAttach就异常？？为什么？？？？--》在Application::PushOverLay(Layer* layer)里已经OnAttach()了！
	}

	void LayerStack::PushOverLay(Layer * overlay)
	{
		m_Layers.emplace_back(overlay);
		//overlay->OnAttach();//有OnAttach就异常？？为什么？？？？
	}

	void LayerStack::PopLayer(Layer * layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end()) {
			//layer->OnDetach();
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverLayer(Layer * overlay)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end()) {
			//overlay->OnDetach();
			m_Layers.erase(it);
		}
	}
}