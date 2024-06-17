#include "pch.h"
#include "LayerStack.h"
namespace BlackPearl {

	LayerStack::LayerStack()
	{

	}


	LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers) {
			GE_SAVE_DELETE(layer);
		}
	}

	void LayerStack::PushLayer(Layer * layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
		//layer->OnAttach();  //��OnAttach���쳣����Ϊʲô��������--����Application::PushOverLay(Layer* layer)���Ѿ�OnAttach()�ˣ�
	}

	void LayerStack::PushOverLay(Layer * overlay)
	{
		m_Layers.emplace_back(overlay);
		//overlay->OnAttach();//��OnAttach���쳣����Ϊʲô��������
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