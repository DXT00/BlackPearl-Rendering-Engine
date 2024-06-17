#include "pch.h"
#include "Object.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include <utility>
#include "glm/ext/matrix_transform.hpp"

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
		
	}

	void Object::SetPosition(glm::vec3 pos)
	{
		glm::vec3 lastPos = GetComponent<Transform>()->GetPosition();
		glm::vec3 deltaPos = pos - lastPos;

		GetComponent<Transform>()->SetPosition(pos);
		
		for (Object* child : m_ChildObjs) {

			glm::vec3 ChildLastPos = child->GetComponent<Transform>()->GetPosition();
			child->GetComponent<Transform>()->SetPosition(ChildLastPos+deltaPos);
		}

	}

	void Object::SetRotation(glm::vec3 rotate)
	{
		glm::vec3 lastRotate = GetComponent<Transform>()->GetRotation();
		glm::vec3 deltaRotate = rotate - lastRotate;
		GetComponent<Transform>()->SetRotation(rotate);
	//	glm::mat4 transformMatrix = GetComponent<Transform>()->GetTransformMatrix();
		for (Object* child : m_ChildObjs) {
			
			glm::vec3 ChildLastRotate = child->GetComponent<Transform>()->GetRotation();

			child->GetComponent<Transform>()->SetRotation(ChildLastRotate+deltaRotate);
		}
	}

	void Object::SetScale(glm::vec3 scale)
	{
		glm::vec3 lastScale = GetComponent<Transform>()->GetScale();
		glm::vec3 deltaScale = scale/lastScale ;
		GetComponent<Transform>()->SetScale(scale);

		for (Object* child : m_ChildObjs) {

			glm::vec3 ChildLastScale = child->GetComponent<Transform>()->GetScale();

			child->GetComponent<Transform>()->SetScale(ChildLastScale*deltaScale);
		}
	}

	void Object::AddChildObj(Object* obj)
	{
		//TODO::Add other components...
		obj->SetParent(this);
		m_ChildObjs.push_back(obj);
		
		/*glm::vec3 posDelta = GetComponent<Transform>()->GetPosition() - obj->GetComponent<Transform>()->GetPosition();
		glm::vec3 rotateDelta = GetComponent<Transform>()->GetRotation() - obj->GetComponent<Transform>()->GetRotation();
		glm::vec3 scaleDelta = glm::vec3(
			GetComponent<Transform>()->GetScale().x / obj->GetComponent<Transform>()->GetScale().x,
			GetComponent<Transform>()->GetScale().y / obj->GetComponent<Transform>()->GetScale().y,
			GetComponent<Transform>()->GetScale().z / obj->GetComponent<Transform>()->GetScale().z
		);
		glm::mat4 transformToParent = glm::mat4(1.0);
		transformToParent = glm::translate(transformToParent, posDelta);
		transformToParent = glm::rotate(transformToParent, glm::radians(rotateDelta.x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformToParent = glm::rotate(transformToParent, glm::radians(rotateDelta.y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformToParent = glm::rotate(transformToParent, glm::radians(rotateDelta.z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformToParent = glm::scale(transformToParent, scaleDelta);

		obj->SetTransformToParent(transformToParent);*/
	}

	Object* Object::GetChildByFrontName(std::string name) const
	{
		for (Object* obj:m_ChildObjs)
		{
			if (obj->GetFrontName() == name) {
				return obj;
			}
		}
		return nullptr;
	}

}
