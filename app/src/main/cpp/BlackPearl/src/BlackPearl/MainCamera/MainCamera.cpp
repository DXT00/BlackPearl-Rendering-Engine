#include "pch.h"
#include "MainCamera/MainCamera.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "Core.h"
namespace BlackPearl {
	glm::vec3 MainCamera::GetPosition() const
	{
		return m_CameraObj->GetComponent<Transform>()->GetPosition();
	}
	
    glm::vec3 MainCamera::GetRotation() const {
        if (m_Type == Camera::CameraType::Perspective)
            return m_CameraObj->GetComponent<PerspectiveCamera>()->GetRotation();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<Transform>()->GetRotation();

    }

	float MainCamera::Yaw() const
	{
        if (m_Type == Camera::CameraType::Perspective)
		    return m_CameraObj->GetComponent<PerspectiveCamera>()->Yaw();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<OrthographicCamera>()->Yaw();

	}
	float MainCamera::Pitch() const
	{
        if (m_Type == Camera::CameraType::Perspective)
		    return m_CameraObj->GetComponent<PerspectiveCamera>()->Pitch();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<OrthographicCamera>()->Pitch();
	}
	glm::vec3 MainCamera::Front() const
	{
        if (m_Type == Camera::CameraType::Perspective)
		    return m_CameraObj->GetComponent<PerspectiveCamera>()->Front();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<OrthographicCamera>()->Front();
	}
	glm::vec3 MainCamera::Up() const
	{
        if (m_Type == Camera::CameraType::Perspective)
            return m_CameraObj->GetComponent<PerspectiveCamera>()->Up();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<OrthographicCamera>()->Up();

	}
	glm::vec3 MainCamera::Right() const
	{
        if (m_Type == Camera::CameraType::Perspective)
		    return m_CameraObj->GetComponent<PerspectiveCamera>()->Right();
        else if (m_Type == Camera::CameraType::Orthographic)
            return m_CameraObj->GetComponent<OrthographicCamera>()->Right();

	}
	float MainCamera::Fov() const
	{
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);
		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetFov();
	}
	void MainCamera::SetFov(float fov)
	{
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);

		m_CameraObj->GetComponent<PerspectiveCamera>()->SetFov(fov);
	}

    float MainCamera::ZFar() const {
        if (m_Type == Camera::CameraType::Perspective)
            return m_CameraObj->GetComponent<PerspectiveCamera>()->GetZfar();
        else if (m_Type == Camera::CameraType::Orthographic)
            return 1.0;
    }
    float MainCamera::ZNear() const {
        if (m_Type == Camera::CameraType::Perspective)
            return m_CameraObj->GetComponent<PerspectiveCamera>()->GetZnear();
        else if (m_Type == Camera::CameraType::Orthographic)
            return -1.0;

	}
	void MainCamera::SetZFar(float zFar) {
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetZfar(zFar);
	}
	void MainCamera::SetZNear(float zNear) {
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);
		m_CameraObj->GetComponent<PerspectiveCamera>()->SetZnear(zNear);

	}
	void MainCamera::SetMoveSpeed(float speed) {
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);

		m_CameraObj->GetComponent<PerspectiveCamera>()->SetMoveSpeed(speed);

	}
	void MainCamera::SetRotateSpeed(float speed) {
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);

		m_CameraObj->GetComponent<PerspectiveCamera>()->SetRotateSpeed(speed);
	}

	float MainCamera::GetMoveSpeed() const
	{
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);

		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetMoveSpeed();
	}

	float MainCamera::GetRotateSpeed() const
	{
        GE_ASSERT(m_Type == Camera::CameraType::Perspective);

		return m_CameraObj->GetComponent<PerspectiveCamera>()->GetRotateSpeed();
	}

	void MainCamera::SetPosition(glm::vec3 pos)
	{
		m_CameraObj->GetComponent<Transform>()->SetPosition(pos);
        if(m_Type == Camera::CameraType::Perspective)
		    m_CameraObj->GetComponent<PerspectiveCamera>()->SetPositionAndUpdateMatrix(pos);
        else if (m_Type == Camera::CameraType::Orthographic)
            m_CameraObj->GetComponent<OrthographicCamera>()->SetPositionAndUpdateMatrix(pos);

	}

	void MainCamera::SetRotation(glm::vec3 rotation)
	{
		m_CameraObj->GetComponent<Transform>()->SetRotation(rotation);
        if (m_Type == Camera::CameraType::Perspective)
		    m_CameraObj->GetComponent<PerspectiveCamera>()->SetRotationAndUpdateMatrix(rotation.y,rotation.x);
        else if (m_Type == Camera::CameraType::Orthographic)
            m_CameraObj->GetComponent<OrthographicCamera>()->SetRotationAndUpdateMatrix(rotation.y, rotation.x);

	}

}
