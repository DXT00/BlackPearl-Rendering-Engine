#pragma once
#include "Node.h"
namespace BlackPearl {
	class SingleNode: public Node
	{
	public:
		SingleNode(Object* obj);
		~SingleNode();
		virtual void AddObj(Object* obj) {
			GE_CORE_WARN("Single node only support 1 object");
		};
		virtual void UpdateObjs() {};

		uint32_t GetVertexCount() const;
		uint32_t GetIndexCount() const;
	private:
		//Object* m_Obj;
		uint32_t m_VertexCnt;
		uint32_t m_IndexCnt;
	};
}


