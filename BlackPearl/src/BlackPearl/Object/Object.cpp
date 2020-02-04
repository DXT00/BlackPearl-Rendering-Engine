#include "pch.h"
#include "Object.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include <utility>
namespace BlackPearl {


	
	Configuration::ComponentMask Object::GetComponentMask() const
	{
		return m_ComponentMask;
	}



	void Object::Destroy()
	{
		//for (auto& pair : m_Components) { //TODO::看一下要不要用智能指针
		//	auto component = pair.second;
		//	component.reset(nullptr);
		//	
		//}
		m_ComponentMask.reset();
		m_EntityManager->DestroyEntity(m_Id);	
		m_Components.clear();
	}

	void Object::AddChildObj(Object* obj)
	{
		//TODO::Add other components...
		m_ChildObjs.push_back(obj);
		if (this->HasComponent<Transform>()) {
			this->GetComponent<Transform>()->AddChild(obj->GetComponent<Transform>());
		}


	}

}
