#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Mesh/MeshManager.h"
#include "BlackPearl/Node/Node.h"
namespace BlackPearl {
	class Scene
	{
	public:
		enum DemoType {
			Empty,
			SDFScene,
			MetaBallSDFScene,
			MeshletScene
		};
		Scene(DemoType type = DemoType::Empty);
		~Scene();
		void AddCamera();
		void GetCamera();
		void AddLights();
		void GetLights();
		void AddObject(Object* obj);
		void AddNode(Node* node);

		void AddModel(Model* obj);

		std::vector<Object*> GetObjects() const { return m_ObjectList; }
		std::vector<Model*> GetModels() const { return m_ModelList; }
		std::vector<Node*> GetNodes() const { return m_NodesList; }
		std::vector<Node*> GetBatchNodes() const { return m_BatchNodesList; }
		std::vector<Node*> GetInstanceNodes() const { return m_InstanceNodesList; }
		std::vector<Node*> GetSingleNodes() const { return m_SingleNodesList; }
		uint32_t GetSingleNodesCnt() const { return m_SingleNodesList.size(); }

		Node* GetSingleNodes(uint32_t i) const { return m_SingleNodesList[i]; }

		DemoType GetDemoType() { return m_DemoType; }
	protected:
		DemoType m_DemoType;
		std::vector<Object*> m_ObjectList;
		std::vector<Node*>   m_NodesList;
		std::vector<Node*>   m_BatchNodesList;
		std::vector<Node*>   m_InstanceNodesList;
		std::vector<Node*>   m_SingleNodesList;
		std::vector<Model*>  m_ModelList;
		std::shared_ptr<MeshManager> m_MeshMgr;
	};


}
