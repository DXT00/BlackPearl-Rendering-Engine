#include "pch.h"
#include "BVHNode.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/RayTracing/Triangle.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {

	BVHNode::BVHNode( const std::vector<Object*>& objs)
		:Component(Component::Type::BVHNode)
	{
		m_Box = Build(objs);
	}

	BVHNode::BVHNode(const std::vector<Vertex>& mesh_vertex)
		:Component(Component::Type::BVHNode) 
	{
		std::vector<Triangle*> trimesh = BuildTriMesh(mesh_vertex);
		m_Box = Build(trimesh);
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

	AABB BVHNode::Build(std::vector<Object*> objs)
	{
		size_t num = objs.size();
		//计算场景的bounding box
		size_t dim = 3;

		if (num == 1) {
			m_Left = g_objectManager->CreateBVHNode({ objs[0] }); 
			m_IsLeaf == true;
			m_Right = NULL;
			m_Box = objs[0]->GetComponent<BlackPearl::BoundingBox>()->Get();
			return m_Box;
		}
		else if (num == 2) {
			m_Left = g_objectManager->CreateBVHNode({objs[0]});
			m_Right = g_objectManager->CreateBVHNode({ objs[1] });
			m_Box = m_Left->GetComponent<BVHNode>()->GetRootBox() + m_Right->GetComponent<BVHNode>()->GetRootBox();
			return m_Box;
		}

		for (auto obj:objs)
		{
			m_Box.Expand(obj->GetComponent<BlackPearl::BoundingBox>()->Get());
		}

		for (int i = 0; i < dim; i++)
		{
			std::vector<std::vector<Object*>> buckets(m_BucketsNum);
			std::vector<AABB> boxOfBuckets(m_BucketsNum);
			float boxLen = m_Box.GetExtent()[i];
			float bucketLen = static_cast<float>(boxLen / num);

			//分配场景中的物体到buckets中
			for (auto obj:objs)
			{
				AABB &obj_box = obj->GetComponent<BlackPearl::BoundingBox>()->Get();
				int bucketID = (obj_box.GetCenter().x - m_Box.GetMinP().x) / boxLen;
				buckets[bucketID].push_back(obj);
				boxOfBuckets[bucketID].Expand(obj_box);
			}

			//计算最佳分割点
			std::vector<AABB> leftBox(m_BucketsNum);
			std::vector<AABB> rightBox(m_BucketsNum);
			std::vector<size_t> leftAccNum(m_BucketsNum);
			std::vector<size_t> rightAccNum(m_BucketsNum);
			
			for (int i = 1; i <= m_BucketsNum; i++)
			{
				leftBox[i] = leftBox[i - 1] + boxOfBuckets[i - 1];
				leftAccNum[i] = leftAccNum[i - 1] + buckets[i].size();
				rightBox[i] = rightBox[i - 1] + boxOfBuckets[m_BucketsNum - i];
				rightAccNum[i] = rightAccNum[i - 1] + buckets[m_BucketsNum - 1].size();


			}
			float minCost = FLT_MAX;
			int partitionIdx = 0;
			float curCost;
			for (int i = 1; i <= m_BucketsNum; i++) {
				curCost = leftBox[i].GetSurfaceArea() * leftAccNum[i] + rightBox[m_BucketsNum-i].GetSurfaceArea() * rightAccNum[m_BucketsNum - i];
				if (curCost < minCost) {
					minCost = curCost;
					partitionIdx = i;
				}
			}

			//递归分割BVH Node
			std::vector<Object*> leftNodes;
			std::vector<Object*> rightNodes;

			for (int i = 0; i < m_BucketsNum; i++)
			{

				for (auto obj:buckets[i])
				{
					(i < partitionIdx) ? leftNodes.push_back(obj) : rightNodes.push_back(obj);

				}
			}
			m_Left = g_objectManager->CreateBVHNode(leftNodes);
			m_Right = g_objectManager->CreateBVHNode(leftNodes);
	
		}

	}
	
	
	AABB BVHNode::Build(const std::vector<Triangle*>& triMesh) {

		size_t num = triMesh.size();
		//计算场景的bounding box
		size_t dim = 3;

		if (num == 1) {
			m_Left = DBG_NEW BVHNode({ triMesh[0] });
			m_Right = NULL;
			m_Box = triMesh[0]->GetBoundingBox();
			return m_Box;
		}
		else if (num == 2) {
			m_Left = DBG_NEW BVHNode({triMesh[0]});
			m_Right = DBG_NEW BVHNode({ triMesh[1] });
			m_Box = triMesh[0]->GetBoundingBox() + triMesh[1]->GetBoundingBox();
			return m_Box;
		}

		for (auto tri : triMesh)
		{
			m_Box.Expand(tri->GetBoundingBox());
		}

		for (int i = 0; i < dim; i++)
		{
			std::vector<std::vector<Triangle*>> buckets(m_BucketsNum);
			std::vector<AABB> boxOfBuckets(m_BucketsNum);
			float boxLen = m_Box.GetExtent()[i];
			float bucketLen = static_cast<float>(boxLen / num);

			//分配场景中的物体到buckets中
			for (auto tri : triMesh)
			{
				AABB& obj_box = tri->GetBoundingBox();
				int bucketID = (obj_box.GetCenter().x - m_Box.GetMinP().x) / boxLen;
				buckets[bucketID].push_back(tri);
				boxOfBuckets[bucketID].Expand(obj_box);
			}

			//计算最佳分割点
			std::vector<AABB> leftBox(m_BucketsNum);
			std::vector<AABB> rightBox(m_BucketsNum);
			std::vector<size_t> leftAccNum(m_BucketsNum);
			std::vector<size_t> rightAccNum(m_BucketsNum);

			for (int i = 1; i <= m_BucketsNum; i++)
			{
				leftBox[i] = leftBox[i - 1] + boxOfBuckets[i - 1];
				leftAccNum[i] = leftAccNum[i - 1] + buckets[i].size();
				rightBox[i] = rightBox[i - 1] + boxOfBuckets[m_BucketsNum - i];
				rightAccNum[i] = rightAccNum[i - 1] + buckets[m_BucketsNum - 1].size();


			}
			float minCost = FLT_MAX;
			int partitionIdx = 0;
			float curCost;
			for (int i = 1; i <= m_BucketsNum; i++) {
				curCost = leftBox[i].GetSurfaceArea() * leftAccNum[i] + rightBox[m_BucketsNum - i].GetSurfaceArea() * rightAccNum[m_BucketsNum - i];
				if (curCost < minCost) {
					minCost = curCost;
					partitionIdx = i;
				}
			}

			//递归分割BVH Node
			std::vector<Triangle*> leftNodes;
			std::vector<Triangle*> rightNodes;

			for (int i = 0; i < m_BucketsNum; i++)
			{

				for (auto tri : buckets[i])
				{
					(i < partitionIdx) ? leftNodes.push_back(tri) : rightNodes.push_back(tri);

				}
			}
			m_Left = DBG_NEW BVHNode(leftNodes);
			m_Right = DBG_NEW BVHNode(rightNodes);

		}
	}

	std::vector<Triangle*> BVHNode::BuildTriMesh(const std::vector<Vertex>& mesh_vertex)
	{
		std::vector<Triangle*> triMesh;
		std::shared_ptr<BVHNode> bvhNode;
		if (mesh_vertex.size() % 3 != 0) {
			m_Box.IsValid = false;
			return std::vector<Triangle*>();
		}
		for (size_t i = 0; i < mesh_vertex.size(); i += 3)
		{
			Triangle* tri = DBG_NEW Triangle(mesh_vertex[i],mesh_vertex[i+1],mesh_vertex[i+2]);

			triMesh.push_back(tri);

		}
		return triMesh;
	}



	AABB BVHNode::Build(const std::vector<Hitable*>::const_iterator begin, const std::vector<Hitable*>::const_iterator end)
	{
		size_t num = end - begin;
		//计算场景的bounding box
		size_t dim = 3;

		if (num == 1) {
			m_Left = (*begin)->GetBoundingBox();
			m_Right = AABB::InValid;
			m_Box = (*begin)->GetBoundingBox();
			return m_Box;
		}
		else if (num == 2) {
			m_Left = (*begin)->GetBoundingBox();
			m_Right = (*end)->GetBoundingBox();
			m_Box = m_Left + m_Right;
			return m_Box;
		}

		for (auto hit = begin; hit != end; hit++)
		{
			m_Box.Expand((*hit)->GetBoundingBox());
		}

		for (int i = 0; i < dim; i++)
		{
			std::vector<std::vector<Hitable>> buckets(m_BucketsNum);
			std::vector<AABB> boxOfBuckets(m_BucketsNum);
			float boxLen = m_Box.GetExtent()[i];
			float bucketLen = static_cast<float>(boxLen / num);

			//分配场景中的物体到buckets中
			for (auto hit = begin; hit != end; hit++)
			{
				AABB& obj_box = (*hit)->GetBoundingBox();
				int bucketID = (obj_box.GetCenter().x - m_Box.GetMinP().x) / boxLen;
				buckets[bucketID].push_back((*hit));
				boxOfBuckets[bucketID].Expand(obj_box);

			}

			//计算最佳分割点
			std::vector<AABB> leftBox(m_BucketsNum);
			std::vector<AABB> rightBox(m_BucketsNum);
			std::vector<size_t> leftAccNum(m_BucketsNum);
			std::vector<size_t> rightAccNum(m_BucketsNum);

			for (int i = 1; i <= m_BucketsNum; i++)
			{
				leftBox[i] = leftBox[i - 1] + boxOfBuckets[i - 1];
				leftAccNum[i] = leftAccNum[i - 1] + buckets[i].size();
				rightBox[i] = rightBox[i - 1] + boxOfBuckets[m_BucketsNum - i];
				rightAccNum[i] = rightAccNum[i - 1] + buckets[m_BucketsNum - 1].size();


			}
			float minCost = FLT_MAX;
			int partitionIdx = 0;
			float curCost;
			for (int i = 1; i <= m_BucketsNum; i++) {
				curCost = leftBox[i].GetSurfaceArea() * leftAccNum[i] + rightBox[m_BucketsNum - i].GetSurfaceArea() * rightAccNum[m_BucketsNum - i];
				if (curCost < minCost) {
					minCost = curCost;
					partitionIdx = i;
				}
			}

			//递归分割BVH Node
			std::vector<Hitable> leftNodes;
			std::vector<Hitable> rightNodes;

			for (int i = 0; i < m_BucketsNum; i++)
			{

				for (auto obj : buckets[i])
				{
					(i < partitionIdx) ? leftNodes.push_back(obj) : rightNodes.push_back(obj);

				}
			}
			m_Left = Build(leftNodes);
			m_Right = Build(rightNodes);

		}
	}

}