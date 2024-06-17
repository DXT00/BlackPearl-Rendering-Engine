#pragma once
#include "BlackPearl/RayTracing/GenData.h"
#include "BlackPearl/RayTracing/Group.h"
namespace BlackPearl {

	class SceneBuilder
	{
	public:
		SceneBuilder();
		void CreateSceneData(Object* obj);
		void CreateSceneData(Group* group);
		std::shared_ptr<GenData> GetScene() const;
	private:
		std::shared_ptr<GenData> m_SceneData;
	};

}

