#include "pch.h"
#include "BatchNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	BatchNode::BatchNode(Object* selfObj, const std::vector<Object*>& objs, bool dynamic)
		: Node(selfObj, Node::Type::Batch_Node)
	{
		//m_ObjectsList = objs;
		m_Batch = std::make_shared<Batch>(objs, dynamic);
		m_SelfObj = selfObj;
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
		m_Batch->UpdateObjsTransform();
	}

	uint32_t BatchNode::GetIndexCount() const
	{
		return m_Batch->GetIndexCount();
	}

	uint32_t BatchNode::GetVertexCount() const
	{
		return m_Batch->GetVertexCount();
	}

	uint32_t BatchNode::GetObjCnt() const
	{
		return m_Batch->GetObjects().size();
	}

	//uint32_t BatchNode::GetInstanceCnt() const
	//{
	//	return m_Batch->GetInstanceCnt();
	//}
	
	std::vector<Object*> BatchNode::GetObjs() const
	{
		return m_Batch->GetObjects();
	}

	Object* BatchNode::GetSelfObj() const
	{
		return m_SelfObj;
	}

	std::shared_ptr<VertexArray> BatchNode::GetVertexArray() const
	{
		return m_Batch->GetVertexArray();
	}

	std::shared_ptr<Batch> BatchNode::GetBatch() const
	{
		return m_Batch;
	}

	float* BatchNode::GetModelMatrix() const
	{
		return m_Batch->GetModelMatrix();
	}

	void BatchNode::SetDeltaPosition(math::float3 pos)
	{
		m_SelfObj->GetComponent<Transform>()->SetPosition(Math::ToVec3(pos));
		m_Batch->UpdateBatchPosition(pos);
		
	}

	void BatchNode::SetDeltaScale(math::float3 scale)
	{
		m_SelfObj->GetComponent<Transform>()->SetScale(Math::ToVec3(scale));
		m_Batch->UpdateBatchScale(scale);
	}



}