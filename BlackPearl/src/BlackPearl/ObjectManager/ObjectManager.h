#pragma once
#include <unordered_map>
#include <string>
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Renderer/Mesh/Vertex.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/RHIDevice.h"

namespace BlackPearl {
	class ObjectManager
	{
	public:
		ObjectManager()
		{
		}

		~ObjectManager() 
		{
			DestroyObjects();
		};
		void RegisterDeviceManager(DeviceManager* deviceManager);

		Object* CreateEmpty(std::string name = "");
		Object* CreateLight(LightType type,LightSources* lightSources, const std::string& name);
		Object* CreateModel(
			const std::string& modelPath,
			const std::string& shaderPath,
			const bool isAnimated,
			const bool vertices_sorted,
			const bool addBondingBox,
			const std::string& name,
			const bool createMeshlet = false,
			const bool isMeshletModel = false,
			MeshletOption options = MeshletOption());

		Object* CreateCube(const std::string& shaderPath,const std::string& texturePath, const std::string& name);
		Object* CreateSphere(const float radius, const unsigned int stackCount, const unsigned int sectorCount, const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		Object* CreatePlane(const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		Object* CreateSkyBox(const std::vector<std::string>& textureFaces, const std::string& shaderPath, const std::string& name);
		Object* CreateQuad(const std::string& shaderPath , const std::string& texturePath, const std::string& name);
		Object* CreateTerrain(const std::string& shaderPath, const std::string& heightMapPath, const std::string& texturePath, uint32_t chunkCntX, uint32_t chunkCntZ, const std::string name);
		BatchNode* CreateBatchNode(std::vector<Object*> objs, bool dynamic, const std::string& name);
		/*Blending Object, include logical and actual properties*/
		Object* CreateLightProbe(ProbeType type, const std::string& shaderPath, const std::string& texturePath, const std::string& name);
		MainCamera* CreateCamera(const std::string& name);
		Object* CreateGroup(const std::string name);
		Object* CreateBVHNode(const std::vector<Object*>& objs, const std::string name = "BVHNode");
		Object* CreateBVHNode(const std::vector<Vertex>& mesh_vertex, const std::string name = "BVHNode");
		Object* CreateTriangle(const std::vector<Vertex>& points, const std::string name = "Triangle");
		//RayTracing ¹ÜÏßµÄTransform
		Object* CreateRTXTransformNode(const glm::mat4& transform_mat, Object* bvh_node, std::shared_ptr<Material> rtx_material = nullptr, const std::string name = "RayTracingTansform");
		
		std::vector<Object*> GetObjects();


		void DestroyObjects();

	private:
		std::vector<Object*> m_Objs;
		DeviceManager* m_DeviceManager;
		IDevice* m_Device;

	};
}

