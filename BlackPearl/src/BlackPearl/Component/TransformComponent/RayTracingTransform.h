#pragma once
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	class Object;
	class RTXTransformNode :public Component<RTXTransformNode>
	{
	public:
		RTXTransformNode(glm::mat4 transform, Object* bvh_obj)
			:Component(Component::Type::RTXTransformNode),
			m_BVHObj(bvh_obj),
			m_TransformMatrix(transform),
			m_InverseTransformMatrix(glm::inverse(transform)),
			m_NormalTransformMatrix(glm::transpose(glm::inverse(glm::mat3(transform))))
		{}

		Object* GetObj() const { return m_BVHObj; }
		glm::mat4 GetTransformMatrix() const { return m_TransformMatrix; }
		glm::mat4 GetInverseTransformMatrix() const { return m_InverseTransformMatrix; }
		glm::mat3 GetNormalTransformMatrix() const { return m_NormalTransformMatrix; }

	private:

		Object* m_BVHObj;
		glm::mat4 m_TransformMatrix;
		glm::mat4 m_InverseTransformMatrix;
		glm::mat3 m_NormalTransformMatrix;

	};



}
