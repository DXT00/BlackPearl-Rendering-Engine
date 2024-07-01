#include "pch.h"
#include "Ray.h"

namespace BlackPearl {
	const float Ray::tMin = 10e-4;


	Ray::Ray()
	{
		m_Origin = math::float3(0.0f);
		m_Dir = math::float3(0, 0, -1);
		m_tMax =FLT_MAX;
	}

	Ray::Ray(const math::float3 origin, const math::float3 dir)
	{
		m_Origin = origin;
		m_Dir = dir;
		m_tMax = FLT_MAX;

	}

	void Ray::SetOrigin(const math::float3 origin)
	{
		m_Origin = origin;
	}

	void Ray::SetDir(const math::float3 dir)
	{
		m_Dir = dir;
	}


}
