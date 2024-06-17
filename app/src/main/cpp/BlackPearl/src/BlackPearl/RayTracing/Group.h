#pragma once
#include <vector>
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Object/Object.h"
#include <map>
namespace BlackPearl {
	class Group
	{
	public:
		Group(Object* obj);
		AABB GetBoundingBox() const { return m_Box; }
		void PushBack(Object* obj);
		Object* GetRoot() const { return m_RootObject; }
		Group& operator << (Object* obj);
	private:
		//std::vector<Object*> m_Children;
		AABB m_Box;
		Object* m_RootObject = nullptr;
		
	};
}
