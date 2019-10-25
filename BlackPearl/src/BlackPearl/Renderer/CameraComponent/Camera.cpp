#include "pch.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "BlackPearl/Core.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Component/Component.h"
namespace BlackPearl {
//const std::initializer_list<float> &projectionMatrixProps

	Camera * Camera::Create(EntityManager * entityManager, Entity::Id id, unsigned int type)
	
	{
		//m_EntityManager = entityManager;
		//m_Id = id;
		//auto it = projectionMatrixProps.begin();
		switch (type)
		{
		case Orthographic:
			//GE_ASSERT(sizeof...(Args) == 4, "parameters' list of Orthographic Camera doesn't match!")
			return DBG_NEW OrthographicCamera(entityManager, id);
			break;
		case Perspective:
			//GE_ASSERT(sizeof...(Args) == 5, "parameters' list of Perspective Camera doesn't match!")
			return DBG_NEW PerspectiveCamera(entityManager, id);

			break;
		default:
			GE_CORE_ERROR("non defined camera type!")
				break;
		}
		return nullptr;
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
	void Camera::RecalculateViewMatrix(float yaw, float pitch)
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

}
