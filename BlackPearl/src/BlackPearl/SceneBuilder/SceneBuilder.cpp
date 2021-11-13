#include "pch.h"
#include "SceneBuilder.h"

namespace BlackPearl {

	SceneBuilder::SceneBuilder() {

	}
	
	void SceneBuilder::CreateSceneData(Object* obj) {
		m_SceneData.reset(DBG_NEW GenData(obj));
	}
	void SceneBuilder::CreateSceneData(Group* group) {
		m_SceneData.reset(DBG_NEW GenData(group->GetRoot()));
	}
	std::shared_ptr<GenData> SceneBuilder::GetScene() const
	{
		return m_SceneData;
	}
}

