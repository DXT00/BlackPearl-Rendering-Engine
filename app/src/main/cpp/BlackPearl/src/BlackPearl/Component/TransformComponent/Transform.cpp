#include "pch.h"
#include "Component/TransformComponent/Transform.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
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

	void Transform::SetTransformMatrix(glm::mat4 worldMatrix)
	{
		m_TransformMatrix = worldMatrix;


		m_LastPosition = m_Position;
		m_LastRotation = m_Rotation;
		m_LastScale = m_Scale;

		m_Position = glm::vec3{ worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2] };
		m_Rotation = glm::degrees(glm::eulerAngles(glm::quat_cast(worldMatrix)));

		glm::vec3 skew;
		glm::vec3 localScale;
		glm::quat localOrientation;
		glm::vec3 localPosition;
		glm::vec4 perspective;
		glm::decompose(worldMatrix, localScale, localOrientation, localPosition, skew, perspective);

		m_Scale = localScale;

	}

}

