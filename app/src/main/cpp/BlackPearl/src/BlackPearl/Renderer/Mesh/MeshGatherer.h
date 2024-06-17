#pragma once
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/VertexArray.h"
namespace BlackPearl {
	class MeshGatherer
	{
	public:
		enum LayOutSlot {
			POS_SLOT = 0,
			NORMAL_SLOT,
			TEXCOORD_SLOT,
			TANGENT_SLOT,
			BITANGENT_SLOT,
			DRAWID_SLOT,
			MODEL_SLOT,
			//JOINTINDICES_SLOT,
			//JOINTINDICES1_SLOT,
			//WEIGHT_SLOT,
			//WEIGHT1_SLOT,
			COUNT
		};


		struct ObjInfo {
			glm::vec2 mesh; //meshId = mesh.x; meshCnt = mesh.y
			glm::vec3 boundingBox; // bbox.xyz = bbox.extend, 
			glm::vec3 pos;
		};

		struct MeshIdxInfo
		{
			glm::vec2 pos; //pos.x = mesh.startidx, pos.y = mesh.cnt (num of mesh in a obj)
			glm::vec4 matId; //最大存储4种material， 每个mesh 一种的话，每个obj最多4个mesh
		};
		enum GatherType {
			GatherSingleNode,
			GatherBatchNode
		};

		MeshGatherer(Scene *scene, GatherType type);
		~MeshGatherer();
		uint32_t GetObjCnt() const { return m_ObjsCnt; }
		uint32_t GetMeshCnt() const { return m_MeshCnt; }
		//ObjInfo* GetObjInfoBuffer() const { return m_ObjInfoBuffers; }
		float* GetObjInfoBuffer() const { return m_ObjInfoBuffers; }

		MeshIdxInfo* GetMeshIdBuffer() const { return m_MeshIdxBuffers; }
		std::shared_ptr<VertexArray> GetVAO() const { return m_VAO; }

	private:
		void GatherSingle();
		void InitMeshBuffers();
		void CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, float* buffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt);
		void FillVAO();
		void AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat);

		void AppendObjsBuf(uint32_t objId, ObjInfo info);
		void AppendMeshIdxBuf(MeshIdxInfo meshInfo);

		std::shared_ptr<VertexArray> m_VAO;
		std::shared_ptr<IndexBuffer> m_IBO;

		//ObjInfo* m_ObjInfoBuffers;

		float* m_ObjInfoBuffers = nullptr;

		MeshIdxInfo* m_MeshIdxBuffers;

		/********************* buffers ***************************/
		// not consider animated node now
		uint32_t* m_IndexBuffer;
		float* m_PositionBuffer = nullptr;
		float* m_NormalBuffer = nullptr;
		float* m_TexCordBuffer = nullptr;
		float* m_TangentBuffer = nullptr;
		float* m_BitangentBuffer = nullptr;
		glm::mat4* m_ObjsTransformBuffer = nullptr;

		Scene* m_Scene;


		uint32_t m_ObjsCnt = 0;
		uint32_t m_VertexCnt = 0;
		uint32_t m_IndexCnt = 0;
		uint32_t m_MeshCnt = 0;


	};



}
