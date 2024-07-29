#pragma once
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
#include "BlackPearl/Renderer/Mesh/MeshGatherer.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/MainCamera/MainCamera.h"

namespace BlackPearl {
	class IndirectCullRenderer : public BasicRenderer
	{

	public:
		struct IndirectVertex
		{
			float pos[3];
			float normal[3];
			float texcoords[2];
		};
		struct DrawElementsCommand
		{
			GLuint vertexCount;
			GLuint instanceCount;
			GLuint firstIndex;
			GLuint baseVertex;
			GLuint baseInstance;
		};
		struct SDrawElementsCommand
		{
			GLuint vertexCount;
			GLuint instanceCount;
			GLuint firstIndex;
			GLuint baseVertex;
			GLuint baseInstance;
		};
		enum IndirectSlot {
			POS_SLOT = 0,
			NORMAL_SLOT,
			TEXCOORD_SLOT,
			DRAWID_SLOT,
			MODELS_SLOT,
			MODELSV1_SLOT,
			MODELSV2_SLOT,
			MODELSV3_SLOT
		};

		enum ComputeShaderSlot {
			CountBuffer_Slot, // record cmd.instance after culling, if mesh is occluded cmd.instance = 0
			DispactchObjThreads_Slot,
			ObjsInfoData_Slot,
			MeshInfoData_Slot,
			IndirectCmds_Slot,
			VisibleObjs_Slot,
			COUNT
		};

		struct DispatchIndirect {
			uint32_t numX;
			uint32_t numY;
			uint32_t numZ;
		};

		IndirectCullRenderer();
		~IndirectCullRenderer();
		void Render(std::shared_ptr<Shader> indirectShader,  MainCamera* cam);
		void RenderDepthMap(Object* Quad);
		void Cull();
		void Clear();
		void FillterCommands();

		void ShowDepthMap(int mipmap);

		void Init(Scene* scene);
		void UpdateTransform();
		void UpdateDrawCommands();

		void GenerateIndirectDrawCommands();
		//void GernerateIndirectData();
		void GernerateInterleaveIndirectData();

		void CreateAttributesBuffer();



	private:
		void InitIndirectData();
		void CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, float* indirectBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt);
		void CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, uint32_t* indirectBuffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt);
		void DestructTransformMatrix(uint32_t objId, std::pair<float*, uint32_t> batchTransformMatrix);
		void AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat);

		void PrintData();
		Scene* m_Scene;
		MeshGatherer* m_MeshGatherer;

		std::vector<std::shared_ptr<ShaderStorageBuffer>> m_ComputeShaderSSBOs;
		std::shared_ptr<FrameBuffer> m_HizFrameBuffer;
		TextureHandle m_DepthTexture;
		std::shared_ptr<Shader> m_CullingShader;
		std::shared_ptr<Shader> m_FillCmdsShader;
		std::shared_ptr<Shader> m_CmdsClearShader;
		std::shared_ptr<Shader> m_DepthShader;
		std::shared_ptr<Shader> m_ShowShader;
		std::shared_ptr<Shader> m_HizShader;
		std::vector<IndirectCommand> m_Commands;
		std::shared_ptr<VertexArray> m_VertexArray;
		std::shared_ptr<IndirectBuffer> m_IndirectBuffer;
		std::shared_ptr<VertexBuffer> m_MatrixVBO;
		std::shared_ptr<VertexBuffer> m_VertexVBO;

		std::shared_ptr<IndexBuffer> m_IndexIBO;

		uint32_t m_ObjsCnt = 0;
		uint32_t m_VertexCnt = 0;
		uint32_t m_IndexCnt = 0;
		uint32_t m_MeshCnt = 0;
		uint32_t m_MipmapLevel;

		IndirectVertex* m_InterleaveVertexBuffer = nullptr;
		// per objs matrix 
		glm::mat4* m_ObjsTransformBuffer = nullptr;
		uint32_t* m_IndexBuffer = nullptr;


		float* m_PositionBuffer = nullptr;
		float* m_NormalBuffer = nullptr;
		float* m_TexCordBuffer = nullptr;
		float* m_TangentBuffer = nullptr;
		float* m_BitangentBuffer = nullptr;
		uint32_t* m_JointIndicesBuffer = nullptr;
		uint32_t* m_JointIndices1Buffer = nullptr;
		float* m_WeightBuffer = nullptr;
		float* m_Weight1Buffer = nullptr;
		float* m_ColorBuffer = nullptr;


		float* m_ObjIdBuffer = nullptr;

		//3 * vec4 of matrix
		float* m_ObjsTransformV0Buffer = nullptr;
		float* m_ObjsTransformV1Buffer = nullptr;
		float* m_ObjsTransformV2Buffer = nullptr;


	};
}
