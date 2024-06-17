#include "pch.h"
#include "Transform.h"

namespace BlackPearl {


	Transform::~Transform()
	{
	}
	void Transform::UpdateTransformMatrix()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, GetPosition());
		model = glm::rotate(model, glm::radians(GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, glm::radians(GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, GetScale());

		m_TransformMatrix = model;
	}

}

