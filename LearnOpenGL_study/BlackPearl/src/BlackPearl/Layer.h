#pragma once
#include"Timestep/Timestep.h"
#include<string>
#include "BlackPearl/System/System.h"
namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name, SystemManager *systemManager, EntityManager *entityManager)
			:m_DebugName(name),m_SystemManager(systemManager),m_EntityManager(entityManager) {};
		virtual ~Layer() {
			delete m_EntityManager;
			delete m_SystemManager;
		};
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImguiRender() {}

		inline std::string GetString() { return m_DebugName; }


	protected:
		std::string m_DebugName;
		SystemManager *m_SystemManager;
		EntityManager *m_EntityManager;
	};

}