#include "pch.h"
#include "MainCamera.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"

namespace BlackPearl {
	glm::vec3 MainCamera::GetPosition() const
	{
		return m_CameraObj->GetComponent<Transform>()->GetPosition();
	}
	
	float MainCamera::Yaw() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->Yaw();
	}
	float MainCamera::Pitch() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->Pitch();
	}
	glm::vec3 MainCamera::Front() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->Front();
	}
	glm::vec3 MainCamera::Up() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->Up();
	}
	glm::vec3 MainCamera::Right() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->Right();
	}
	void MainCamera::SetPosition(glm::vec3 pos)
	{
		m_CameraObj->GetComponent<Transform>()->SetPosition(pos);
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetPositionAndUpdateMatrix(pos);
	}

	void MainCamera::SetRotation(glm::vec3 rotation)
	{
		m_CameraObj->GetComponent<Transform>()->SetRotation(rotation);
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetRotationAndUpdateMatrix(rotation.y,rotation.x);
	}

}
