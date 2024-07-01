#include "pch.h"
#include "AABB.h"

#include "BlackPearl/RayTracing/Ray.h"
#include "BlackPearl/Math/Math.h"

namespace BlackPearl {

	const AABB AABB::InValid(math::float3(-1), math::float3(-1), true, false);

	AABB::AABB()
	{
		*this = InValid;
	}
	
	AABB::AABB(const math::float3& minPOrCenter, const math::float3& maxPOrExtent, bool setMinMax, bool IsValid)
	{
		m_IsValid = IsValid;

		if (setMinMax) {
			m_MinP = minPOrCenter;
			m_MaxP = maxPOrExtent;
			_CalculateCenterAndExtent();
		}
		else {
			m_Center = minPOrCenter;
			m_Extent = maxPOrExtent;

			m_MinP = m_Center - m_Extent / 2.0f;
			m_MaxP = m_Center + m_Extent / 2.0f;
		}
		//m_IsValid = true;
	}

	void AABB::SetInvalid()
	{
		m_IsValid = false;
		*this = InValid;
	}

	void AABB::SetP(const math::float3& minP, const math::float3& maxP)
	{
		m_MinP = minP;
		m_MaxP = maxP;
		_CalculateCenterAndExtent();

	}

	const AABB AABB::operator+(const AABB& aabb) const
	{
		if (m_IsValid) {
			if (aabb.IsValid()) {
				math::float3 minP = math::min(aabb.GetMinP(), m_MinP);
				math::float3 maxP = math::max(aabb.GetMaxP(), m_MaxP);
				return { minP,maxP,true };
			}
			else {
				return *this;
			}
		}
		else {
			if (aabb.IsValid()) {
				return aabb;
			}
			else
				return InValid;
		}
	}


	bool AABB::Hit(std::shared_ptr<Ray> ray)
	{	
		float tMin,tMax;
		return Hit(ray,tMin,tMax);
	}

	bool AABB::Hit(std::shared_ptr<Ray> ray, float& tMin, float& tMax)
	{
		if (!IsValid())
			return false;

		tMin = Ray::tMin;
		tMax = ray->GetTMax();
		math::float3& dir = ray->GetDir();
		math::float3& org = ray->GetOrigin();


		for (int i = 0; i < 3; i++)
		{
			float t0, t1;
			if (dir[i] == 0) {
				if (org[i] != m_MinP[i] && org[i] != m_MaxP[i])
					return false;
			}
			else {
				t0= (m_MinP[i] - org[i]) / dir[i];
				t1= (m_MaxP[i] - org[i]) / dir[i];

				if (dir[i] < 0.0f)
					std::swap(t0, t1);
				tMin = math::max(tMin,t0);
				tMin = math::min(tMax,t1);
			}


		}

		if (tMin >= tMax)
			return false;

		return true;
	}

	float AABB::GetSurfaceArea() const
	{
		math::float3 extends = GetExtent();
		return 2.0f * (extends.x * extends.y + extends.y * extends.z + extends.z * extends.x);
	}

	void AABB::Expand(const AABB& aabb)
	{
		if (aabb.IsValid()) {
			if (m_IsValid) {
				m_MinP = math::min(m_MinP, aabb.GetMinP());
				m_MaxP = math::max(m_MaxP, aabb.GetMaxP());	
			}
			else {
				m_MinP = aabb.GetMinP();
				m_MaxP = aabb.GetMaxP();
			}
			_CalculateCenterAndExtent();
			m_IsValid = true;
		}
		
		
	}

	void AABB::UpdateTransform(glm::mat4 model)
	{
		glm::vec4 min = model * glm::vec4(m_MinP.x, m_MinP.y, m_MinP.z,1);
		glm::vec4 max = model * glm::vec4(m_MaxP.x, m_MaxP.y, m_MaxP.z,1);

		m_MinP = math::float3(min.r, min.b, min.g);
		m_MaxP = math::float3(max.r, max.b, max.g);
		_CalculateCenterAndExtent();
	}

	void AABB::_CalculateCenterAndExtent()
	{
		m_Center = (m_MinP + m_MaxP) * 0.5f;
		m_Extent =  m_MaxP - m_MinP;
	}

}
