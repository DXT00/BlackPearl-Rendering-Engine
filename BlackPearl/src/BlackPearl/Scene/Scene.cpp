#include "pch.h"
#include "Scene.h"

namespace BlackPearl {
	Scene::Scene(DemoType type)
	{
		m_DemoType = type;
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
	void Scene::AddLights()
	{
	}
	void Scene::GetLights()
	{
	}
	void Scene::AddObject(Object* obj)
	{
		m_ObjectList.push_back(obj);
	}
}
