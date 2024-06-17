#include "pch.h"
#include "Scene.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
namespace BlackPearl {
	Scene::Scene(DemoType type)
	{
		m_DemoType = type;
		m_MeshMgr = std::make_shared<MeshManager>();
	}

	Scene::~Scene()
	{
	}
	void Scene::AddCamera()
	{
	}
	void Scene::GetCamera()
	{
	}
	void Scene::AddLights(Object* obj)
	{
	}
	void Scene::GetLights()
	{
	}
	void Scene::AddObject(Object* obj)
	{
		SingleNode* singleNode = DBG_NEW SingleNode(obj);
		AddNode(singleNode);
		m_ObjectList.push_back(obj);

	}

	void Scene::AddNode(Node* node)
	{
		m_NodesList.push_back(node);
		if (node->GetType() == Node::Type::Batch_Node) {
			m_BatchNodesList.push_back(node);
		}
		else if (node->GetType() == Node::Type::Instance_Node) {
			m_InstanceNodesList.push_back(node);
		}
		else if (node->GetType() == Node::Type::Single_Node) {
			m_SingleNodesList.push_back(node);
		}
	}

	void Scene::AddModel(Model* model) {
		m_ModelList.push_back(model);
	}

	void Scene::UpdateObjsAABB() {
		for (auto node: m_SingleNodesList)
		{
			SingleNode* single = dynamic_cast<SingleNode*>(node);
			if (single->GetObj()->HasComponent<BoundingBox>()) {
				AABB box = single->GetObj()->GetComponent<BoundingBox>()->Get();
				box.UpdateTransform(single->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
				single->GetObj()->GetComponent<BoundingBox>()->SetBox(box);
			}
		}
	}
}
