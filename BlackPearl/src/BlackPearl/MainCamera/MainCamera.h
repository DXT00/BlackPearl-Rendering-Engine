#pragma once
#include "BlackPearl/Object/Object.h"
#include "glm/glm.hpp"
/*Logical Camera*/
namespace BlackPearl {
	class MainCamera
	{
	public:
		MainCamera(Object* cameraObj) {
			m_CameraObj = cameraObj;
		}
		~MainCamera() {

		}
		glm::vec3 GetPosition() const;
		float Yaw() const;
		float Pitch() const;
		glm::vec3 Front() const;
		glm::vec3 Up() const;
		glm::vec3 Right() const;
		float Fov() const;

		void SetFov(float fov);
		void SetPosition(glm::vec3 pos);
		void SetRotation(glm::vec3 rotation);
		void SetZNear(float nearZ);
		void SetZFar(float farZ);

		Object* GetObj() const { return m_CameraObj; }
	private:
		Object* m_CameraObj = nullptr;


	};

}


