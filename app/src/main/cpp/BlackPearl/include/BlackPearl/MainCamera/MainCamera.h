#pragma once
#include "BlackPearl/Object/Object.h"
#include "glm/glm.hpp"
/*Logical Camera*/
namespace BlackPearl {
    struct CameraRotation {
        float Yaw;
        float Pitch;
        CameraRotation() {
            Yaw = 0.0f;
            Pitch = 0.0f;
        }
        CameraRotation(float yaw, float pitch) {
            Yaw = yaw;
            Pitch = pitch;
        }
    };
	class MainCamera
	{
	public:
		MainCamera(Object* cameraObj) {
			m_CameraObj = cameraObj;
		}
		~MainCamera() {

		}
		glm::vec3 GetPosition() const;
        glm::vec3 GetRotation() const;
		float Yaw() const;
		float Pitch() const;
		glm::vec3 Front() const;
		glm::vec3 Up() const;
		glm::vec3 Right() const;
		float Fov() const;
		float ZFar() const;
		float ZNear() const;

		void SetFov(float fov);
		void SetPosition(glm::vec3 pos);
		void SetRotation(glm::vec3 rotation);
		void SetZNear(float nearZ);
		void SetZFar(float farZ);
		void SetMoveSpeed(float speed);
		void SetRotateSpeed(float speed);
		float GetMoveSpeed() const;
		float GetRotateSpeed() const;
		Object* GetObj() const { return m_CameraObj; }
	private:
		Object* m_CameraObj = nullptr;


	};

}


