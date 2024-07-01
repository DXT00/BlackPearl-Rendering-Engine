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
		virtual void AddObj(Object* obj) {};
		virtual void UpdateObjs() {};

		//virtual void UpdateTransform(const Transform& trans) = 0;

		void AppendChild(Node* node);
		Type GetType() const {
			return m_Type;
		}
		[[nodiscard]] Node* GetParent() const { return m_Parent; }
		[[nodiscard]] Node* GetFirstChild() const { return m_FirstChild.get(); }
		[[nodiscard]] Node* GetNextSibling() const { return m_NextSibling.get(); }
		[[nodiscard]] const std::shared_ptr<Node>& GetLeaf() const { return m_Leaf; }
		std::string m_Name;
		bool m_Dirty = false;
	protected:
		std::vector<Object*> m_ChildNodes;
		Type m_Type;

	private:
		//friend class SceneGraph;
		//std::weak_ptr<SceneGraph> m_Graph;

		Node* m_Parent = nullptr;
		std::shared_ptr<Node> m_FirstChild;
		std::shared_ptr<Node> m_NextSibling;
		std::shared_ptr<Node> m_Leaf;

	};

}

