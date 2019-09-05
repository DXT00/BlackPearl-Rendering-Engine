#include "pch.h"
#include "PerspectiveCamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


PerspectiveCamera::PerspectiveCamera(float fov, float width, float height, float znear, float zfar)
	:m_Fov(fov),m_Width(width),m_Height(height),m_zNear(znear),m_zFar(zfar)
{	

	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);
	m_ViewMatrix = glm::mat4(1.0f);

	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}

void PerspectiveCamera::RecalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspective(glm::radians(m_Fov), m_Width / m_Height, m_zNear, m_zFar);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}
