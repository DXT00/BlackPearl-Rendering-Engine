#pragma once
#include "BlackPearl/Renderer/Mesh/Vertex.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/Renderer/Mesh/MeshletConfig.h"

namespace BlackPearl {
	////////////////////////ObjectCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class ObjectCreater
	{
	public:
		ObjectCreater(){}
		virtual ~ObjectCreater() {};
		Object* CreateEmpty(std::string name = "");

	};

	////////////////////////Object3DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class Object3DCreater :public ObjectCreater{
	public:
		Object3DCreater(){}
		Object* CreateCamera(const std::string name);
		Object* CreateCube(const std::string& shaderPath, const std::string& texturePath,const std::string name);
		Object* CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string name);
		Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string name);
		Object* CreateModel(
			std::string modelPath, 
			std::string shaderPath, 
			const bool isAnimated, 
			const bool vertices_sorted,
			const bool addBondingBox, 
			const std::string name,
			const bool createMeshlet = false,
			const bool isMeshletModel = false,
			MeshletOption options = MeshletOption());
		Object* CreateSkyBox( const std::vector<std::string>& textureFaces,const std::string& shaderPath, const std::string name);
		Object* CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string name);
		Object* CreateGroup(const std::string name);
		Object* CreateBVHNode(const std::vector<Object*>& objs,const std::string name);
		Object* CreateBVHNode(const std::vector<Vertex>& mesh_vertex, const std::string name);
		Object* CreateTriangle(const std::vector<Vertex>& points, const std::string name);
		Object* CreateRTXTransformNode(const glm::mat4& transform_mat, Object* bvh_node, std::shared_ptr<Material> rtx_material, const std::string name);
		Object* CreateTerrain(const std::string& shaderPath, const std::string& heightMapPath, const std::string& texturePath, uint32_t chunkCntX, uint32_t chunkCntZ, const std::string name);
	
	
	};

	////////////////////////Object2DCreater////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class Object2DCreater :public ObjectCreater{
	public:
		Object2DCreater(){}

		Object* CreateQuad(const std::string& shaderPath, const std::string& texturePath, const std::string name);


	};
	////////////////////////LightCreater//////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	class LightCreater :public ObjectCreater {

	public:
		LightCreater(){}

		Object* CreateLight(LightType type, LightSources* lightSources, const std::string name);

	};
	//////////////////////////CameraCreater//////////////////////////////////
	///////////////////////////////////////////////////////////////////////
	//class CameraCreater :public ObjectCreater {

	//public:
	//	CameraCreater(EntityManager *entityManager)
	//		:ObjectCreater(entityManager) {}

	//

	//};
}
