#pragma once
#include "glm/glm.hpp"
namespace BlackPearl {

	class Ray
	{
	public:
		Ray();
		Ray(const glm::vec3 origin, const glm::vec3 dir);
		glm::vec3 GetOrigin() const { return m_Origin; }
		glm::vec3 GetDir() const { return m_Dir; }
		float GetTMax() const { return m_tMax; }
		void SetOrigin(const glm::vec3 origin);
		void SetDir(const glm::vec3 dir);
		static const float tMin;
	private:
		glm::vec3 m_Origin;
		glm::vec3 m_Dir;
		float m_tMax;
		
	};
}

