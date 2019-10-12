#include "pch.h"
#include "OrthographicCamera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, const ViewMatrixProps &viewMatrixProps)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);


		m_ViewMatrixProps.Front = viewMatrixProps.Front;
		m_ViewMatrixProps.Up = viewMatrixProps.Up;
		m_ViewMatrixProps.Right = viewMatrixProps.Right;
		m_ViewMatrixProps.WorldUp = viewMatrixProps.WorldUp;
		m_ViewMatrixProps.Yaw = viewMatrixProps.Yaw;
		m_ViewMatrixProps.Pitch = viewMatrixProps.Pitch;

		RecalculateViewMatrix();
	}

}