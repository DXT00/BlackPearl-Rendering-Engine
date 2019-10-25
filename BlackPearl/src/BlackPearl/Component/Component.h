#pragma once
#include <bitset>
#include "BlackPearl/Component/BaseComponent.h"
#include "BlackPearl/Entity/Entity.h"
//#include "BlackPearl/Entity/EntityManager.h"
namespace BlackPearl {
	template<typename C>
	class Component : public BaseComponent 
	{
	public:
		Component(EntityManager* entityManager, Entity::Id id)
		:m_EntityManager(entityManager),m_Id(id){}
		virtual ~Component()=default;

		//friend class EntityManager;		
		static BaseComponent::Family Famliy();
	protected:
		EntityManager* m_EntityManager = nullptr;
		Entity::Id m_Id;
	};

	template<typename C>
	BaseComponent::Family Component<C>::Famliy()
	{
		//static变量只初始化一次！
		//这句话只会执行一次！s_FamliyCounter只会在第一次famliy初始化时加1
		static BaseComponent::Family famliy = BaseComponent::s_FamliyCounter++;
		GE_ASSERT(famliy < MaxComponents, "famliy num exceed MaxComponents!")
			return famliy;
	}
}
