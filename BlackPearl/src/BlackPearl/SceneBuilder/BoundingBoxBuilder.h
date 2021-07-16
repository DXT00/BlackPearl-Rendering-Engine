#pragma once
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl {
	class BoundingBoxBuilder
	{
	public:
		BoundingBoxBuilder(const std::vector<Mesh>& meshes);
		BoundingBoxBuilder(const std::vector<Vertex>& mesh_vertex);



		AABB Get() const { return m_Box; };
	private:
		void ChangeMeshToBVHNode(const std::vector<Vertex>& mesh_vertex);
		AABB Build(const std::vector<Mesh>& meshes);
		AABB Build(const std::vector<Vertex>& mesh_vertex);

		AABB m_Box;
	};
}


