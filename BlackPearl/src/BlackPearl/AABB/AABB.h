#pragma once
#include "BlackPearl/Math/vector.h"
#include "BlackPearl/RayTracing/Ray.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {

	class AABB
	{
	public:
		AABB();
		AABB(const donut::math::float3& minP, const donut::math::float3& maxP, bool isValid = true);

		inline donut::math::float3 GetMinP() const { return m_MinP; }
		inline donut::math::float3 GetMaxP() const { return m_MaxP; }
		inline donut::math::float3 GetExtent() const{ return m_MaxP - m_MinP; }
		inline donut::math::float3 GetCenter() const { return (m_MaxP + m_MinP) / 2.0f; }
		inline float GetRadius() const { return length(m_MaxP - m_MinP) / 2.0f; }
		void SetInvalid();
		inline bool IsValid() const { return m_IsValid; }
		bool JudgeValid();
		static const AABB InValid;
		void SetP(const donut::math::float3& minP, const donut::math::float3& maxP);
		const AABB operator +(const AABB & aabb) const;
		const std::shared_ptr<AABB> operator +(const std::shared_ptr<AABB>& aabb) const;

		bool Hit(std::shared_ptr<Ray> ray);
		bool Hit(std::shared_ptr<Ray> ray,float& tMin,float& tMax);
		float GetSurfaceArea() const;
		void Expand(const AABB& aabb);
		void UpdateTransform(glm::mat4 model);
	private:
		bool m_IsValid;
		donut::math::float3 m_MinP;
		donut::math::float3 m_MaxP;


	};

}