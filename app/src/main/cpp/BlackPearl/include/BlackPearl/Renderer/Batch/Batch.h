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
		void AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat);
		void SetAttributes();
		uint32_t GetIndexCount() const { return m_IndexCnt; }
		uint32_t GetVertexCount() const { return m_VertexCnt; }

		//uint32_t GetInstanceCnt() const { return m_InstanceCnt; }

		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		float* GetModelMatrix() const { return m_ModelTransformBuffer; }
		std::vector<Object*> GetObjects() const { return m_ObjectsList; }

		void UpdateBatchPosition(math::float3 pos);
		void UpdateBatchScale(math::float3 scale);

		void UpdateObjsTransform();



		std::pair<float*, uint32_t> GetPositionBuffer() const { return { m_PositionBuffer, m_PositionsSize }; }
		std::pair<float*, uint32_t> GetNormalBuffer() const { return { m_NormalBuffer, m_NormalsSize }; }
		std::pair<float*, uint32_t> GetTexCoordsBuffer() const { return { m_TexCordBuffer, m_TexCoordsSize }; }
		std::pair<float*, uint32_t> GetTangentBuffer() const { return { m_TangentBuffer, m_TangentsSize }; }
		std::pair<float*, uint32_t> GetBitangentBuffer() const { return { m_BitangentBuffer, m_BitangentsSize }; }
		std::pair<uint32_t*, uint32_t> GetJiontBuffer() const { return { m_JointIndicesBuffer, m_JointIndicesSize }; }
		std::pair<uint32_t*, uint32_t> GetJiont1Buffer() const { return { m_JointIndices1Buffer, m_JointIndicesSize }; }
		std::pair<float*, uint32_t> GetWeightBuffer() const { return { m_WeightBuffer, m_WeightSize }; }
		std::pair<float*, uint32_t> GetWeight1Buffer() const { return { m_Weight1Buffer, m_WeightSize }; }
		std::pair<float*, uint32_t> GetColorBuffer() const { return { m_ColorBuffer, m_ColorSize }; }
		std::pair<float*, uint32_t> GetModelTransformBuffer() const { return { m_ModelTransformBuffer, m_ModelTransformBufferSize }; }
		std::pair<float*, uint32_t> GetObjIdBuffer() const { return { m_ObjIdBuffer, m_ObjIdSize }; }

		std::pair<uint32_t*, uint32_t> GetIndicesBuffer() const { return { m_IndexBuffer, m_IndexSize }; }

	private:
		void CreateVertexArray();
		void AddMeshToBuffer(uint32_t objId, const std::vector<std::shared_ptr<Mesh>>& meshes, uint32_t& baseVertexCnt, uint32_t& baseIndexCnt);
		void AddMaterialIdToBuffer(uint32_t objId, uint32_t matId, uint32_t baseVertexCnt);
		void AddColorToBuffer(uint32_t objId, math::float3 color);
		void CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, float* buffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt);
		void CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, uint32_t* buffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt);
		void UpdateModelTransformBuffer(uint32_t objId, glm::mat4 modelMat);

		std::vector<Object*> m_ObjectsList;
		//uint32_t   m_InstanceCnt;

		float*	   m_PositionBuffer = nullptr;
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


		uint32_t   m_PositionsSize = 0;
		uint32_t   m_NormalsSize = 0;
		uint32_t   m_TexCoordsSize = 0;
		uint32_t   m_TangentsSize = 0;
		uint32_t   m_BitangentsSize = 0;
		uint32_t   m_JointIndicesSize = 0;
		uint32_t   m_WeightSize = 0;
		uint32_t   m_ColorSize = 0;
		uint32_t   m_IndexSize = 0;
		uint32_t   m_ModelTransformBufferSize = 0;
		uint32_t   m_ObjIdSize = 0;

		uint32_t m_ObjsCnt = 0;
		uint32_t m_VertexCnt = 0;
		uint32_t m_IndexCnt = 0;
		bool     m_IsDynamic = false;

		VertexBufferLayout  m_VertexBufferLayout;
		std::shared_ptr<VertexArray> m_VertexArray;


	};
}


