#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
namespace BlackPearl {
	class Transform : public Component<Transform>
	{
	public:
		Transform()
			:Component(Component::Type::Transform) {}
		~Transform();

		glm::vec3 GetPosition()const { return m_Position; }
		glm::vec3 GetRotation()const { return m_Rotation; }
		glm::vec3 GetScale()   const { return m_Scale;    }

		glm::vec3 GetLastPosition()const { return m_LastPosition; }
		glm::vec3 GetLastRotation()const { return m_LastRotation; }
		glm::vec3 GetLastScale()const { return m_LastScale; }

		glm::mat4 GetTransformMatrix()const { return m_TransformMatrix; }
		
		void UpdateTransformMatrix();
		void SetLastPosition(const glm::vec3& position) { m_LastPosition = position; }

		void SetPosition(const glm::vec3& position) { m_LastPosition = m_Position; m_Position.x = position.x, m_Position.y = position.y, m_Position.z = position.z; UpdateTransformMatrix(); }
		void SetInitPosition(const glm::vec3& position) { m_LastPosition = position; m_Position = position; UpdateTransformMatrix(); }

		void SetRotation(const glm::vec3& rotation) { m_LastRotation = m_Rotation,  m_Rotation.x = rotation.x, m_Rotation.y = rotation.y, m_Rotation.z = rotation.z; UpdateTransformMatrix();}
		void SetInitRotation(const glm::vec3& rotation) { m_LastRotation = rotation, m_Rotation.x = rotation.x, m_Rotation.y = rotation.y, m_Rotation.z = rotation.z; UpdateTransformMatrix(); }

		void SetScale(const glm::vec3&scale)        { m_LastScale = m_Scale,m_Scale.x    = scale.x,    m_Scale.y = scale.y,       m_Scale.z = scale.z; UpdateTransformMatrix();}
		void SetInitScale(const glm::vec3& scale) {  m_LastScale = scale, m_Scale.x = scale.x, m_Scale.y = scale.y, m_Scale.z = scale.z; UpdateTransformMatrix(); }


	private:
		glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
		glm::vec3 m_LastPosition = { 0.0f,0.0f,0.0f };

		glm::vec3 m_Rotation = { 0.0f,0.0f,0.0f };//单位：角度
		glm::vec3 m_LastRotation = { 0.0f,0.0f,0.0f };//单位：角度

		glm::vec3 m_Scale    = { 1.0f,1.0f,1.0f };
		glm::vec3 m_LastScale = { 1.0f,1.0f,1.0f };

		glm::mat4 m_TransformMatrix = glm::mat4(1.0f);

	};
}

