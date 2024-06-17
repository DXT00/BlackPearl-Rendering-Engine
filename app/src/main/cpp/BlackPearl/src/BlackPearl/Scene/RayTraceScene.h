#pragma once
#include "Scene.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "BlackPearl/ObjectManager/ObjectManager.h"
namespace BlackPearl {
	extern ObjectManager* g_objectManager;
	class RayTraceScene :public Scene
	{
		
	public:

		struct material {
			alignas(4)	Material::RTXType materialType;
			alignas(16) glm::vec3 albedo;
		};

		struct triangle {
			alignas(16) glm::vec3 v0;
			alignas(16) glm::vec3 v1;
			alignas(16) glm::vec3 v2;
			alignas(4) unsigned int materialIndex;
			alignas(4) unsigned int objIndex;
		};


		struct light {
			alignas(4) unsigned int triangleIndex;
			alignas(4) float area;
		};

		struct sphere {
			alignas(16) glm::vec4 s;
			alignas(4) unsigned int materialIndex;
		};

		struct bvhNode {
			alignas(16) glm::vec3 min;
			alignas(16) glm::vec3 max;
			alignas(4) int leftNodeIndex = -1;
			alignas(4) int rightNodeIndex = -1;
			alignas(4) int objectIndex = -1;
		};

		struct onb {
			glm::vec3 u;
			glm::vec3 v;
			glm::vec3 w;
		};
		struct Object0
		{
			uint32_t index;
			triangle t;
		};

	public:
		RayTraceScene() {
			
		}
		void BuildRayTraceData();
		std::vector<triangle> GetSceneTriangles();
		std::vector<bvhNode> GetSceneBVHNode();
		std::vector<material> GetSceneMaterial();
		std::vector<light> GetSceneLight();
		std::vector<glm::mat4> GetObjTransforms();

		int GetBVHLevel() const { return m_BVHLevel; }

	private:
		Object* m_Root = nullptr;
		std::vector<triangle> triangles;
		std::vector<material> materials;
		std::vector<light> lights;
		std::vector<bvhNode> bvhNodes;
		std::vector<glm::mat4> objTransforms;

		std::vector<Object0> objects;

		std::vector<Object* > Tris;

		void _BuildSceneRayTraceBVHNode(Object* root);

		int m_BVHLevel = 0;


	};


}
