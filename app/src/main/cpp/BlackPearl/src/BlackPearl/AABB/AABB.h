#pragma once
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/RayTracing/Ray.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {

	class AABB
	{

	public:
		AABB();
		//AABB(const math::float3& pos, const math::float3& extent)
		//	: m_Center(pos), m_Extent(extent), m_IsValid(true)
		//{
		//	m_MinP = math:: float3(-1);
		//	m_MaxP = math::float3(-1);

		//}

		AABB(const math::float3& minPOrCenter, const math::float3& maxPOrExtent, bool setMinMax = false, bool IsValid = true);

		inline math::float3 GetMinP() const { return m_MinP; }
		inline math::float3 GetMaxP() const { return m_MaxP; }
		inline math::float3 GetExtent() const { return m_Extent; }
		inline math::float3 GetCenter() const { return (m_MaxP + m_MinP) / 2.0f; }
		
		inline float GetRadius() const { return length(m_MaxP - m_MinP) / 2.0f; }
		void SetInvalid();
		inline bool IsValid() const { return m_IsValid; }
		bool JudgeValid();
		static const AABB InValid;
		void SetP(const math::float3& minP, const math::float3& maxP);
		const AABB operator +(const AABB & aabb) const;
		const std::shared_ptr<AABB> operator +(const std::shared_ptr<AABB>& aabb) const;

		bool Hit(std::shared_ptr<Ray> ray);
		bool Hit(std::shared_ptr<Ray> ray,float& tMin,float& tMax);
		float GetSurfaceArea() const;
		void Expand(const AABB& aabb);
		void UpdateTransform(glm::mat4 model);

	public:
		math::float3 m_Center;
		math::float3 m_Extent;
	private:
		
		void _CalculateCenterAndExtent();
		bool m_IsValid;
		math::float3 m_MinP;
		math::float3 m_MaxP;


	};

}