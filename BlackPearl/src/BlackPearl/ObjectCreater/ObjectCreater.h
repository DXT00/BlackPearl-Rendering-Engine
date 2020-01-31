#pragma once
#include "BlackPearl/Entity/Entity.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/LightProbes/LightProbes.h"

namespace BlackPearl {
	////////////////////////ObjectCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class ObjectCreater
	{
	public:
		ObjectCreater() {}
		ObjectCreater(EntityManager *entityManager)
			:m_EntityManager(entityManager) {}
		virtual ~ObjectCreater() {
			//if (m_EntityManager != nullptr) {//TODO::多个Creater共享一个m_EntityManager，所以有可能被删除多次
			//	delete m_EntityManager;
			//	m_EntityManager = nullptr;
			//}
		};
		Object* CreateEmpty(std::string name = "");

	protected:
		EntityManager* m_EntityManager;

	};

	////////////////////////Object3DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class Object3DCreater :public ObjectCreater{
	public:
		Object3DCreater(EntityManager *entityManager)
		{
			m_EntityManager = entityManager;
		}

		Object* CreateCube(const std::string& shaderPath, const std::string& texturePath,const std::string name = "Cube");
		Object* CreatePlane(const std::string& shaderPath, const std::string& texturePath);
		Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath);
		Object* CreateModel(std::string modelPath,std::string shaderPath);
		Object* CreateSkyBox( const std::vector<std::string>& textureFaces,const std::string& shaderPath);
		LightProbe* CreateLightProbe(const std::string& shaderPath, const std::string& texturePath);
	};

	////////////////////////Object2DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class Object2DCreater :public ObjectCreater{
	public:
		Object2DCreater(EntityManager *entityManager)
			:ObjectCreater(entityManager) {}

		Object* CreateQuad(const std::string& shaderPath, const std::string& texturePath);


	};
	////////////////////////LightCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class LightCreater :public ObjectCreater {

	public:
		LightCreater(EntityManager *entityManager)
			:ObjectCreater(entityManager) {}

		Object* CreateLight(LightType type, LightSources* lightSources);

	};
	////////////////////////CameraCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class CameraCreater :public ObjectCreater {

	public:
		CameraCreater(EntityManager *entityManager)
			:ObjectCreater(entityManager) {}

		Object* CreateCamera();

	};
}
