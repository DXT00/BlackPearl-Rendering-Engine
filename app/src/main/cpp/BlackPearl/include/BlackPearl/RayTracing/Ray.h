#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/Math/vector.h"
namespace BlackPearl {

	class Ray
	{
	public:
		Ray();
		Ray(const math::float3 origin, const math::float3 dir);
		math::float3 GetOrigin() const { return m_Origin; }
		math::float3 GetDir() const { return m_Dir; }
		float GetTMax() const { return m_tMax; }
		void SetOrigin(const math::float3 origin);
		void SetDir(const math::float3 dir);
		static const float tMin;
	private:
		math::float3 m_Origin;
		math::float3 m_Dir;
		float m_tMax;
		
	};
}

