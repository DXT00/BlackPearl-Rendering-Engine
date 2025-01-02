#include "pch.h"
#include "Camera.h"
#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "BlackPearl/Core.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Math/Math.h"

namespace BlackPearl {

	Camera * Camera::Create(unsigned int type)
	
	{
		
		switch (type)
		{
		case Orthographic:
			return DBG_NEW OrthographicCamera();
			break;
		case Perspective:
			return DBG_NEW PerspectiveCamera();
			break;
		default:
			GE_CORE_ERROR("non defined camera type!")
				break;
		}
		return nullptr;
	}

	void Camera::RecalculateViewMatrix()
	{

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewMatrixProps.Front, m_ViewMatrixProps.Up);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}
	void Camera::RecalculateViewMatrix(float yaw, float pitch)
	{
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		m_ViewMatrixProps.Front = glm::normalize(front);


		m_ViewMatrixProps.Right = glm::normalize(glm::cross(m_ViewMatrixProps.Front, m_ViewMatrixProps.WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_ViewMatrixProps.Up = glm::normalize(glm::cross(m_ViewMatrixProps.Right, m_ViewMatrixProps.Front));


		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_ViewMatrixProps.Front, m_ViewMatrixProps.Up);
		m_ProjectionViewMatrix = m_ProjectionMatrix * m_ViewMatrix;

	}

	void Camera::SwitchToFace(unsigned int i)
	{	//服从右手坐标系（食指指向front，大拇指指向up）
		switch (i)
		{
		case 0://+x yaw pitch
			SetRotationAndUpdateMatrix(0.0f, 0.0f);
			break;
		case 1://-x
			SetRotationAndUpdateMatrix(-180.0f, 0.0f);
			break;
		case 2://+y
			SetRotationAndUpdateMatrix(0.0f, 90.0f);
			break;
		case 3://-y
			SetRotationAndUpdateMatrix(90.0f, -90.0f);
			break;
		case 4://+z
			SetRotationAndUpdateMatrix(90.0f, 0.0f);
			break;
		case 5://-z
			SetRotationAndUpdateMatrix(-90.0f, 0.0f);
			break;
		default:
			break;
		}
	}

}
