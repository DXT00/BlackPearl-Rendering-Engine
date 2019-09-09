#include "pch.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "GameEngine/Core.h"
#include <glm/gtc/matrix_transform.hpp>
Camera * Camera::Create(unsigned int type, const std::initializer_list<float> &projectionMatrixProps,
	const ViewMatrixProps &viewMatrixProps)
{
	auto it = projectionMatrixProps.begin();
	switch (type)
	{
	case Orthographic:
		GE_ASSERT(projectionMatrixProps.size() == 4, "parameters' list of Orthographic Camera doesn't match!")
			return new OrthographicCamera(*it, *(it + 1), *(it + 2), *(it + 3), viewMatrixProps);
		break;
	case Perspective:
		GE_ASSERT(projectionMatrixProps.size() == 5, "parameters' list of Perspective Camera doesn't match!")
			return new PerspectiveCamera(*it, *(it + 1), *(it + 2), *(it + 3), *(it + 4), viewMatrixProps);

		break;
	default:
		GE_CORE_ERROR("non defined camera type!")
			break;
	}





}

void Camera::RecalculateViewMatrix()
{

	/*glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)*
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))*
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));*/

	//m_ViewMatrix = glm::inverse(transform);


	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewMatrixProps.Front, m_ViewMatrixProps.Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}
void Camera::RecalculateViewMatrix(float yaw,float pitch)
{

	/*glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)*
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f))*
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));*/

		//m_ViewMatrix = glm::inverse(transform);

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_ViewMatrixProps.Front = glm::normalize(front);


	m_ViewMatrixProps.Right = glm::normalize(glm::cross(m_ViewMatrixProps.Front, m_ViewMatrixProps.WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_ViewMatrixProps.Up = glm::normalize(glm::cross(m_ViewMatrixProps.Right, m_ViewMatrixProps.Front));


	m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewMatrixProps.Front, m_ViewMatrixProps.Up);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}