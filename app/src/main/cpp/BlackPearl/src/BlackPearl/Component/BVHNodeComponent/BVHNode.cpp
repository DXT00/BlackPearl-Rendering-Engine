#include "pch.h"
#include "BVHNode.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/RayTracing/Triangle.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {
	extern ObjectManager* g_objectManager;

	BVHNode::BVHNode(Object* obj)
		:Component(Component::Type::BVHNode)
	{
		m_IsLeaf = true;
		m_LeafObj = obj;

		GE_ASSERT(obj->HasComponent<BlackPearl::Triangle>(), "not triagle");
		m_Box = obj->GetComponent<BlackPearl::BoundingBox>()->Get();
	}

	BVHNode::BVHNode(const std::vector<Object*>& objs)
		:Component(Component::Type::BVHNode)
	{
		m_IsLeaf = false;

		Build(objs);
	}

	BVHNode::BVHNode(const std::vector<Vertex>& mesh_vertex)
		: Component(Component::Type::BVHNode)
	{
		GE_ASSERT(!mesh_vertex.empty(), "mesh_vertex is empty");
		m_IsLeaf = false;
		BuildTriMesh(mesh_vertex);
		if (m_TriangleMesh.size() == 0) {
			return;
		}
		Build(m_TriangleMesh);
	}

	AABB BVHNode::GetRootBox() const
	{
		return m_Box;
	}

	bool BVHNode::IsLeaf() const
	{
		return m_IsLeaf;
	}

	Object* BVHNode::GetLeafObj() const
	{
		return m_LeafObj;
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

	std::vector<Object*> BVHNode::GetTriangleMesh() const
	{
		return m_TriangleMesh;
	}

	void BVHNode::Build(std::vector<Object*> objs)
	{
		size_t num = objs.size();
		//���㳡����bounding box
		size_t dim = 3;
		if (num == 1) {
			m_IsLeaf = true;
			m_Left = NULL;
			m_Right = NULL;
			m_Box = objs[0]->GetComponent<BlackPearl::BoundingBox>()->Get();
			m_LeafObj = objs[0];
			GE_ASSERT(objs[0]->HasComponent<BlackPearl::Triangle>(), "not triagle");
			return ;
		}
		else if (num == 2) {
			m_Left = objs[0];
			m_Right = objs[1];
			if(!m_Left->HasComponent<BVHNode>())
				m_Left->AddComponent<BVHNode>(objs[0]);
			if (!m_Right->HasComponent<BVHNode>())
				m_Right->AddComponent<BVHNode>(objs[1]);
			m_IsLeaf = false;
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

			//���䳡���е����嵽buckets��
			for (auto obj : objs)
			{
				AABB& obj_box = obj->GetComponent<BlackPearl::BoundingBox>()->Get();
				int bucketID = 0;
				if(boxLen != 0)
					bucketID = (obj_box.GetCenter()[i] - m_Box.GetMinP()[i]) / boxLen;
				buckets[bucketID].push_back(obj);
				boxOfBuckets[bucketID].Expand(obj_box);
			}

			//������ѷָ��
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
		//�ݹ�ָ�BVH Node
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
	//	//���㳡����bounding box
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

	//		//���䳡���е����嵽buckets��
	//		for (auto tri : triMesh)
	//		{
	//			AABB& obj_box = tri->GetBoundingBox();
	//			int bucketID = (obj_box.GetCenter().x - m_Box.GetMinP().x) / boxLen;
	//			buckets[bucketID].push_back(tri);
	//			boxOfBuckets[bucketID].Expand(obj_box);
	//		}

	//		//������ѷָ��
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

	//		//�ݹ�ָ�BVH Node
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
		if (mesh_vertex.size() % 3 != 0) {
			m_Box.SetInvalid();
			GE_CORE_WARN("mesh_vertex.size() % 3 != 0, m_Box is invalid")
				return std::vector<Object*>();
		}
		for (size_t i = 0; i < mesh_vertex.size(); i += 3)
		{
			Object* tri = g_objectManager->CreateTriangle({ mesh_vertex[i],mesh_vertex[i + 1],mesh_vertex[i + 2] });
			m_TriangleMesh.push_back(tri);

		}
		return m_TriangleMesh;
	}
}