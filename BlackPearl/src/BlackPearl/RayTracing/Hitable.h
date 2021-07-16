#pragma once
#include "BlackPearl/AABB/AABB.h"
namespace BlackPearl {
	class Hitable
	{
	public:
		Hitable();
		virtual void BuildBox() = 0;
		AABB GetBoundingBox() const { return m_Box; }

	protected:
		AABB m_Box;

	};

}

