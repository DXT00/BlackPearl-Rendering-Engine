#pragma once

#include "BlackPearl/Component/Component.h"
#include "BlackPearl/Math/vector.h"
namespace BlackPearl {

	struct BoundingSphere: public Component<class BoundingSphere>
	{
	public:
		BoundingSphere()
		: Component(Component::Type::BoundingSphere) {
		};

		BoundingSphere(donut::math::float3 center, float radius)
		: Component(Component::Type::BoundingSphere) {
			m_Center = center;
			m_Radius = radius;
		}
		static void CreateFromPoints(BoundingSphere &sphere, size_t Count, donut::math::float3* pPoints, size_t Stride);
		static void CreateMerged(BoundingSphere& Out, const BoundingSphere& S1, const BoundingSphere& S2);

	public:

        donut::math::float3 m_Center;            // Center of the sphere.
		float m_Radius;               // Radius of the sphere.
		
	};
}

