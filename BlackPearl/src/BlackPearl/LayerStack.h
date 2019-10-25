#pragma once
#include "Layer.h"
namespace BlackPearl {

	class  LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer *layer);
		void PushOverLay(Layer * overlay);

		void PopLayer(Layer *layer);
		void PopOverLayer(Layer *overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }

	private:
		std::vector<Layer*> m_Layers;
		//	std::vector<Layer*>::iterator m_LayerInsert;
		unsigned int m_LayerInsertIndex = 0;


	};


}


