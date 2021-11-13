#include "pch.h"
#include "Entity.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	
	
	const Entity::Id Entity::s_INVALID;

	extern EntityManager* g_entityManager;
	inline bool Entity::Vaild()
	{
		return g_entityManager && g_entityManager->Valid(m_Id);
	}



	




	//////////////////////////////EntityManager///////////////////////////////////


	EntityManager::EntityManager()
	{
	}




	Entity *EntityManager::CreateEntity() {

		std::uint32_t index, version;
		if (m_FreeList.empty()) {
			index = m_IndexCounter++;
			ResizeEntityStorage(index);
			version = m_EntityVersion[index] = 1;
		}
		else {
			index = m_FreeList.back();
			m_FreeList.pop_back();
			version = m_EntityVersion[index];

		}
		Entity *entity = DBG_NEW Entity(Entity::Id(index, version));
		m_EntityList[index] = entity;
		return entity;
	}

	void EntityManager::DestroyEntity(Entity::Id entity)
	{
		AssertValid(entity);
		std::uint32_t index = entity.index();
		//ComponentMask mask = m_EntityComponentMasks[index];

		//TODO::remove component
		//m_EntityComponentMasks[index].reset();
		delete m_EntityList[index];
		m_EntityList[index] = nullptr;
		m_EntityVersion[index]++;
		m_FreeList.push_back(entity.index());
	}

	Entity EntityManager::GetEntity(Entity::Id id)
	{
		AssertValid(id);
		return Entity(id);
	}

	void EntityManager::ResizeEntityStorage(std::uint32_t index)
	{
		if (m_EntityList.size() <= index) {
			m_EntityList.resize(index + 1);
			m_EntityVersion.resize(index + 1);

		}
	}

	void EntityManager::ResizeConponentStorage(std::uint32_t index)
	{


	}



	//template<typename C, typename ...Args>
	//C* EntityManager::AddComponent(Entity::Id id, Args && ...args)
	//{
	//	AssertValid(id);
	//	const BaseComponent::Family family = GetComponentFamliy<C>();
	//	//Returns whether the bit at family pos is set (i.e., whether it is one).
	//	GE_ASSERT(!m_EntityComponentMasks[id.index].test(family), "Component already existed!")
	//

	//	m_EntityComponentMasks[id.index].set(family);
	//	
	//	//所有的component都用Create实例化-->方便子类继承！
	//	C* component = C::Create(this,id,std::forward<Args>(args)...);


	//	return component;
	//}

	//template<typename C>
	//ComponentHandle<C> EntityManager::GetComponent(Entity::Id id)
	//{
	//	size_t famliy = GetComponentFamliy<C>();
	//	return ComponentHandle<C>(this, id);
	//}

	//template<typename C>
	//bool EntityManager::HasComponent(Entity::Id id)
	//{
	//	size_t famliy = GetComponentFamliy<C>();

	//	if (m_EntityComponentMasks[id.index()][famliy])
	//		return true;
	//	else
	//		return false;
	//}

	//template<typename C>
	//void EntityManager::RemoveComponent(Entity::Id id)
	//{
	//	AssertValid(id);
	//	const BaseComponent::Family famliy = GetComponentFamliy<C>();

	//	m_EntityComponentMasks[id.index(0)].reset(famliy);

	//}



	//template<typename C>
	//BaseComponent::Family EntityManager::GetComponentFamliy()
	//{
	//	//remove_const:If C is const-qualified, this is the same type as C but with its const-qualification removed. Otherwise, it is C unchanged.
	//	return Component<typename std::remove_const<C>::type>::Famliy();
	//}
}

	


