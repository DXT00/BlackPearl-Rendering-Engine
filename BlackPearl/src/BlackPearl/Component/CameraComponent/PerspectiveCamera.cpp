#include "pch.h"
#include "PerspectiveCamera.h"
#include <glm/glm.hpp>
#include "BlackPearl/Config.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {
	//float fov, float width, float height, float znear, float zfar,
	PerspectiveCamera::PerspectiveCamera(EntityManager * entityManager, Entity::Id id, const ViewMatrixProps &viewMatrixProps)
		:Camera(entityManager, id),m_Fov(45.0f), m_Width(Configuration::WindowWidth), m_Height(Configuration::WindowHeight), m_zNear(0.1f), m_zFar(100.0f)
	{

		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);


		m_ViewMatrixProps.Front = viewMatrixProps.Front;
		m_ViewMatrixProps.Up = viewMatrixProps.Up;
		m_ViewMatrixProps.Right = viewMatrixProps.Right;
		m_ViewMatrixProps.WorldUp = viewMatrixProps.WorldUp;
		m_ViewMatrixProps.Yaw = viewMatrixProps.Yaw;
		m_ViewMatrixProps.Pitch = viewMatrixProps.Pitch;

		RecalculateViewMatrix();

	}

	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
}