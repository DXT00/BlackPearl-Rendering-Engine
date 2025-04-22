#pragma once
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/LayerStack.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/Renderer/DeviceManager.h"

#ifdef USE_IMGUI
#include "BlackPearl/ImGui/ImGuiLayer.h"
#endif

namespace BlackPearl {

	class LayerManager {
	public:
		LayerManager();
		~LayerManager();
		void OnUpdateLayers(Timestep ts);
		void PushLayer(Layer *layer);
		void PushOverLayer(Layer *overlay);
		void RegisterDeviceManager(DeviceManager* deviceManager);

	private:

		LayerStack m_LayerStack;
#ifdef USE_IMGUI
		ImGuiLayer* m_ImGuiLayer = nullptr;
#endif
		DeviceManager* m_DeviceManager;

	};



}
