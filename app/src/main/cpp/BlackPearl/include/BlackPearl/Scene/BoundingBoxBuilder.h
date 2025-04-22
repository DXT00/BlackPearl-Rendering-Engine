#pragma once
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl {
	class BoundingBoxBuilder
	{
	public:

		BoundingBoxBuilder();
		static AABB Build(Object* obj);
	//	AABB Build(const std::vector<Mesh>& meshes);
	//	AABB Build(const std::vector<Vertex>& mesh_vertex);

		AABB Get() const { return m_Box; };
	private:
		//void ChangeMeshToBVHNode(const std::vector<Vertex>& mesh_vertex);
		static AABB SphereBoundingBox(Object* obj);
		static AABB TriangleBoundingBox(Object* obj);

		AABB m_Box;
	};
}


