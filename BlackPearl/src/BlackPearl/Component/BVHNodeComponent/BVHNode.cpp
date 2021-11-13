#include "pch.h"
#include "BVHNode.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/RayTracing/Triangle.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {
	extern ObjectManager* g_objectManager;

	BVHNode::BVHNode(const std::vector<Object*>& objs)
		:Component(Component::Type::BVHNode)
	{
		Build(objs);
	}

	BVHNode::BVHNode(const std::vector<Vertex>& mesh_vertex)
		: Component(Component::Type::BVHNode)
	{
		std::vector<Object*> trimesh = BuildTriMesh(mesh_vertex);
		if (trimesh.size() == 0) {
			return;
		}
		Build(trimesh);
	}

	AABB BVHNode::GetRootBox() const
	{
		return m_Box;
	}

	bool BVHNode::IsLeaf() const
	{
		return m_IsLeaf;
	}

	void BVHNode::SetLeaf(bool is_leaf)
	{
		m_IsLeaf = is_leaf;
	}

	Object* BVHNode::GetLeft() const
	{
		return m_Left;
	}

	Object* BVHNode::GetRight() const
	{
		return m_Right;
	}

	void BVHNode::Build(std::vector<Object*> objs)
	{
		size_t num = objs.size();
		//计算场景的bounding box
		size_t dim = 3;
		if (num == 1) {
			m_Left = objs[0];
			m_IsLeaf == true;
			m_Right = NULL;
			m_Box = objs[0]->GetComponent<BlackPearl::BoundingBox>()->Get();
			return ;
		}
		else if (num == 2) {
			m_Left = objs[0];
			m_Right = objs[1];
			m_Box = m_Left->GetComponent<BlackPearl::BoundingBox>()->Get() + m_Right->GetComponent<BlackPearl::BoundingBox>()->Get();
			return ;
		}

		for (auto obj : objs)
		{
			m_Box.Expand(obj->GetComponent<BlackPearl::BoundingBox>()->Get());
		}
		// get best partition
		std::vector<Object*> leftNodes;
		std::vector<Object*> rightNodes;
		double minCost = DBL_MAX;
		for (int i = 0; i < dim; i++)
		{
			std::vector<std::vector<Object*>> buckets(m_BucketsNum);
			std::vector<AABB> boxOfBuckets(m_BucketsNum);
			float boxLen = m_Box.GetExtent()[i];
			float bucketLen = static_cast<float>(boxLen / num);

			//分配场景中的物体到buckets中
			for (auto obj : objs)
			{
				AABB& obj_box = obj->GetComponent<BlackPearl::BoundingBox>()->Get();
				int bucketID = (obj_box.GetCenter()[i] - m_Box.GetMinP()[i]) / boxLen;
				buckets[bucketID].push_back(obj);
				boxOfBuckets[bucketID].Expand(obj_box);
			}

			//计算最佳分割点
			std::vector<AABB> leftBox(m_BucketsNum);
			std::vector<AABB> rightBox(m_BucketsNum);
			std::vector<size_t> leftAccNum(m_BucketsNum);
			std::vector<size_t> rightAccNum(m_BucketsNum);
			leftAccNum[0] = 0;
			rightAccNum[0] = 0;
			for (int i = 1; i < m_BucketsNum; i++)
			{
				leftBox[i] = leftBox[i - 1] + boxOfBuckets[i - 1];
				leftAccNum[i] = leftAccNum[i - 1] + buckets[i].size();
				rightBox[i] = rightBox[i - 1] + boxOfBuckets[m_BucketsNum - i];
				rightAccNum[i] = rightAccNum[i - 1] + buckets[m_BucketsNum - 1].size();


			}
			float minCostDim = FLT_MAX;
			int partitionIdx = 0;
			size_t bestLeftNum = 0;
			float CostDim;
			for (int i = 1; i < m_BucketsNum; i++) {
				CostDim = leftBox[i].GetSurfaceArea() * leftAccNum[i] + rightBox[m_BucketsNum - i].GetSurfaceArea() * rightAccNum[m_BucketsNum - i];
				if (CostDim < minCostDim) {
					minCostDim = CostDim;
					bestLeftNum = i;
				}
			}

			if (minCostDim < minCost) {
				leftNodes.clear();
				rightNodes.clear();
				minCost = minCostDim;
				for (int i = 0; i < m_BucketsNum; i++)
				{

					for (auto obj : buckets[i])
					{
						(i < bestLeftNum) ? leftNodes.push_back(obj) : rightNodes.push_back(obj);

					}
				}
			}

		}
		//递归分割BVH Node
		if (leftNodes.size() == num) {
			size_t leftNum = num / 2;

			for (size_t i = 0; i < leftNum; i++)
			{
				rightNodes.push_back(leftNodes.back());
				leftNodes.pop_back();
			}
		}
		else if (rightNodes.size() == num) {
			size_t leftNum = num / 2;
			for (size_t i = 0; i < leftNum; i++)
			{
				leftNodes.push_back(rightNodes.back());
				rightNodes.pop_back();
			}
		}


		m_Left = g_objectManager->CreateBVHNode(leftNodes);
		m_Right = g_objectManager->CreateBVHNode(rightNodes);

	}


	//AABB BVHNode::Build(const std::vector<Triangle*>& triMesh) {

	//	size_t num = triMesh.size();
	//	//计算场景的bounding box
	//	size_t dim = 3;

	//	if (num == 1) {
	//		m_Left = DBG_NEW BVHNode({ triMesh[0] });
	//		m_Right = NULL;
	//		m_Box = triMesh[0]->GetBoundingBox();
	//		return m_Box;
	//	}
	//	else if (num == 2) {
	//		m_Left = DBG_NEW BVHNode({triMesh[0]});
	//		m_Right = DBG_NEW BVHNode({ triMesh[1] });
	//		m_Box = triMesh[0]->GetBoundingBox() + triMesh[1]->GetBoundingBox();
	//		return m_Box;
	//	}

	//	for (auto tri : triMesh)
	//	{
	//		m_Box.Expand(tri->GetBoundingBox());
	//	}

	//	for (int i = 0; i < dim; i++)
	//	{
	//		std::vector<std::vector<Triangle*>> buckets(m_BucketsNum);
	//		std::vector<AABB> boxOfBuckets(m_BucketsNum);
	//		float boxLen = m_Box.GetExtent()[i];
	//		float bucketLen = static_cast<float>(boxLen / num);

	//		//分配场景中的物体到buckets中
	//		for (auto tri : triMesh)
	//		{
	//			AABB& obj_box = tri->GetBoundingBox();
	//			int bucketID = (obj_box.GetCenter().x - m_Box.GetMinP().x) / boxLen;
	//			buckets[bucketID].push_back(tri);
	//			boxOfBuckets[bucketID].Expand(obj_box);
	//		}

	//		//计算最佳分割点
	//		std::vector<AABB> leftBox(m_BucketsNum);
	//		std::vector<AABB> rightBox(m_BucketsNum);
	//		std::vector<size_t> leftAccNum(m_BucketsNum);
	//		std::vector<size_t> rightAccNum(m_BucketsNum);

	//		for (int i = 1; i <= m_BucketsNum; i++)
	//		{
	//			leftBox[i] = leftBox[i - 1] + boxOfBuckets[i - 1];
	//			leftAccNum[i] = leftAccNum[i - 1] + buckets[i].size();
	//			rightBox[i] = rightBox[i - 1] + boxOfBuckets[m_BucketsNum - i];
	//			rightAccNum[i] = rightAccNum[i - 1] + buckets[m_BucketsNum - 1].size();


	//		}
	//		float minCost = FLT_MAX;
	//		int partitionIdx = 0;
	//		float curCost;
	//		for (int i = 1; i <= m_BucketsNum; i++) {
	//			curCost = leftBox[i].GetSurfaceArea() * leftAccNum[i] + rightBox[m_BucketsNum - i].GetSurfaceArea() * rightAccNum[m_BucketsNum - i];
	//			if (curCost < minCost) {
	//				minCost = curCost;
	//				partitionIdx = i;
	//			}
	//		}

	//		//递归分割BVH Node
	//		std::vector<Triangle*> leftNodes;
	//		std::vector<Triangle*> rightNodes;

	//		for (int i = 0; i < m_BucketsNum; i++)
	//		{

	//			for (auto tri : buckets[i])
	//			{
	//				(i < partitionIdx) ? leftNodes.push_back(tri) : rightNodes.push_back(tri);

	//			}
	//		}
	//		m_Left = DBG_NEW BVHNode(leftNodes);
	//		m_Right = DBG_NEW BVHNode(rightNodes);

	//	}
	//}

	std::vector<Object*> BVHNode::BuildTriMesh(const std::vector<Vertex>& mesh_vertex)
	{
		std::vector<Object*> triMesh;
		if (mesh_vertex.size() % 3 != 0) {
			m_Box.SetInvalid();
			GE_CORE_WARN("mesh_vertex.size() % 3 != 0, m_Box is invalid")
				return std::vector<Object*>();
		}
		for (size_t i = 0; i < mesh_vertex.size(); i += 3)
		{
			Object* tri = g_objectManager->CreateTriangle({ mesh_vertex[i],mesh_vertex[i + 1],mesh_vertex[i + 2] });

			triMesh.push_back(tri);

		}
		return triMesh;
	}
}