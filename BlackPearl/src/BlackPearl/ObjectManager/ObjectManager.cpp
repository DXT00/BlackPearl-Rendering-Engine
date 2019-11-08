#include "pch.h"
#include "ObjectManager.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"

#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
namespace BlackPearl {

	
	Object * ObjectManager::CreateEmpty(std::string name)
	{
		Object *obj= m_ObjectCreater->CreateEmpty(name);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;
	}

	Object * ObjectManager::CreateCamera()
	{
		Object* obj = m_CameraCreater->CreateCamera();
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));

		return obj;

	}

	Object * ObjectManager::CreateLight(LightType type, LightSources* lightSources)
	{
		Object *obj = m_LightCreater->CreateLight(type, lightSources);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	Object * ObjectManager::CreateModel(const std::string & modelPath, const std::string & shaderPath)
	{
		Object *obj = m_Object3DCreater->CreateModel(modelPath, shaderPath);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	Object * ObjectManager::CreateCube()
	{
		Object *obj = m_Object3DCreater->CreateCube();
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	std::vector<Object*> ObjectManager::GetObjects()
	{
		std::vector<Object*>objs;
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				objs.push_back(obj);
			}
		}
		return objs;
	}
	std::vector<std::string> ObjectManager::GetObjectsName()
	{
		std::vector<std::string>objs;
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				objs.push_back(obj->ToString());
			}
		}
		return objs;
	}
	void ObjectManager::DrawObjects()
	{
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr &&obj->HasComponent<MeshRenderer>()) {
				auto transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
				obj->GetComponent<MeshRenderer>()->UpdateTransformMatrix(transformMatrix);

				//灯光单独处理
				if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>()) {
					obj->GetComponent<MeshRenderer>()->DrawLight();
				}
				else {
					obj->GetComponent<MeshRenderer>()->DrawMeshes();
					obj->GetComponent<MeshRenderer>()->DrawModel();
				}

			}
		}
	}
	void ObjectManager::DestroyObjects()
	{
		for (auto pair : m_EntityToObjects) {
			Object* obj = pair.second;
			if (obj != nullptr) {
				delete obj;
				obj = nullptr;
			}
		}
	}
}