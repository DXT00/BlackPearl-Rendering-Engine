#include "pch.h"
#include "Ray.h"

namespace BlackPearl {
	const float Ray::tMin = 10e-4;


	Ray::Ray()
	{
		m_Origin = glm::vec3(0);
		m_Dir = glm::vec3(0, 0, -1);
		m_tMax =FLT_MAX;
	}

	Ray::Ray(const glm::vec3 origin, const glm::vec3 dir)
	{
		m_Origin = origin;
		m_Dir = dir;
		m_tMax = FLT_MAX;

	}

	void Ray::SetOrigin(const glm::vec3 origin)
	{
		m_Origin = origin;
	}

	void Ray::SetDir(const glm::vec3 dir)
	{
		m_Dir = dir;
	}


}
