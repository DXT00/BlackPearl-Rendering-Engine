#pragma once
#include "BlackPearl/Scene/Scene.h"
#include "BlackPearl/Renderer/VertexArray.h"
#include "BlackPearl/Renderer/MasterRenderer/BasicRenderer.h"
namespace BlackPearl {
	class IndirectRendererTest : public BasicRenderer
	{

	public:
		enum IndirectSlot {
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
			COLOR_SLOT,
			MODELS_SLOT,
			DRAWID_SLOT,
			MODELSV0_SLOT,
			MODELSV1_SLOT,
			MODELSV2_SLOT
		};

		IndirectRendererTest();
		~IndirectRendererTest();
		void Render(const std::shared_ptr<Shader>& shader);
		void Init(Scene* scene, const std::shared_ptr<Shader>& shader);
		void GenerateIndirectDrawCommands();
		void GernerateIndirectData();
		void GernerateIndirectData1();

		void CreateIndirectBuffer();
		void CreateAttributesBuffer();
		void CreateInterleaveVertexBuffer();
		void AddIndirectDrawCommand(const IndirectCommand& commands);

		//Test Example

		void TestGenerateGeometry();
		void TestGenerateDrawCommands();

		GLuint gVAO;
		GLuint gArrayTexture;
		GLuint gVertexBuffer;
		GLuint gElementBuffer;
		GLuint gIndirectBuffer;
		GLuint gMatrixBuffer;
		GLuint gProgram;

		float gMouseX;
		float gMouseY;

	private:
		void InitIndirectData();
		void CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, float* indirectBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt);
		void CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, uint32_t* indirectBuffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt);
		void DestructTransformMatrix(uint32_t objId, std::pair<float*, uint32_t> batchTransformMatrix);
		void AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat);
		Scene* m_Scene;
		std::vector<IndirectCommand> m_Commands;
		std::shared_ptr<VertexArray> m_VertexArray;
		uint32_t m_ObjsCnt = 0;
		uint32_t m_VertexCnt = 0;
		uint32_t m_IndexCnt = 0;

		float* m_InterleaveVertexBuffer = nullptr;



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
		uint32_t* m_IndexBuffer = nullptr;

		// per objs matrix 
		float* m_ObjsTransformBuffer = nullptr;
		//3 * vec4 of matrix
		float* m_ObjsTransformV0Buffer = nullptr;
		float* m_ObjsTransformV1Buffer = nullptr;
		float* m_ObjsTransformV2Buffer = nullptr;


	};
}
