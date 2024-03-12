#include "pch.h"
#include "RayTraceScene.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include <queue>
namespace BlackPearl {
	void RayTraceScene::BuildRayTraceData()
	{
		int matId = 0;
		uint32_t triId = 0;
		for (size_t objid = 0; objid < m_ObjectList.size(); objid++)
		{
			Object* obj = m_ObjectList[objid];
			matId = obj->GetComponent<MeshRenderer>()->GetSingleMaterial()->GetId();
			objTransforms.push_back(obj->GetComponent<Transform>()->GetTransformMatrix());
			BVHNode* bvhNode = obj->GetComponent<BVHNode>();
			if (bvhNode && !bvhNode->GetTriangleMesh().empty()) {
				for (uint32_t i = 0; i < bvhNode->GetTriangleMesh().size(); i++)
				{
					Object* triObj = bvhNode->GetTriangleMesh()[i];
					Triangle* tri = triObj->GetComponent<Triangle>();
					if (tri) {
						RayTraceScene::triangle rtTri;
						rtTri.v0 = tri->GetPoints()[0].position;
						rtTri.v1 = tri->GetPoints()[1].position;
						rtTri.v2 = tri->GetPoints()[2].position;
						rtTri.materialIndex = matId;
						rtTri.objIndex = objid;
						triangles.push_back(rtTri);
						triObj->GetComponent<class Triangle>()->Id = (int)Tris.size();
						Tris.push_back(triObj);

						objects.push_back({ triId, rtTri });

						if (obj->GetComponent<MeshRenderer>()->GetSingleMaterial()->GetRTXType() == Material::RTX_EMISSION) {
							float area = glm::length(glm::cross(rtTri.v0, rtTri.v1)) * 0.5f;
							lights.push_back({ i, area });
						}

						triId++;
					}
				}
			}
		}

		m_Root = g_objectManager->CreateBVHNode(Tris, "SceneRoot");
		
		_BuildSceneRayTraceBVHNode(m_Root);

		RayTraceScene::material gray{ Material::RTXType::RTX_DIFFUSE, glm::vec3(0.3f, 0.3f, 0.3f) };
		RayTraceScene::material red{ Material::RTXType::RTX_DIFFUSE, glm::vec3(0.9f, 0.1f, 0.1f) };
		/*RayTraceScene::material green{Material::RTXType::RTX_DIFFUSE, glm::vec3(0.1f, 0.9f, 0.1f)}; */
		RayTraceScene::material whiteLight{ Material::RTXType::RTX_EMISSION, glm::vec3(2.0f, 2.0f, 5.0f) };
		/*RayTraceScene::material metal{ Material::RTXType::RTX_METALLIC, glm::vec3(1.0f, 1.0f, 1.0f) };
		RayTraceScene::material glass{ Material::RTXType::RTX_DIELECTRIC, glm::vec3(1.0f, 1.0f, 1.0f) };*/

		materials.push_back(gray);
		materials.push_back(red);
		/*materials.push_back(green);*/
		materials.push_back(whiteLight);
		//materials.push_back(metal);
		//materials.push_back(glass);


	}
	std::vector<RayTraceScene::triangle> RayTraceScene::GetSceneTriangles()
	{
		return triangles;
	}

	std::vector<RayTraceScene::bvhNode> RayTraceScene::GetSceneBVHNode()
	{
		return bvhNodes;
	}

	std::vector<RayTraceScene::material> RayTraceScene::GetSceneMaterial()
	{
		
		return materials;
	}

	std::vector<RayTraceScene::light> RayTraceScene::GetSceneLight()
	{
		return lights;
	}

	std::vector<glm::mat4> RayTraceScene::GetObjTransforms()
	{
		return objTransforms;
	}

	void RayTraceScene::_BuildSceneRayTraceBVHNode(Object* root)
	{
		GE_ASSERT(bvhNodes.empty(),"bvhNodes has builded");
		int level = 1;
		std::vector<RayTraceScene::bvhNode> ret;
		Object* p = root;
		std::queue<Object*> queue;
		int nodeCounter = 0;
		queue.push(p);
		while (!queue.empty()) {
			int n = queue.size();
			for (size_t i = 0; i < n; i++)
			{
				Object* obj = queue.front();
				BVHNode* bvhNode = obj->GetComponent<BVHNode>();
				queue.pop();
				if (bvhNode) {
					RayTraceScene::bvhNode node;
					node.min = bvhNode->GetRootBox().GetMinP();
					node.max = bvhNode->GetRootBox().GetMaxP();
					if (bvhNode->IsLeaf()) {
						node.objectIndex = bvhNode->GetLeafObj()->GetComponent<Triangle>()->Id;
					}
					else {
						node.objectIndex = -1;
					}
					Object* left = bvhNode->GetLeft();
					Object* right = bvhNode->GetRight();

					if (left) {
						nodeCounter++;
						node.leftNodeIndex = nodeCounter;
						queue.push(left);
					}
					if (right) {
						nodeCounter++;
						node.rightNodeIndex = nodeCounter;
						queue.push(right);
					}
					bvhNodes.push_back(node);
				}
			}
			level++;
			
		}
		m_BVHLevel = level;

	}
}
