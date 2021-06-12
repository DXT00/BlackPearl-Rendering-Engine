#include "pch.h"
#include "GenData_HV.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
#include "BlackPearl/Component/MeshFilterComponent/SphereMeshFilter.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"

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