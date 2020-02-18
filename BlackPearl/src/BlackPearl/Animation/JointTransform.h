#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/Math/Quaternion.h"
namespace BlackPearl {

	class JointTransform
	{
	public:
		JointTransform(glm::vec3 pos, Quaternion qua) {
			m_Position = pos;
			m_Quaternion = qua;
		}
		glm::vec3 GetPosition()const { return m_Position; }
		Quaternion GetQuaternion() const{ return m_Quaternion; }

	private:
		Quaternion m_Quaternion;
		glm::vec3 m_Position;
	};
}
