#include "pch.h"
#include "GenData_HV.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
#include "BlackPearl/Component/MeshFilterComponent/SphereMeshFilter.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
namespace BlackPearl {
	GenData_HV::GenData_HV(){ }


	void GenData_HV::ParseMatData(Object* obj)
	{
		//这里注意obj添加Material时要指定Material Type
		bool has_material = false;
		if (obj == NULL) return;
		if (obj->HasComponent<MeshRenderer>()) {
			 std::vector<Mesh> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
			 if (!meshes.empty()) {
				 //TODO:: RayTracing 暂时默认所有Meshes是同一种类型的材质
				 std::shared_ptr<Material> material = meshes[0].GetMaterial();
				 if (material != NULL) {
					 m_Mat2Vec[material].push_back(m_SceneData.size());
					 has_material = true;
				 }
			 }
		}
		m_SceneData.push_back(-1);
		m_SceneData.push_back(has_material);

	}

	void GenData_HV::ParseSceneData(Object* obj,std::vector<float>& packData) 
	{
		ObjectType type = obj->GetComponent<BasicInfo>()->GetObjectType();
		if (type == OT_Group) {
			ParseGroupData(obj, packData);
		}
		else if (type == OT_Sphere) {
			ParseSphereData(obj, packData);
		}
		else if (type == OT_BVH_Node) {
			ParseBVHNodeData(obj, packData);
		}
	}

	void GenData_HV::ParseGroupData(Object* group, std::vector<float>& packData)
	{
		if (group == NULL)
			return;
		if (m_Hitable2Idx.find(group) != m_Hitable2Idx.end())
			return;

		m_Hitable2Idx[group] = m_SceneData.size();

		m_SceneData.push_back(OT_Group);
		ParseMatData(group);

		size_t childSize = group->GetChildObjs().size();

		size_t childIdx = m_SceneData.size();

		for (size_t i = 0; i < childSize; i++) {
			m_SceneData.push_back(-float(m_Hitable2Idx[group]));
		}
		//m_SceneData.push_back(-float(m_Hitable2Idx[group]));
		m_SceneData.push_back(0);
		for (size_t i = 0; i < childSize; i++)
		{
			Object* child = group->GetChildObjs()[i];
			if (m_Hitable2Idx.find(child) != m_Hitable2Idx.end()) {
				m_SceneData[childIdx++] = m_Hitable2Idx[child];
				continue;
			}
			m_SceneData[childIdx++] = m_SceneData.size();
			ParseSceneData(group->GetChildObjs()[i], packData);
		}

	}

	void GenData_HV::ParseSphereData(Object* sphere, std::vector<float>& packData)
	{
		if (sphere == NULL)
			return;
		if (m_Hitable2Idx.find(sphere) != m_Hitable2Idx.end())
			return;
		m_Hitable2Idx[sphere] = m_SceneData.size();

		m_SceneData.push_back(OT_Sphere);
		ParseMatData(sphere);

		//size_t childSize = sphere->GetChildObjs().size();
		m_SceneData.push_back(packData.size() / 4);

		packData.push_back(sphere->GetComponent<Transform>()->GetPosition().x);
		packData.push_back(sphere->GetComponent<Transform>()->GetPosition().y);
		packData.push_back(sphere->GetComponent<Transform>()->GetPosition().z);
		packData.push_back(sphere->GetComponent<SphereMeshFilter>()->GetRadius());


	}
	void GenData_HV::ParseBVHNodeData(Object* bvh_node, std::vector<float>& packData)
	{
		if (bvh_node == NULL)
			return;
		if (m_Hitable2Idx.find(bvh_node) != m_Hitable2Idx.end())
			return;

		m_Hitable2Idx[bvh_node] = m_SceneData.size();

		m_SceneData.push_back(OT_BVH_Node);
		ParseMatData(bvh_node);

		m_SceneData.push_back(packData.size() / 4);

		const AABB box = bvh_node->GetComponent<BVHNode>()->GetRootBox();
		packData.push_back(box.GetMinP().x);
		packData.push_back(box.GetMinP().y);
		packData.push_back(box.GetMinP().z);
		packData.push_back(0);
		packData.push_back(box.GetMaxP().x);
		packData.push_back(box.GetMaxP().y);
		packData.push_back(box.GetMaxP().z);
		packData.push_back(0);


		size_t curChildIt = m_SceneData.size();
		// left
		if (bvh_node->GetComponent<BVHNode>()->GetLeft())
			m_SceneData.push_back(-1);
		// right
		if (bvh_node->GetComponent<BVHNode>()->GetRight())
			m_SceneData.push_back(-1);

		// childrenEnd 用以标识结尾
		m_SceneData.push_back(-float(m_Hitable2Idx[bvh_node]));

		if (bvh_node->GetComponent<BVHNode>()->GetLeft()) {
			auto targetLeftIdx = m_Hitable2Idx.find(bvh_node->GetComponent<BVHNode>()->GetLeft());
			if (targetLeftIdx == m_Hitable2Idx.end()) {
				m_SceneData[curChildIt++] = m_SceneData.size();
				ParseSceneData(bvh_node->GetComponent<BVHNode>()->GetLeft(), packData);
			}
			else
				m_SceneData[curChildIt++] = targetLeftIdx->second;
		}

		if (bvh_node->GetComponent<BVHNode>()->GetRight()) {
			auto targetRightIdx = m_Hitable2Idx.find(bvh_node->GetComponent<BVHNode>()->GetRight());
			if (targetRightIdx == m_Hitable2Idx.end()) {
				m_SceneData[curChildIt++] = m_SceneData.size();
				ParseSceneData(bvh_node->GetComponent<BVHNode>()->GetRight(), packData);

			}
			else
				m_SceneData[curChildIt++] = targetRightIdx->second;
		}



	}
	//把MatData id 填充到 m_SceneData
	void GenData_HV::SetMat(std::map<std::shared_ptr<Material>, size_t> const mat2idx)
	{
		for (auto const& pair : mat2idx) {
			auto target = m_Mat2Vec.find(pair.first);
			if (target != m_Mat2Vec.end()) {
				for (auto const& idx : target->second)
					m_SceneData[idx] = pair.second;
			}
		}
	}




}