#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/RayTracing/Triangle.h"
#include "BlackPearl/Scene/BoundingBoxBuilder.h"

namespace BlackPearl {
	class BoundingBox: public Component<BoundingBox>
	{
	public:
		BoundingBox(AABB box) 
			:Component(Component::Type::BoundingBox),m_Box(box){
		};
		const AABB& Get() const { return m_Box; }
		void SetBox(const AABB& box) {
			m_Box = box;
		}
	private:
		AABB m_Box;

	};

}

