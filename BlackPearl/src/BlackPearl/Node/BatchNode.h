#pragma once
#include "Node.h"
#include "BlackPearl/Renderer/Batch/Batch.h"
namespace BlackPearl {
	class BatchNode: public Node
	{
	public:
		BatchNode(const std::vector<Object*>& objs, bool dynamic);
		void SetRenderState();
		virtual void AddObj(Object* obj) override;
		virtual void UpdateObjs() override;
		uint32_t GetIndexCount() const;
		std::shared_ptr<VertexArray> GetVertexArray() const;
		float* GetModelMatrix() const;
		uint32_t GetObjCnt() const{ return m_Batch->GetObjects().size(); }
		std::vector<Object*> GetObjs() const { return m_Batch->GetObjects(); }

	private:
		std::unique_ptr<Batch> m_Batch;
		//std::vector<Object*> m_ObjectsList;
	};
}


