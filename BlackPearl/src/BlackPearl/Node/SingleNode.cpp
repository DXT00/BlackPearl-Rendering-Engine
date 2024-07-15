#include "pch.h"
#include "SingleNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/Object/Object.h"

namespace BlackPearl {
	SingleNode::SingleNode(Object* obj)
		: Node(obj, Node::Type::Single_Node),
		
		m_VertexCnt(0),
		m_IndexCnt(0)
	{
		//m_Obj = obj;
		if (obj->HasComponent<MeshRenderer>()) {
			std::vector<std::shared_ptr<Mesh>>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
			for (auto mesh : meshes)
			{
				m_VertexCnt += mesh->GetVertexCount();
				m_IndexCnt += mesh->GetIndicesCount();
			}
			//PrimitiveOctreeNode Init
			Bound = obj->GetComponent<BoundingBox>()->Get();
		}



	}
	SingleNode::~SingleNode()
	{
	}
	uint32_t SingleNode::GetVertexCount() const
	{
		return m_VertexCnt;
	}

	uint32_t SingleNode::GetIndexCount() const
	{
		return m_IndexCnt;
	}

	//Object* SingleNode::GetObj() const
	//{
	//	return m_Obj;
	//}
}
