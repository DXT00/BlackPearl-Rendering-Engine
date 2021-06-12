#include "pch.h"
#include "Group.h"
//#include "BlackPearl/Component/Hitable/Hitable.h"
namespace BlackPearl {

	Group::Group(Object *obj)
	{
		m_RootObject = obj;
	}
	void Group::PushBack(Object* obj)
	{
		if (obj != NULL) {
			m_RootObject->AddChildObj(obj);
			//m_Box = m_Box + obj->GetComponent<Hitable>()->GetBoundingBox();
		}
	}
}

