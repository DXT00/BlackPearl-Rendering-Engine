#pragma once
#include <vector>
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/AABB/AABB.h"
#include "BlackPearl/RayTracing/Hitable.h"
#include "BlackPearl/RayTracing/Triangle.h"
namespace BlackPearl {
	class BVHNode : public Component<BVHNode>
	{
	public:
		BVHNode(Object* obj);
		BVHNode(const std::vector<Object*>& objs);

		BVHNode(const std::vector<Vertex>& mesh_vertex);

		AABB GetRootBox() const;
		bool IsLeaf() const;
		Object* GetLeafObj() const;

		void SetLeaf(bool is_leaf);
		Object* GetLeft() const;
		Object* GetRight() const;
		std::vector<Object*> GetTriangleMesh() const;

	private:
		void Build(std::vector<Object*> objs);
		//AABB Build(const std::vector<Triangle*>& triMesh);
		std::vector<Object*> BuildTriMesh(const std::vector<Vertex>& mesh_vertex);
		unsigned int m_BucketsNum = 8;
		Object* m_Left = NULL;
		Object* m_Right = NULL;
		Object* m_LeafObj = nullptr;
		bool m_IsLeaf = false;
		AABB m_Box;
		std::vector<Object*> m_TriangleMesh;
	};
}


