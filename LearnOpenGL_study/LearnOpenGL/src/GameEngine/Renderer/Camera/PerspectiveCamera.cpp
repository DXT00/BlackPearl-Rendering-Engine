#include "pch.h"
#include "PerspectiveCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float znear, float zfar, const ViewMatrixProps &viewMatrixProps)
	:m_Fov(fov),m_Width(width),m_Height(height),m_zNear(znear),m_zFar(zfar)
{	

	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);


	m_ViewMatrixProps.Front   = viewMatrixProps.Front;
	m_ViewMatrixProps.Up      = viewMatrixProps.Up;
	m_ViewMatrixProps.Right   = viewMatrixProps.Right;
	m_ViewMatrixProps.WorldUp = viewMatrixProps.WorldUp;
	m_ViewMatrixProps.Yaw     = viewMatrixProps.Yaw;
	m_ViewMatrixProps.Pitch   = viewMatrixProps.Pitch;

	RecalculateViewMatrix();

}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}
