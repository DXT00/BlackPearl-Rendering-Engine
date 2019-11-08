#pragma once
#include "BlackPearl/Component/Component.h"
#include "glm/glm.hpp"
#include "glm/ext/matrix_transform.hpp"
namespace BlackPearl {
	class Transform :public Component<Transform>
	{
	public:
		Transform(EntityManager* entityManager, Entity::Id id)
			:Component(entityManager, id,Component::Type::Transform) {}
		~Transform();

		glm::vec3 GetPosition()const { return m_Position; }
		glm::vec3 GetRotation()const { return m_Rotation; }
		glm::vec3 GetScale()   const { return m_Scale;    }
		glm::mat4 GetTransformMatrix()const { return m_TransformMatrix; }
		
		void UpdateTransformMatrix();

		void SetPosition(const glm::vec3& position) { m_Position.x = position.x, m_Position.y = position.y, m_Position.z = position.z;UpdateTransformMatrix();}
		void SetRotation(const glm::vec3& rotation) { m_Rotation.x = rotation.x, m_Rotation.y = rotation.y, m_Rotation.z = rotation.z; UpdateTransformMatrix();}
		void SetScale(const glm::vec3&scale)        { m_Scale.x    = scale.x,    m_Scale.y = scale.y,       m_Scale.z = scale.z; UpdateTransformMatrix();}
	private:
		glm::vec3 m_Position = { 0.0f,0.0f,0.0f };
		glm::vec3 m_Rotation = { 0.0f,0.0f,0.0f };//µ¥Î»£º½Ç¶È
		glm::vec3 m_Scale    = { 1.0f,1.0f,1.0f };
		glm::mat4 m_TransformMatrix = glm::mat4(1.0f);
	};
}

