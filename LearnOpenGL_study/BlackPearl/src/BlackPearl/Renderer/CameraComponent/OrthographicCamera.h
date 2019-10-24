#pragma once
#include "glm/glm.hpp"
#include"Camera.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {

	class OrthographicCamera :public Camera
	{
	public:
		OrthographicCamera(EntityManager * entityManager, Entity::Id id, const ViewMatrixProps &viewMatrixProps = ViewMatrixProps())
			:Camera(entityManager,id),m_Left(-1.6f), m_Right(1.6f), m_Bottom(-0.9f), m_Top(0.9f) {
		
			m_ProjectionMatrix = glm::ortho(m_Left, m_Right, m_Bottom, m_Top, -1.0f, 1.0f);


			m_ViewMatrixProps.Front = viewMatrixProps.Front;
			m_ViewMatrixProps.Up = viewMatrixProps.Up;
			m_ViewMatrixProps.Right = viewMatrixProps.Right;
			m_ViewMatrixProps.WorldUp = viewMatrixProps.WorldUp;
			m_ViewMatrixProps.Yaw = viewMatrixProps.Yaw;
			m_ViewMatrixProps.Pitch = viewMatrixProps.Pitch;

			RecalculateViewMatrix();
		
		
		}
		virtual ~OrthographicCamera() = default;


	private:
		float m_Left,m_Right;
		float m_Top, m_Bottom;
	};
}
