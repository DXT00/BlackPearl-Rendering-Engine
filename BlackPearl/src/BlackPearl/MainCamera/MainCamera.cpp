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
	float MainCamera::Fov() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetFov();
	}
	void MainCamera::SetFov(float fov)
	{
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetFov(fov);
	}
	void MainCamera::SetZFar(float zFar) {
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetZfar(zFar);
	}
	void MainCamera::SetZNear(float zNear) {
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetZnear(zNear);

	}
	void MainCamera::SetMoveSpeed(float speed) {
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetMoveSpeed(speed);

	}
	void MainCamera::SetRotateSpeed(float speed) {
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetRotateSpeed(speed);
	}

	float MainCamera::GetMoveSpeed() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetMoveSpeed();
	}

	float MainCamera::GetRotateSpeed() const
	{
		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetRotateSpeed();
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
