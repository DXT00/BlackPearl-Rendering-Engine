#pragma once
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Model/Model.h"
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
		void AddModel(Model* obj);

		std::vector<Object*> GetObjects() const { return m_ObjectList; }
		std::vector<Model*> GetModels() const { return m_ModelList; }

		DemoType GetDemoType() { return m_DemoType; }
	protected:
		DemoType m_DemoType;
		std::vector<Object*> m_ObjectList;
		std::vector<Model*> m_ModelList;

	};


}
