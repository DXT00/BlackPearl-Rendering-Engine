#pragma once
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/LayerStack.h"
#include "BlackPearl/ImGui/ImGuiLayer.h"
#include "BlackPearl/Timestep/Timestep.h"
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
#include "BlackPearl/Renderer/DeviceManager.h"

namespace BlackPearl {

	class LayerManager {
	public:
		LayerManager() {
			m_ImGuiLayer = DBG_NEW ImGuiLayer("ImGuiLayer");
			PushOverLayer(m_ImGuiLayer);
		}
		~LayerManager() {
			//Layer通过m_LayerStack删除，不要重复删除！
			//GE_SAVE_DELETE(m_ImGuiLayer);
			//GE_SAVE_DELETE(m_ImGuiEntityMgr);
			//GE_SAVE_DELETE(m_ImGuiObjectMgr);

		};

		void OnUpdateLayers(Timestep ts);
		void PushLayer(Layer *layer);
		void PushOverLayer(Layer *overlay);
		void RegisterDeviceManager(DeviceManager* deviceManager);

	private:

		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer; //每个LayerManager都有一个ImGuiLayer
		DeviceManager* m_DeviceManager;

	};



}
