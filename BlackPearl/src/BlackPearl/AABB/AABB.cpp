#include "pch.h"
#include "AABB.h"
#include <math.h>
#include "BlackPearl/RayTracing/Ray.h"


namespace BlackPearl {
	const AABB AABB::InValid(glm::vec3(-1), glm::vec3(-1), false);

	AABB::AABB()
	{
		*this = InValid;
	}
	
	AABB::AABB(const glm::vec3& minP, const glm::vec3& maxP, bool isValid)
		: m_MinP(minP),m_MaxP(maxP),m_IsValid(isValid)
	{

	}

	void AABB::SetInvalid()
	{
		m_IsValid = false;
		*this = InValid;
	}

	void AABB::SetP(const glm::vec3& minP, const glm::vec3& maxP)
	{
	}

	const AABB AABB::operator+(const AABB& aabb) const
	{
		if (m_IsValid) {
			if (aabb.IsValid()) {
				glm::vec3 minP = glm::min(aabb.GetMinP(), m_MinP);
				glm::vec3 maxP = glm::max(aabb.GetMaxP(), m_MaxP);
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
		glm::vec3& dir = ray->GetDir();
		glm::vec3& org = ray->GetOrigin();


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
				tMin = glm::max(tMin,t0);
				tMin = glm::min(tMax,t1);
			}


		}

		if (tMin >= tMax)
			return false;

		return true;
	}

	float AABB::GetSurfaceArea() const
	{
		glm::vec3 extends = GetExtent();
		return 2.0f * (extends.x * extends.y + extends.y * extends.z + extends.z * extends.x);
	}

	void AABB::Expand(const AABB& aabb)
	{
		if (aabb.IsValid()) {
			if (m_IsValid) {
				m_MinP = glm::min(m_MinP, aabb.GetMinP());
				m_MaxP = glm::max(m_MaxP, aabb.GetMaxP());	
			}
			else {
				m_MinP = aabb.GetMinP();
				m_MaxP = aabb.GetMaxP();
			}
			m_IsValid = true;
		}
		
		
	}

}
