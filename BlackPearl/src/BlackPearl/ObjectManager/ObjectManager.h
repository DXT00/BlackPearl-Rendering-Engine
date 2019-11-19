#pragma once
#include <unordered_map>
#include <string>
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/ObjectCreater/ObjectCreater.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"

namespace BlackPearl {
	class ObjectManager
	{
	public:
		ObjectManager(EntityManager* entityManager)
			:m_EntityManager(entityManager) {
			m_ObjectCreater   = new ObjectCreater(entityManager);
			m_Object3DCreater = new Object3DCreater(entityManager);
			m_Object2DCreater = new Object2DCreater(entityManager);
			m_LightCreater    = new LightCreater(entityManager);
			m_CameraCreater   = new CameraCreater(entityManager);
		
		}
		~ObjectManager() {
			delete m_EntityManager;//TODO::×¢Òâ½âÎöË³Ðò

			delete m_ObjectCreater;
			delete m_Object3DCreater;
			delete m_Object2DCreater;
			delete m_LightCreater;

		};

		Object* CreateEmpty(std::string name = "");
		Object* CreateCamera();
		Object* CreateLight(LightType type,LightSources* lightSources);
		Object* CreateModel(const std::string& modelPath, const std::string& shaderPath);
		Object* CreateCube(const std::string& shaderPath,const std::string& texturePath);
		Object* CreatePlane(const std::string& shaderPath, const std::string& texturePath);
		Object* CreateSkyBox(const std::vector<std::string>& textureFaces);

		Object* CreateQuad(const std::string& shaderPath , const std::string& texturePath);



		std::vector<Object*> GetObjects();
		std::vector<std::string> GetObjectsName();

		void DrawObjects();
		void DrawObject(Object* obj);
		void DrawObjectsExcept(std::vector<Object *>objs);
		void DrawObjectsExcept(Object * obj);

		void DestroyObjects();

	private:
		EntityManager* m_EntityManager;
		std::unordered_map<std::uint32_t, Object*> m_EntityToObjects;


		ObjectCreater*   m_ObjectCreater;
		Object3DCreater* m_Object3DCreater;
		Object2DCreater* m_Object2DCreater;
		LightCreater*    m_LightCreater;
		CameraCreater*   m_CameraCreater;

	};
}

