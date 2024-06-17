#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
namespace BlackPearl {
	class Node
	{
	public:
		enum Type {
			Single_Node,
			Batch_Node,
			Instance_Node,
		};
		Node(Type type):
			m_Type(type){}
		virtual void AddObj(Object* obj) = 0;
		virtual void UpdateObjs() = 0;

		//virtual void UpdateTransform(const Transform& trans) = 0;

		void AppendChild(Node* node);
		Type GetType() const {
			return m_Type;
		}
		 
	protected:


		Node* m_Parent = nullptr;
		std::vector<Object*> m_ChildNodes;
		Type m_Type;

	};

}

