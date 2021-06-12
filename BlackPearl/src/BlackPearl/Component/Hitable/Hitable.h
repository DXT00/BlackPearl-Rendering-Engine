#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {
	class Hitable: public Component<Hitable>
	{
	public:
		Hitable(EntityManager* entityManager, Entity::Id id)
			:Component(entityManager, id, Component::Type::Hitable) {

			m_Type = Component::Type::Hitable;

		}

		AABB GetBoundingBox() { return m_Box; }
	private:
		AABB m_Box;
	};

}

