#pragma once
#include "BlackPearl/Renderer/Mesh/Mesh.h"
namespace BlackPearl {
	class Batch
	{
	public:
		enum BatchSlot {
			POS_SLOT = 0,
			NORMAL_SLOT,
			TEXCOORD_SLOT,
			TANGENT_SLOT,
			BITANGENT_SLOT,
			JOINTINDICES_SLOT,
			JOINTINDICES1_SLOT,
			WEIGHT_SLOT,
			WEIGHT1_SLOT,
			OBJID_SLOT,
			COLOR_SLOT
		};
		Batch(const std::vector<Object*>& objs, bool dynamic);
		

		void AddObj(Object* obj);
		void AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat, uint32_t baseVertexCnt);
		void SetAttributes();
		void UpdateObjTransform(uint32_t objId, glm::mat4 modelMat);
		uint32_t GetIndexCount() const { return m_IndexCnt; }
		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		float* GetModelMatrix() const { return m_ModelTransformBuffer; }
		std::vector<Object*> GetObjects() const { return m_ObjectsList; }

	private:
		void CreateVertexArray();
		void AddMeshToBuffer(uint32_t objId, const std::vector<std::shared_ptr<Mesh>>& meshes, uint32_t& baseVertexCnt, uint32_t& baseIndexCnt);
		void AddMaterialIdToBuffer(uint32_t objId, uint32_t matId, uint32_t baseVertexCnt);
		void AddColorToBuffer(uint32_t objId, glm::vec3 color);
		void CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, float* buffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt);
		void CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, uint32_t* buffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt);
		std::vector<Object*> m_ObjectsList;

		float*	   m_VertexBuffer = nullptr;
		float*	   m_NormalBuffer = nullptr;
		float*	   m_TexCordBuffer = nullptr;
		float*	   m_TangentBuffer = nullptr;
		float*	   m_BitangentBuffer = nullptr;
		uint32_t*  m_JointIndicesBuffer = nullptr;
		uint32_t*  m_JointIndices1Buffer = nullptr;
		float*	   m_WeightBuffer = nullptr;
		float*	   m_Weight1Buffer = nullptr;
		float*     m_ColorBuffer = nullptr;
		float*     m_ObjIdBuffer = nullptr;
		uint32_t*  m_IndexBuffer = nullptr;
		float*     m_ModelTransformBuffer = nullptr;

		uint32_t m_ObjsCnt = 0;
		uint32_t m_VertexCnt = 0;
		uint32_t m_IndexCnt = 0;
		bool     m_IsDynamic = false;

		VertexBufferLayout  m_VertexBufferLayout;
		std::shared_ptr<VertexArray> m_VertexArray;


	};
}


