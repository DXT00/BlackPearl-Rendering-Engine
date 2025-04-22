#pragma once
#include "Node.h"
#include "BlackPearl/Renderer/Batch/Batch.h"
namespace BlackPearl {
	class BatchNode: public Node
	{
	public:
		BatchNode(Object* selfObj, const std::vector<Object*>& objs, bool dynamic);
		virtual void				 AddObj(Object* obj) override;
		virtual void				 UpdateObjs() override;
		void						 SetRenderState();
		void						 SetDeltaPosition(math::float3 pos);
		void						 SetDeltaScale(math::float3);

		float*						 GetModelMatrix() const;
		uint32_t                     GetIndexCount() const;
		uint32_t					 GetVertexCount() const;
		uint32_t					 GetObjCnt() const;
		uint32_t					 GetInstanceCnt() const;
		Object*						 GetSelfObj() const;
		std::vector<Object*>		 GetObjs() const;
		std::shared_ptr<VertexArray> GetVertexArray() const;

		std::shared_ptr<Batch>	     GetBatch() const;

	private:
		std::shared_ptr<Batch> m_Batch;
		//m_SelfObj is Used to transform the entire batch
		Object* m_SelfObj;
	};
}


