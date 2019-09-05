#include "pch.h"
#include "Camera.h"
#include"OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "GameEngine/Core.h"
#include <glm/gtc/matrix_transform.hpp>
Camera * Camera::Create(unsigned int type, std::initializer_list<float> l)
{
	auto it = l.begin();
	switch (type)
	{
	case Orthographic:
		GE_ASSERT(l.size()== 4,"parameters' list of Orthographic Camera doesn't match!")
		return new OrthographicCamera(*it,*(it+1),*(it+2),*(it+3));
		break;
	case Perspective:
		GE_ASSERT(l.size() == 5, "parameters' list of Perspective Camera doesn't match!")
		return new PerspectiveCamera(*it, *(it + 1), *(it + 2), *(it + 3), *(it + 4));

		break;
	default:
		GE_CORE_ERROR("non defined camera type!")
		break;
	}
}

void Camera::RecalculateViewMatrix()
{

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position)*
		glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

	m_ViewMatrix = glm::inverse(transform);
	m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;

}
