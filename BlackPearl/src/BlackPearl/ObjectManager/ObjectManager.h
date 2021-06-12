#pragma once
#include <unordered_map>
#include <string>
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/ObjectCreater/ObjectCreater.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/MainCamera/MainCamera.h"
namespace BlackPearl {
	class ObjectManager
	{
	public:
		ObjectManager(EntityManager* entityManager)
			:m_EntityManager(entityManager) {
			m_ObjectCreater   = DBG_NEW ObjectCreater(entityManager);
			m_Object3DCreater = DBG_NEW Object3DCreater(entityManager);
			m_Object2DCreater = DBG_NEW Object2DCreater(entityManager);
			m_LightCreater    = DBG_NEW LightCreater(entityManager);
			//m_CameraCreater   = new CameraCreater(entityManager);
		
		}
		~ObjectManager() {
			DestroyObjects();
			GE_SAVE_DELETE(m_EntityManager);
			GE_SAVE_DELETE(m_ObjectCreater);
			GE_SAVE_DELETE(m_Object3DCreater);
			GE_SAVE_DELETE(m_Object2DCreater);
			GE_SAVE_DELETE(m_LightCreater);
			
			/*for (auto& obj : m_EntityToObjects) {

				if (obj.second != nullptr)
					delete obj.second;
			}*/


		};

		Object* CreateEmpty(std::string name = "");
		Object* CreateLight(LightType type,LightSources* lightSources, const std::string& name);
		Object* CreateModel(const std::string& modelPath, const std::string& shaderPath, const bool isAnimated, const std::string& name);
		Object* CreateCube(const std::string& shaderPath,const std::string& texturePath, const std::string& name);
		Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		Object* CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		Object* CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string& name);
		Object* CreateQuad(const std::string& shaderPath , const std::string& texturePath, const std::string& name);
		/*Blending Object, include logical and actual properties*/
		Object* CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		MainCamera* CreateCamera(const std::string& name);
		Object* CreateGroup(const std::string name);


		std::vector<Object*> GetObjects();
		std::vector<std::string> GetObjectsName();


		void DestroyObjects();

	private:
		EntityManager* m_EntityManager;
		std::unordered_map<std::uint32_t, Object*> m_EntityToObjects;


		ObjectCreater*   m_ObjectCreater;
		Object3DCreater* m_Object3DCreater;
		Object2DCreater* m_Object2DCreater;
		LightCreater*    m_LightCreater;
	//	CameraCreater*   m_CameraCreater;

	};
}

