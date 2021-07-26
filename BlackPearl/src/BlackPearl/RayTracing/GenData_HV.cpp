#include "pch.h"
#include "GenData_HV.h"
#include "BlackPearl/Component/BasicInfoComponent/BasicInfo.h"
#include "BlackPearl/Component/MeshFilterComponent/SphereMeshFilter.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
#include "BlackPearl/Component/TransformComponent/RayTracingTransform.h"
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
		else if (type == OT_Triangle) {
			ParseTriangleData(obj, packData);
		}
		else if (type == OT_RTXTransformNode) {
			ParseRTXTransformNodeData(obj, packData);
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
	void GenData_HV::ParseTriangleData(Object* triangle, std::vector<float>& packData)
	{
		if (triangle == NULL)
			return;
		if (m_Hitable2Idx.find(triangle) != m_Hitable2Idx.end())
			return;
		m_Hitable2Idx[triangle] = m_SceneData.size();

		m_SceneData.push_back(OT_Triangle);
		ParseMatData(triangle);

		//size_t childSize = sphere->GetChildObjs().size();
		m_SceneData.push_back(packData.size() / 4);

		std::vector<Vertex>& points = triangle->GetComponent<Triangle>()->GetPoints();
		for (size_t i = 0; i < 3; i++)
		{
			packData.push_back(points[i].position.x);
			packData.push_back(points[i].position.y);
			packData.push_back(points[i].position.z);
			packData.push_back(points[i].texCoords.x);
			packData.push_back(points[i].normal.x);
			packData.push_back(points[i].normal.y);
			packData.push_back(points[i].normal.z);
			packData.push_back(points[i].texCoords.y);
		}
	
	}
	void GenData_HV::ParseRTXTransformNodeData(Object* rt_transform, std::vector<float>& packData)
	{
		if (rt_transform == NULL)
			return;
		if (m_Hitable2Idx.find(rt_transform) != m_Hitable2Idx.end())
			return;

		m_Hitable2Idx[rt_transform] = m_SceneData.size();
		m_SceneData.push_back(OT_RTXTransformNode);
		ParseMatData(rt_transform);

		m_SceneData.push_back(packData.size() / 4);


		glm::mat4 tfmMat4 = rt_transform->GetComponent<RTXTransformNode>()->GetTransformMatrix();
		for (size_t col = 0; col < 4; col++) {
			glm::vec4 colVec4 = tfmMat4[col];
			for (size_t row = 0; row < 4; row++)
				packData.push_back(colVec4[row]);
		}

		glm::mat4 invTfmMat4 = rt_transform->GetComponent<RTXTransformNode>()->GetInverseTransformMatrix();
		for (size_t col = 0; col < 4; col++) {
			glm::vec4 colVec4 = invTfmMat4[col];
			for (size_t row = 0; row < 4; row++)
				packData.push_back(colVec4[row]);
		}

		glm::mat3 normTfmMat3 = rt_transform->GetComponent<RTXTransformNode>()->GetNormalTransformMatrix();
		for (size_t col = 0; col < 3; col++) {
			glm::vec3 colVec3 = normTfmMat3[col];
			for (size_t row = 0; row < 3; row++)
				packData.push_back(colVec3[row]);
			packData.push_back(0);
		}

		Object* bvh_obj = rt_transform->GetComponent<RTXTransformNode>()->GetObj();
		if (bvh_obj == NULL) {
			m_SceneData.push_back(-float(m_Hitable2Idx[rt_transform]));
			return;
		}

		size_t childIt = m_SceneData.size();
		m_SceneData.push_back(-1);
		m_SceneData.push_back(-float(m_Hitable2Idx[rt_transform]));

		auto targetChildIdx = m_Hitable2Idx.find(bvh_obj);
		if (targetChildIdx == m_Hitable2Idx.end()) {
			m_SceneData[childIt] = m_SceneData.size();
			ParseSceneData(bvh_obj, packData);
		}
		else
			m_SceneData[childIt] = targetChildIdx->second;
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