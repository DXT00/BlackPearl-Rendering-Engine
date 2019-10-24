#pragma once
#include "BlackPearl/Entity/Entity.h"
//#include "BlackPearl/Entity/EntityManager.h"
/**
 * A ComponentHandle<C> is a wrapper around an instance of a component.
 *
 * It provides safe access to components. The handle will be invalidated under
 * the following conditions:
 *
 * - If a component is removed from its host entity.
 * - If its host entity is destroyed.
 */
namespace BlackPearl {

	class EntityManager;


	template<typename C>
	class ComponentHandle {
	public:
		/*ComponentHandle()
			:m_EntityManager(nullptr){}*/
		typedef ComponentHandle<C> Handle;

		ComponentHandle(EntityManager* manager, Entity::Id id,const C* component)
			:m_EntityManager(manager), m_Id(id),m_Component(component) {};

		virtual ~ComponentHandle() {
			delete m_EntityManager;
			m_EntityManager = nullptr;
		}

		bool Valid() const;
		Entity Entity();
		inline C* GetComponent() {
			return m_Component;
		}
	private:
		EntityManager* m_EntityManager;
		Entity::Id m_Id;
		C* m_Component;

	};

};