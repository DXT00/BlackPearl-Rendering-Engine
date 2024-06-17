#pragma once
#include <bitset>
#include "BlackPearl/Component/BaseComponent.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Core.h"
//#include "BlackPearl/Entity/Entity.h"
//#include "BlackPearl/Entity/EntityManager.h"
namespace BlackPearl {
	template<typename C>
	class Component : public BaseComponent 
	{
	public:

		

		Component(Type type)
		:BaseComponent(type){}
		virtual ~Component()=default;

		
		//friend class EntityManager;		
		static BaseComponent::Family Famliy();
	protected:
		//EntityManager* m_EntityManager = nullptr;
		//Entity::Id m_Id;
	
	};

	template<typename C>
	BaseComponent::Family Component<C>::Famliy()
	{
		//static����ֻ��ʼ��һ�Σ�
		//��仰ֻ��ִ��һ�Σ�s_FamliyCounterֻ���ڵ�һ��famliy��ʼ��ʱ��1
		static BaseComponent::Family famliy = BaseComponent::s_FamliyCounter++;
		GE_ASSERT(famliy < Configuration::MaxComponents, "famliy num exceed MaxComponents!");
			return famliy;
	}
}
