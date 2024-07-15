#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Model/Model.h"
#include "BlackPearl/Renderer/Mesh/MeshManager.h"
#include "BlackPearl/Node/Node.h"
#include "BlackPearl/Lumen/LumenSceneData.h"
#include "SceneOctree.h"
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
		void AddLights(Object* obj);
		void GetLights();
		void AddObject(Object* obj);
		// only single nodes now
		void UpdateObjsAABB();

		void AddModel(Model* obj);

		std::vector<Object*> GetObjects() const { return m_ObjectList; }
		std::vector<Model*> GetModels() const { return m_ModelList; }
		std::vector<Node*> GetNodes() const { return m_NodesList; }
		std::vector<Node*> GetBatchNodes() const { return m_BatchNodesList; }
		std::vector<Node*> GetInstanceNodes() const { return m_InstanceNodesList; }
		std::vector<Node*> GetSingleNodes() const { return m_SingleNodesList; }
		uint32_t GetSingleNodesCnt() const { return m_SingleNodesList.size(); }

		Node* GetSingleNodes(uint32_t i) const { return m_SingleNodesList[i]; }
		//TODO:: Octree manager
		Node* GetRootNode() const {
			return m_RootNode;
		}
		DemoType GetDemoType() { return m_DemoType; }

		/** An octree containing the primitives in the scene. */
		ScenePrimitiveOctree *PrimitiveOctree;

		/**unordered_map of primitive octree node index**/
		std::unordered_map<uint64_t, uint32_t> PrimitiveOctreeIndex;

	public:
		std::shared_ptr<LumenSceneData> LumenSceneData;
		std::vector<AABB> m_ObjectBounds;
		std::vector<uint32_t> m_ObjectOctreeIndex;
	protected:
		DemoType m_DemoType;
		std::vector<Object*> m_ObjectList;


		std::vector<Node*>   m_NodesList;
		std::vector<Node*>   m_BatchNodesList;
		std::vector<Node*>   m_InstanceNodesList;
		std::vector<Node*>   m_SingleNodesList;
		std::vector<Model*>  m_ModelList;
		std::shared_ptr<MeshManager> m_MeshMgr;

		Object* m_RootObj;
		Node* m_RootNode;


	private:
		void _AddNode(Node* node);

	};


}
