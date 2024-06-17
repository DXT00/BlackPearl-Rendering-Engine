#pragma once
#include "glm/glm.hpp"
#include "BlackPearl/RayTracing/Ray.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {

	class AABB
	{
	public:
		AABB();
		AABB(const glm::vec3& minP, const glm::vec3& maxP, bool isValid = true);

		inline glm::vec3 GetMinP() const { return m_MinP; }
		inline glm::vec3 GetMaxP() const { return m_MaxP; }
		inline glm::vec3 GetExtent() const{ return m_MaxP - m_MinP; }
		inline glm::vec3 GetCenter() const { return (m_MaxP + m_MinP) / 2.0f; }
		inline float GetRadius() const { return length(m_MaxP - m_MinP) / 2.0f; }
		void SetInvalid();
		inline bool IsValid() const { return m_IsValid; }
		bool JudgeValid();
		static const AABB InValid;
		void SetP(const glm::vec3& minP, const glm::vec3& maxP);
		const AABB operator +(const AABB & aabb) const;
		const std::shared_ptr<AABB> operator +(const std::shared_ptr<AABB>& aabb) const;

		bool Hit(std::shared_ptr<Ray> ray);
		bool Hit(std::shared_ptr<Ray> ray,float& tMin,float& tMax);
		float GetSurfaceArea() const;
		void Expand(const AABB& aabb);
		void UpdateTransform(glm::mat4 model);
	private:
		bool m_IsValid;
		glm::vec3 m_MinP;
		glm::vec3 m_MaxP;


	};

}