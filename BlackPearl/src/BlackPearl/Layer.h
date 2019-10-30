#pragma once

#include<string>
#include"Object/Object.h"
#include "Entity/Entity.h"
#include "Renderer/LightComponent/Light.h"
#include "Renderer/LightComponent/LightSources.h"
#include"Timestep/Timestep.h"

namespace BlackPearl {

	class Layer
	{
	public:
		Layer(const std::string& name, EntityManager *entityManager)
			:m_DebugName(name),m_EntityManager(entityManager) {
			m_LightSources = new LightSources();
		}
		virtual ~Layer() {
			delete m_EntityManager;
			
		};
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImguiRender() {}

		inline std::string GetString() { return m_DebugName; }
		EntityManager *GetEntityManager() { return m_EntityManager; }
		LightSources *GetLightSources() { return m_LightSources; }

		virtual Object* CreateEmpty();
		virtual Object* CreateLight(LightType type);
		virtual Object* CreateModel();
		virtual Object* CreateCube();


		void DrawObjects();
		void DestroyObjects();


	protected:
		std::string m_DebugName;
		EntityManager *m_EntityManager;
		std::unordered_map<std::uint32_t, Object*> m_EntityToObjects;
		LightSources* m_LightSources;
	};

}