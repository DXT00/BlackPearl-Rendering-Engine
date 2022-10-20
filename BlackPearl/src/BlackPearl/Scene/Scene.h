#pragma once
#include "BlackPearl/Object/Object.h"
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
		std::vector<Object*> GetObjects() const { return m_ObjectList; }
		DemoType GetDemoType() { return m_DemoType; }
	protected:
		DemoType m_DemoType;
		std::vector<Object*> m_ObjectList;

	};


}
