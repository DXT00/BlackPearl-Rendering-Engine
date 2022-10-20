#pragma once
#include "BlackPearl/Component/Component.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace BlackPearl {

	struct BoundingSphere: public Component<class BoundingSphere>
	{
	public:
		BoundingSphere()
		: Component(Component::Type::BoundingSphere) {
		};

		BoundingSphere(XMFLOAT3 center, float radius)
		: Component(Component::Type::BoundingSphere) {
			m_Center = center;
			m_Radius = radius;
		}
		static void CreateFromPoints(BoundingSphere &sphere, size_t Count, const XMFLOAT3* pPoints, size_t Stride);
		static void BoundingSphere::CreateMerged(BoundingSphere& Out, const BoundingSphere& S1, const BoundingSphere& S2);

	public:
		
		XMFLOAT3 m_Center;            // Center of the sphere.
		float m_Radius;               // Radius of the sphere.
		
	};
}


