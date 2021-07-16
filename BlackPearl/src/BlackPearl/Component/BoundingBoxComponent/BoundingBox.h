#pragma once
#include "BlackPearl/Component/Component.h"
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/BVHNode/BVHNode.h"
#include "BlackPearl/RayTracing/Triangle.h"
#include "BlackPearl/SceneBuilder/BoundingBoxBuilder.h"

namespace BlackPearl {
	class BoundingBox: public Component<BoundingBox>
	{
	public:
		BoundingBox(EntityManager* entityManager, Entity::Id id, const std::vector<Mesh>& meshes)
			:Component(entityManager, id, Component::Type::BoundingBox), m_Meshes(meshes) {
			m_BoundingBoxBuilder.reset(DBG_NEW BoundingBoxBuilder());
		}

		AABB Get() const { return m_BoundingBoxBuilder->Get(); }
		AABB Build();
	private:
		AABB m_Box;
		std::vector<Mesh> m_Meshes;
		std::unique_ptr<BoundingBoxBuilder> m_BoundingBoxBuilder;
	};

}

