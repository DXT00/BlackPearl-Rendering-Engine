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

	Object * ObjectManager::CreateLight(LightType type, LightSources* lightSources)
	{
		Object *obj = m_LightCreater->CreateLight(type, lightSources);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	Object * ObjectManager::CreateModel(const std::string & modelPath, const std::string & shaderPath,const bool isAnimated)
	{
		Object *obj = m_Object3DCreater->CreateModel(modelPath, shaderPath,isAnimated);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	Object * ObjectManager::CreateCube(const std::string& shaderPath, const std::string& texturePath)
	{
		Object *obj = m_Object3DCreater->CreateCube(shaderPath, texturePath);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;

	}

	Object * ObjectManager::CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string & shaderPath, const std::string & texturePath)
	{
		Object *obj = m_Object3DCreater->CreateSphere(radius, stackCount, sectorCount,shaderPath, texturePath);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;
	}

	Object * ObjectManager::CreatePlane(const std::string& shaderPath, const std::string& texturePath)
	{
		Object* obj = m_Object3DCreater->CreatePlane(shaderPath, texturePath);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;
	}

	Object * ObjectManager::CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath)
	{
		Object* obj = m_Object3DCreater->CreateSkyBox(textureFaces,shaderPath);
		m_EntityToObjects.insert(std::make_pair(obj->GetId().index(), obj));
		return obj;
	}
	///////////////////////Blending Object///////////////////////////////

	LightProbe* ObjectManager::CreateLightProbe(const std::string& shaderPath, const std::string& texturePath)
	{
		Object* obj = m_Object3DCreater->CreateLightProbe(shaderPath, texturePath);
		Object* cameraObj = m_Object3DCreater->CreateCamera();

		LightProbe* lightProbe = DBG_NEW LightProbe(obj, cameraObj);
		m_EntityToObjects.insert(std::make_pair(lightProbe->GetObj()->GetId().index(), lightProbe->GetObj()));
		m_EntityToObjects.insert(std::make_pair(cameraObj->GetId().index(), cameraObj));

		//note:dosen't put cameraObj to m_EntityToObjects!
		return lightProbe;
	}

	MainCamera* ObjectManager::CreateCamera()
	{
		Object* obj = m_Object3DCreater->CreateCamera();
		MainCamera* mainCamera = DBG_NEW MainCamera(obj);
		m_EntityToObjects.insert(std::make_pair(mainCamera->GetObj()->GetId().index(), mainCamera->GetObj()));

		return mainCamera;

	}


	///////////////////////2D///////////////////////////////
	Object * ObjectManager::CreateQuad(const std::string & shaderPath, const std::string & texturePath)
	{
		Object* obj = m_Object2DCreater->CreateQuad(shaderPath, texturePath);
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
	/*void ObjectManager::DrawShadowMap(std::vector<Object*> objs)
	{
		
	}*/
	//void ObjectManager::DrawObjects()
	//{
	//	for (auto pair : m_EntityToObjects) {
	//		Object* obj = pair.second;
	//		DrawObject(obj);
	//	}
	//}
	//void ObjectManager::DrawObject(Object * obj)
	//{
	//	if (obj != nullptr &&obj->HasComponent<MeshRenderer>()) {
	//		auto transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
	//		obj->GetComponent<MeshRenderer>()->UpdateTransformMatrix(transformMatrix);

	//		//灯光单独处理 //ParallelLight只有方向所以不需要Draw()
	//		if (obj->HasComponent<PointLight>()  || obj->HasComponent<SpotLight>()) {
	//			obj->GetComponent<MeshRenderer>()->DrawLight();
	//		}
	//		else {
	//			obj->GetComponent<MeshRenderer>()->DrawMeshes();
	//		//	obj->GetComponent<MeshRenderer>()->DrawModel();
	//		}

	//	}
	//}
	//void ObjectManager::DrawObjectsExcept(std::vector<Object *>objs)
	//{

	//	for (auto pair : m_EntityToObjects) {
	//		Object* Obj = pair.second;

	//		bool canDraw = true;
	//		for (auto obj : objs) {
	//			if (Obj->GetId().id == obj->GetId().id) {
	//				canDraw = false;
	//			}
	//		}
	//		if(canDraw)
	//			DrawObject(Obj);
	//			
	//	}
	//}
	//void ObjectManager::DrawObjectsExcept(Object * obj)
	//{
	//	for (auto pair : m_EntityToObjects) {
	//		Object* Obj = pair.second;		
	//		if (Obj->GetId().id != obj->GetId().id)
	//			DrawObject(Obj);
	//	}
	//}
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