#include "pch.h"
#include "BatchNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	BatchNode::BatchNode(const std::vector<Object*>& objs, bool dynamic)
		: Node(Node::Type::Batch_Node)
	{
		//m_ObjectsList = objs;
		m_Batch = std::make_unique<Batch>(objs, dynamic);
	}

	void BatchNode::SetRenderState()
	{
		m_Batch->SetAttributes();
	}

	void BatchNode::AddObj(Object* obj)
	{
		//m_ObjectsList.push_back(obj);
		m_Batch->AddObj(obj);
	}

	void BatchNode::UpdateObjs()
	{
		/*for (size_t i = 0; i < m_ObjectsList.size(); i++)
		{
			auto obj = m_ObjectsList[i];
			m_Batch->UpdateObjTransform(i, obj->GetComponent<Transform>()->GetTransformMatrix());
		}*/
	}

	uint32_t BatchNode::GetIndexCount() const
	{
		return m_Batch->GetIndexCount();
	}

	std::shared_ptr<VertexArray> BatchNode::GetVertexArray() const
	{
		return m_Batch->GetVertexArray();
	}

	float* BatchNode::GetModelMatrix() const
	{
		return m_Batch->GetModelMatrix();
	}


}