#include "pch.h"
#include "BoundingBoxBuilder.h"
#include "BlackPearl/Object/Object.h"
#include "BlackPearl/LayerScene/Layer.h"
#include "glm/glm.hpp"
#include "BlackPearl/RayTracing/Hitable.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"
#include "BlackPearl/Component/BVHNodeComponent/BVHNode.h"
#include "BlackPearl/Component/MeshFilterComponent/SphereMeshFilter.h"
namespace BlackPearl {

	/*struct Triangle :public Hitable {
		Vertex p[3];
	};*/
	extern ObjectManager* g_objectManager;
	BoundingBoxBuilder::BoundingBoxBuilder()
	{

	}

	AABB BoundingBoxBuilder::Build(Object* obj)
	{
		if (obj->GetComponent<BasicInfo>()->GetObjectType() == ObjectType::OT_Sphere)
			return SphereBoundingBox(obj);
		else if (obj->GetComponent<BasicInfo>()->GetObjectType() == ObjectType::OT_Sphere)
			return TriangleBoundingBox(obj);
		else if (obj->GetComponent<BasicInfo>()->GetObjectType() == ObjectType::OT_Model) {
			std::vector<BlackPearl::Vertex> verteices = obj->GetComponent<MeshRenderer>()->GetModel()->GetMeshVertex();
			obj->AddComponent<BVHNode>(verteices);
			AABB box = obj->GetComponent<BVHNode>()->GetRootBox();

			box.UpdateTransform(obj->GetComponent<Transform>()->GetTransformMatrix());

			obj->AddComponent<BoundingBox>(box);
			return box;
		}
	}

	//AABB BoundingBoxBuilder::Build(const std::vector<Vertex>& mesh_vertex) {

	//	/*std::vector<Triangle> triMesh;
	//	std::shared_ptr<BVHNode> bvhNode;
	//	if (mesh_vertex.size() % 3 != 0) {
	//		m_Box.IsValid = false;
	//		return AABB::InValid;
	//	}
	//	for (size_t i = 0; i < mesh_vertex.size(); i += 3)
	//	{
	//		Triangle tri;
	//		tri.p[0] = mesh_vertex[i];
	//		tri.p[1] = mesh_vertex[i+1];
	//		tri.p[2] = mesh_vertex[i+2];

	//		tri.GetBoundingBox

	//		triMesh.push_back(tri);

	//	}
	//	bvhNode.reset(DBG_NEW BVHNode(triMesh)); */
	//	return AABB();
	//}

	/*void BoundingBoxBuilder::ChangeMeshToBVHNode(const std::vector<Vertex>& mesh_vertex)
	{
	}*/

	AABB BoundingBoxBuilder::SphereBoundingBox(Object* obj)
	{
		glm::vec3 center = obj->GetComponent<Transform>()->GetPosition();
		glm::vec3 radius =glm::vec3(obj->GetComponent<SphereMeshFilter>()->GetRadius());
		glm::vec3 minP = center - radius;
		glm::vec3 maxP = center + radius;
		return AABB(minP,maxP);
	}

	AABB BoundingBoxBuilder::TriangleBoundingBox(Object* obj)
	{
		return AABB();
	}

	////��ʱ��֧��AnimationModel(AnimationModel vertexBuffer��unsigned int ���͵�)
	//AABB BoundingBoxBuilder::Build(const std::vector<Mesh>& meshes)
	//{
	//	std::vector<Triangle> TriMesh;
	//	for (const auto& mesh : meshes)
	//	{
	//		const std::vector<std::shared_ptr<VertexBuffer>>& vertexBuffers = mesh.GetVertexArray()->GetVertexBuffers();
	//		for (const auto& buffer : vertexBuffers)
	//		{
	//			uint32_t stride = buffer->GetBufferLayout().GetStride();
	//			uint32_t vertex_num = buffer->GetVertexSize() / stride;
	//			if (buffer->GetVerticesUInt != nullptr) {
	//				GE_CORE_ERROR("Not support uint vertex buffer!");
	//				return AABB::InValid;
	//			}
	//			size_t vertices_size = buffer->GetVertexSize() / sizeof(float);
	//			const float* vertices = buffer->GetVerticesFloat();
	//			VertexBufferLayout layout = buffer->GetBufferLayout();
	//			std::vector<BufferElement> elements =layout.GetElements();
	//			uint32_t cnt = 0;
	//			for (size_t i = 0; i < vertices_size; i += stride/sizeof(float))
	//			{
	//				if (cnt == 3) {
	//					cnt = 0;
	//					 
	//				}
	//				Vertex v;
	//				/*v.pos.x = vertices[i];
	//				v.pos.y = vertices[i];
	//				v.pos.z = vertices[i];*/

	//			}


	//		}



	//	}



	//	return AABB();
	//}


}