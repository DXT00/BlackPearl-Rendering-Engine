#include "pch.h"
#include "Object.h"
#include "BlackPearl/Renderer/CameraComponent/PerspectiveCamera.h"
#include <utility>
namespace BlackPearl {


	
	ComponentMask Object::GetComponentMask() const
	{
		return m_ComponentMask;
	}



	void Object::Destroy()
	{
		//for (auto& pair : m_Components) { //TODO::看一下要不要用智能指针
		//	BaseComponent* component = pair.second;
		//	delete component;
		//	component = nullptr;
		//}
		m_ComponentMask.reset();
		m_EntityManager->DestroyEntity(m_Id);	
		m_Components.clear();
	}

}
