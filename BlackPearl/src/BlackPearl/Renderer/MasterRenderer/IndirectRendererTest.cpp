#include "pch.h"
#include "IndirectRendererTest.h"
#include "BlackPearl/Node/BatchNode.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"

namespace BlackPearl {
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
	struct Matrix
	{
		float a0, a1, a2, a3;
		float b0, b1, b2, b3;
		float c0, c1, c2, c3;
		float d0, d1, d2, d3;
	};
	struct Vertex2D
	{
		float x, y;  //Position
		float u, v;  //Uv
	};

	struct Vertex3D
	{
		float x, y, z;  //Position
		float n0, n1, n2;  //Normal
		float u, v;  //Uv
	};

	void setMatrix(Matrix* matrix, const float x, const float y, float scale = 1.0)
	{
		/*
		1 0 0 0
		0 1 0 0
		0 0 1 0
		x y 0 1
		*/
		matrix->a0 = 1* scale;
		matrix->a1 = matrix->a2 = matrix->a3 = 0;

		matrix->b1 = 1 * scale;
		matrix->b0 = matrix->b2 = matrix->b3 = 0;

		matrix->c2 = 1 * scale;
		matrix->c0 = matrix->c1 = matrix->c3 = 0;

		matrix->d0 = x;
		matrix->d1 = y;
		matrix->d2 = 0;
		matrix->d3 = 1;
	}
	const std::vector<Vertex2D> gQuad = {
		//xy			//uv
		{ 0.0f,0.0f,	0.0f,0.0f },
		{ 0.1f,0.0f,	1.0f,0.0f },
		{ 0.05f, 0.05f, 0.5f, 0.5f},
		{ 0.0f,0.1f,	0.0f,1.0f },
		{ 0.1f,0.1f,	1.0f,1.0f }
	};

	const std::vector<Vertex2D> gTriangle =
	{
		{ 0.0f, 0.0f,	0.0f,0.0f},
		{ 0.05f, 0.1f,	0.5f,1.0f},
		{ 0.1f, 0.0f,	1.0f,0.0f}
	};

	const std::vector<Vertex3D> gCube =
	{
			{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f   },
			{1.0f,  1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f	 },
			{1.0f,  1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f	 },
			{-1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f	 },

			{-1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
			{1.0f,  -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },
			{1.0f,  -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
			{-1.0f, -1.0f,  1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },

			{-1.0f, -1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{-1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{-1.0f,  1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
			{-1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },

			{1.0f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },
			{1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
			{1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
			{1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },

			{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f },
			{ 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f },
			{-1.0f,  1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },

			{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },
			{ 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
			{-1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	};
	const std::vector<unsigned int> gQuadIndex = {
		0,1,2,
		1,4,2,
		2,4,3,
		0,2,3
	};

	const std::vector<unsigned int> gTriangleIndex =
	{
		0,1,2
	};
	const std::vector<unsigned int> gCubeIndex =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};
	struct IndirectVertex
	{
		float pos[3];
		float normal[3];
		float texcoords[2];
	};
	IndirectRendererTest::IndirectRendererTest()
	{
		m_VertexArray = std::make_shared<VertexArray>();
	}

	IndirectRendererTest::~IndirectRendererTest()
	{
		GE_SAVE_FREE(m_PositionBuffer);
		GE_SAVE_FREE(m_NormalBuffer);
		GE_SAVE_FREE(m_TexCordBuffer);
		GE_SAVE_FREE(m_TangentBuffer);
		GE_SAVE_FREE(m_BitangentBuffer);
		GE_SAVE_FREE(m_JointIndicesBuffer);
		GE_SAVE_FREE(m_JointIndices1Buffer);
		GE_SAVE_FREE(m_WeightBuffer);
		GE_SAVE_FREE(m_Weight1Buffer);
		GE_SAVE_FREE(m_ColorBuffer);
		GE_SAVE_FREE(m_ObjIdBuffer);
		GE_SAVE_FREE(m_IndexBuffer);
		GE_SAVE_FREE(m_ObjsTransformV0Buffer);
		GE_SAVE_FREE(m_ObjsTransformV1Buffer);
		GE_SAVE_FREE(m_ObjsTransformV2Buffer);

	}

	void IndirectRendererTest::Render(const std::shared_ptr<Shader>& shader)
	{

		TestGenerateDrawCommands();
		glMultiDrawElementsIndirect(GL_TRIANGLES, //type
			GL_UNSIGNED_INT, //indices represented as unsigned ints
			(GLvoid*)0, //start with the first draw command
			100, //draw 100 objects
			0); //no stride, the draw commands are tightly packed

	}

	void IndirectRendererTest::Init(Scene* scene, const std::shared_ptr<Shader>& shader)
	{
		m_Scene = scene;
		////CreateAttributesBuffer();

		//CreateIndirectBuffer();


		shader->Bind();
	/*	GernerateIndirectData1();
		CreateInterleaveVertexBuffer();*/
		glGenBuffers(1, &gIndirectBuffer);

		//GenerateIndirectDrawCommands();

		TestGenerateGeometry();

	}

	void IndirectRendererTest::TestGenerateGeometry()
	{
		//Generate 50 quads, 50 triangles
		const unsigned num_vertices = gCube.size() * 100;
		std::vector<Vertex3D> vVertex(num_vertices);
		Matrix vMatrix[100];
		unsigned vertexIndex(0);
		unsigned matrixIndex(0);
		//Clipspace, lower left corner = (-1, -1)
	/*	float xOffset(-0.95f);
		float yOffset(-0.95f);*/
		float xOffset(-2.0f);
		float yOffset(-2.0f);
		// populate geometry
		for (unsigned int i(0); i != 10; ++i)
		{
			for (unsigned int j(0); j != 10; ++j)
			{
				for (unsigned int k(0); k != gCube.size(); ++k)
				{
					vVertex[vertexIndex++] = gCube[k];
				}

				////quad
				//if (j % 2 == 0)
				//{
				//	for (unsigned int k(0); k != gQuad.size(); ++k)
				//	{
				//		vVertex[vertexIndex++] = gQuad[k];
				//	}
				//}
				////triangle
				//else
				//{
				//	for (unsigned int k(0); k != gTriangle.size(); ++k)
				//	{
				//		vVertex[vertexIndex++] = gTriangle[k];
				//	}
				//}
				//set position in model matrix
				setMatrix(&vMatrix[matrixIndex++], xOffset, yOffset,0.2);
				xOffset += 2.0f;
			}
			yOffset += 2.0f;// 0.2f;
			xOffset = -2.0f;// -0.95f;
		}

		glGenVertexArrays(1, &gVAO);
		glBindVertexArray(gVAO);
		//Create a vertex buffer object
		glGenBuffers(1, &gVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * vVertex.size(), vVertex.data(), GL_STATIC_DRAW);

		//Specify vertex attributes for the shader
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(offsetof(Vertex3D, x)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(offsetof(Vertex3D, n0)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (GLvoid*)(offsetof(Vertex3D, u)));
		//Create an element buffer and populate it
		/*int triangle_bytes = sizeof(unsigned int) * gTriangleIndex.size();
		int quad_bytes = sizeof(unsigned int) * gQuadIndex.size();*/
		int cube_bytes = sizeof(unsigned int) * gCubeIndex.size();

		glGenBuffers(1, &gElementBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube_bytes, gCubeIndex.data(), GL_STATIC_DRAW);

		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quad_bytes, gQuadIndex.data());
		//glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quad_bytes, triangle_bytes, gTriangleIndex.data());

		//Setup per instance matrices
		//Method 1. Use Vertex attributes and the vertex attrib divisor
		glGenBuffers(1, &gMatrixBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, gMatrixBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vMatrix), vMatrix, GL_STATIC_DRAW);
		//A matrix is 4 vec4s
		glEnableVertexAttribArray(4 + 0);
		glEnableVertexAttribArray(4 + 1);
		glEnableVertexAttribArray(4 + 2);
		glEnableVertexAttribArray(4 + 3);

		glVertexAttribPointer(4 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*)(offsetof(Matrix, a0)));
		glVertexAttribPointer(4 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*)(offsetof(Matrix, b0)));
		glVertexAttribPointer(4 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*)(offsetof(Matrix, c0)));
		glVertexAttribPointer(4 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix), (GLvoid*)(offsetof(Matrix, d0)));
		//Only apply one per instance
		glVertexAttribDivisor(4 + 0, 1);
		glVertexAttribDivisor(4 + 1, 1);
		glVertexAttribDivisor(4 + 2, 1);
		glVertexAttribDivisor(4 + 3, 1);

		//Method 2. Use Uniform Buffers. Not shown here
	}

	void IndirectRendererTest::TestGenerateDrawCommands()
	{
		//Generate draw commands
		SDrawElementsCommand vDrawCommand[100];
		GLuint baseVert = 0;
		for (unsigned int i(0); i < 100; ++i)
		{
			vDrawCommand[i].vertexCount = 36;		//4 triangles = 12 vertices
			vDrawCommand[i].instanceCount = 1;		//Draw 1 instance
			vDrawCommand[i].firstIndex = 0;			//Draw from index 0 for this instance
			vDrawCommand[i].baseVertex = baseVert;	//Starting from baseVert
			vDrawCommand[i].baseInstance = i;		//gl_InstanceID
			baseVert += gCube.size();
		}

		//feed the draw command data to the gpu
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gIndirectBuffer);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(vDrawCommand), vDrawCommand, GL_DYNAMIC_DRAW);

		//feed the instance id to the shader.
		glBindBuffer(GL_ARRAY_BUFFER, gIndirectBuffer);
		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(SDrawElementsCommand), (void*)(offsetof(DrawElementsCommand, baseInstance)));
		glVertexAttribDivisor(3, 1); //only once per instance
	}

	void IndirectRendererTest::GenerateIndirectDrawCommands()
	{
		uint32_t commandsCnt = static_cast<uint32_t>(m_Scene->GetBatchNodes().size())
			+ static_cast<uint32_t>(m_Scene->GetSingleNodes().size());

		m_Commands.resize(commandsCnt);

		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;

		// //generate a indirect drawcall for each batch node or sigle node
		// for (auto node: m_Scene->GetBatchNodes())
		// {
		// 	// only support 1 instance batch for now.
		// 	BatchNode* batchNode = dynamic_cast<BatchNode*>(node);
		// 	//GE_ASSERT(batchNode->GetInstanceCnt() == 1, "only support 1 instance batch for now");
		// 	m_Commands[idx].vertexCntPerInstance = batchNode->GetVertexCount()/ batchNode->GetObjCnt();
		// 	m_Commands[idx].instanceCnt = batchNode->GetObjCnt();
		// 	m_Commands[idx].firstIndex = 0;
		// 	m_Commands[idx].startVertex = baseVert; 
		// 	m_Commands[idx].startInstance = baseInstance;
		// 	baseVert += batchNode->GetVertexCount();
		// 	baseIndex += batchNode->GetIndexCount();
		// 	m_VertexCnt += batchNode->GetVertexCount();
		// 	m_IndexCnt += batchNode->GetIndexCount();
		// 	m_ObjsCnt += batchNode->GetObjCnt();
		// 	baseInstance += batchNode->GetObjCnt();
		// 	idx++;
		// }

		for (int i = 0; i < m_Scene->GetSingleNodes().size(); i++)
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i));
			m_Commands[i].count = singleNode->GetIndexCount();
			m_Commands[i].instanceCnt = 1;
			m_Commands[i].firstIndex = 0;
			m_Commands[i].startVertex = baseVert;
			m_Commands[i].startInstance = i;

			baseVert += singleNode->GetVertexCount();
			baseIndex += singleNode->GetIndexCount();


			m_VertexCnt += singleNode->GetVertexCount();
			m_IndexCnt += singleNode->GetIndexCount();
			m_ObjsCnt += 1;
		}

		//feed the draw command data to the gpu
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, gIndirectBuffer);
		GE_ERROR_JUDGE();

		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectCommand)* m_Commands.size(), &m_Commands, GL_DYNAMIC_DRAW);
		GE_ERROR_JUDGE();
		//feed the instance id to the shader.
		glBindBuffer(GL_ARRAY_BUFFER, gIndirectBuffer);
		GE_ERROR_JUDGE();

		glEnableVertexAttribArray(3);
		GE_ERROR_JUDGE();

		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));
		GE_ERROR_JUDGE();

		glVertexAttribDivisor(3, 1); //only once per instance
		GE_ERROR_JUDGE();
	}


	void IndirectRendererTest::InitIndirectData()
	{
		m_PositionBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_NormalBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_TexCordBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 2);
		m_TangentBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_BitangentBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);

		m_JointIndicesBuffer = (uint32_t*)malloc(m_VertexCnt * sizeof(uint32_t) * 4);
		m_JointIndices1Buffer = (uint32_t*)malloc(m_VertexCnt * sizeof(uint32_t) * 4);
		m_WeightBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 4);
		m_Weight1Buffer = (float*)malloc(m_VertexCnt * sizeof(float) * 4);

		m_IndexBuffer = (uint32_t*)malloc(m_IndexCnt * sizeof(uint32_t));
		m_ObjIdBuffer = (float*)malloc(m_VertexCnt * sizeof(float));

		// only diffuse color now.
		m_ColorBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_ObjsTransformBuffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 16);
		m_ObjsTransformV0Buffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 4);
		m_ObjsTransformV1Buffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 4);
		m_ObjsTransformV2Buffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 4);


		m_InterleaveVertexBuffer = (float*)malloc(m_VertexCnt * sizeof(IndirectVertex));
		memset(m_InterleaveVertexBuffer, 0.0, m_VertexCnt * sizeof(IndirectVertex));


		memset(m_PositionBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_NormalBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_TexCordBuffer, 0, m_VertexCnt * sizeof(float) * 2);
		memset(m_TangentBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_BitangentBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_JointIndicesBuffer, 0, m_VertexCnt * sizeof(uint32_t) * 4);
		memset(m_JointIndices1Buffer, 0, m_VertexCnt * sizeof(uint32_t) * 4);
		memset(m_WeightBuffer, 0, m_VertexCnt * sizeof(float) * 4);
		memset(m_Weight1Buffer, 0, m_VertexCnt * sizeof(float) * 4);
		memset(m_IndexBuffer, 0, m_IndexCnt * sizeof(uint32_t));
		memset(m_ObjIdBuffer, 0, m_VertexCnt * sizeof(float));
		memset(m_ColorBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_ObjsTransformBuffer, 0.0, m_ObjsCnt * sizeof(float) * 16);
		memset(m_ObjsTransformV0Buffer, 0.0, m_ObjsCnt * sizeof(float) * 4);
		memset(m_ObjsTransformV1Buffer, 0.0, m_ObjsCnt * sizeof(float) * 4);
		memset(m_ObjsTransformV2Buffer, 0.0, m_ObjsCnt * sizeof(float) * 4);

	}

	void IndirectRendererTest::AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat) {
		glm::mat4 tmp = glm::transpose(modelMat);

		float m[12] = {
			tmp[0].x,
			tmp[0].y,
			tmp[0].z,
			tmp[0].w,
			tmp[1].x,
			tmp[1].y,
			tmp[1].z,
			tmp[1].w,
			tmp[2].x,
			tmp[2].y,
			tmp[2].z,
			tmp[2].w
		};
		memcpy(m_ObjsTransformBuffer + objId * 16, &modelMat[0][0], 16 * sizeof(float));
		/*memcpy(m_ObjsTransformV0Buffer + objId * 4, &tmp[0], 4 * sizeof(float));
		memcpy(m_ObjsTransformV1Buffer + objId * 4, &tmp[1], 4 * sizeof(float));
		memcpy(m_ObjsTransformV2Buffer + objId * 4, &tmp[2], 4 * sizeof(float));*/

	}
	void IndirectRendererTest::DestructTransformMatrix(uint32_t objId, std::pair<float*, uint32_t> batchTransformMatrix)
	{
		uint32_t batchObjCnt = batchTransformMatrix.second / (12.0 * sizeof(float));
		for (size_t i = 0; i < batchObjCnt; i++)
		{
			memcpy(m_ObjsTransformV0Buffer + objId * 4, batchTransformMatrix.first + i * 12, sizeof(float) * 4);
			memcpy(m_ObjsTransformV1Buffer + objId * 4, batchTransformMatrix.first + i * 12 + 4, sizeof(float) * 4);
			memcpy(m_ObjsTransformV2Buffer + objId * 4, batchTransformMatrix.first + i * 12 + 8, sizeof(float) * 4);

		}
	}


	void IndirectRendererTest::GernerateIndirectData()
	{
		InitIndirectData();
		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;
		uint32_t baseInstance = 0;

		for (auto node : m_Scene->GetBatchNodes())
		{
			// only support 1 instance batch for now.
			BatchNode* batchNode = dynamic_cast<BatchNode*>(node);
			//GE_ASSERT(batchNode->GetInstanceCnt() == 1, "only support 1 instance batch for now");
			memcpy(m_PositionBuffer + baseVert * 3, batchNode->GetBatch()->GetPositionBuffer().first, batchNode->GetBatch()->GetPositionBuffer().second);
			memcpy(m_NormalBuffer + baseVert * 3, batchNode->GetBatch()->GetNormalBuffer().first, batchNode->GetBatch()->GetNormalBuffer().second);
			memcpy(m_TexCordBuffer + baseVert * 2, batchNode->GetBatch()->GetTexCoordsBuffer().first, batchNode->GetBatch()->GetTexCoordsBuffer().second);
			memcpy(m_TangentBuffer + baseVert * 3, batchNode->GetBatch()->GetTangentBuffer().first, batchNode->GetBatch()->GetTangentBuffer().second);
			memcpy(m_BitangentBuffer + baseVert * 3, batchNode->GetBatch()->GetBitangentBuffer().first, batchNode->GetBatch()->GetBitangentBuffer().second);
			memcpy(m_JointIndicesBuffer + baseVert * 4, batchNode->GetBatch()->GetJiontBuffer().first, batchNode->GetBatch()->GetJiontBuffer().second);
			memcpy(m_JointIndices1Buffer + baseVert * 4, batchNode->GetBatch()->GetJiont1Buffer().first, batchNode->GetBatch()->GetJiont1Buffer().second);
			memcpy(m_WeightBuffer + baseVert * 4, batchNode->GetBatch()->GetWeightBuffer().first, batchNode->GetBatch()->GetWeightBuffer().second);
			memcpy(m_Weight1Buffer + baseVert * 4, batchNode->GetBatch()->GetWeight1Buffer().first, batchNode->GetBatch()->GetWeight1Buffer().second);
			memcpy(m_IndexBuffer + baseIndex, batchNode->GetBatch()->GetIndicesBuffer().first, batchNode->GetBatch()->GetIndicesBuffer().second);
			memcpy(m_ObjIdBuffer + baseVert, batchNode->GetBatch()->GetObjIdBuffer().first, batchNode->GetBatch()->GetObjIdBuffer().second);
			memcpy(m_ColorBuffer + baseVert * 3, batchNode->GetBatch()->GetColorBuffer().first, batchNode->GetBatch()->GetColorBuffer().second);

			//memcpy(m_ObjsTransformBuffer + baseInstance * 12, batchNode->GetBatch()->GetModelTransformBuffer().first, batchNode->GetBatch()->GetModelTransformBuffer().second);
			DestructTransformMatrix(baseInstance, batchNode->GetBatch()->GetModelTransformBuffer());

			baseVert += batchNode->GetVertexCount();
			baseIndex += batchNode->GetIndexCount();
			baseInstance += batchNode->GetObjCnt();
		}

		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();
			/*	uint32_t baseMeshVert = 0;
				uint32_t baseMeshIndex = 0;*/

			for (auto mesh : meshes)
			{
				uint32_t meshVetCnt = mesh->GetVertexCount();
				uint32_t meshIndexCnt = mesh->GetIndicesConut();

				CopyMeshBufferToIndirectBuffer(baseVert, m_PositionBuffer, mesh->GetPositionBuffer(), mesh, 3);
				CopyMeshBufferToIndirectBuffer(baseVert, m_NormalBuffer, mesh->GetNormalBuffer(), mesh, 3);
				CopyMeshBufferToIndirectBuffer(baseVert, m_TexCordBuffer, mesh->GetTexCoordsBuffer(), mesh, 2);
				CopyMeshBufferToIndirectBuffer(baseVert, m_TangentBuffer, mesh->GetTangentBuffer(), mesh, 3);
				CopyMeshBufferToIndirectBuffer(baseVert, m_BitangentBuffer, mesh->GetBitangentBuffer(), mesh, 3);
				CopyMeshBufferToIndirectBuffer(baseVert, m_JointIndicesBuffer, mesh->GetJiontBuffer(), mesh, 4);
				CopyMeshBufferToIndirectBuffer(baseVert, m_JointIndices1Buffer, mesh->GetJiont1Buffer(), mesh, 4);
				CopyMeshBufferToIndirectBuffer(baseVert, m_WeightBuffer, mesh->GetWeightBuffer(), mesh, 4);
				CopyMeshBufferToIndirectBuffer(baseVert, m_Weight1Buffer, mesh->GetWeight1Buffer(), mesh, 4);
				std::fill(m_ObjIdBuffer + baseVert, m_ObjIdBuffer + baseVert + meshVetCnt, baseInstance);

				std::shared_ptr<Material> material = mesh->GetMaterial();
				MaterialColor::Color materialColor = material->GetMaterialColor().Get();
				float diffuse[3] = { materialColor.diffuseColor.r,materialColor.diffuseColor.g,materialColor.diffuseColor.b };
				uint32_t floatsize = sizeof(float);

				for (size_t i = 0; i < meshVetCnt; i++)
				{
					memcpy(m_ColorBuffer + baseVert * 3 + i * 3, diffuse, sizeof(float) * 3);
				}

				uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;
				for (size_t i = 0; i < meshIndexCnt; i++)
				{
					m_IndexBuffer[baseIndex + i] = indexBuffer[i] + baseVert;
					GE_CORE_INFO("i = {0}, m_IndexBuffer[{1}] = {2} ", i, baseIndex + i, m_IndexBuffer[baseVert + i]);
				}

				/*baseMeshVert += mesh->GetVertexCount();
				baseMeshIndex += mesh->GetIndicesConut();*/

				AddObjTransformToBuffer(baseInstance, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
				baseVert += mesh->GetVertexCount();
				baseIndex += mesh->GetIndicesConut();
				baseInstance += 1;

			}


		}
	}
	void IndirectRendererTest::CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, uint32_t* indirectBuffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt)
	{

		if (meshBuffer.first) {
			memcpy(indirectBuffer + vertexCnt * itemCnt, meshBuffer.first, meshBuffer.second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}

	}

	void IndirectRendererTest::GernerateIndirectData1()
	{
		//InitIndirectData();
		m_VertexCnt = 0;
		m_IndexCnt = 0;
		m_ObjsCnt = 0;
		for (size_t i = 0; i < m_Scene->GetSingleNodesCnt(); i++)
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i));
			m_VertexCnt += singleNode->GetVertexCount();
			m_IndexCnt += singleNode->GetIndexCount();
			m_ObjsCnt++;
		}

		m_InterleaveVertexBuffer = (float*)malloc(m_VertexCnt * sizeof(IndirectVertex));
		m_IndexBuffer = (uint32_t*)malloc(m_IndexCnt * sizeof(uint32_t));
		m_ObjsTransformBuffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 16);
		m_ColorBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);

		uint32_t baseVert = 0;
		uint32_t baseIndex = 0;
		uint32_t baseInstance = 0;

		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			auto& meshes = singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes();


			for (auto mesh : meshes)
			{
				uint32_t meshVetCnt = mesh->GetVertexCount();
				uint32_t meshIndexCnt = mesh->GetIndicesConut();


				memcpy(m_InterleaveVertexBuffer + baseVert * sizeof(IndirectVertex) / sizeof(float), mesh->GetVertices(), mesh->GetVertexCount() * sizeof(IndirectVertex));


				std::shared_ptr<Material> material = mesh->GetMaterial();
				MaterialColor::Color materialColor = material->GetMaterialColor().Get();
				float diffuse[3] = { materialColor.diffuseColor.r,materialColor.diffuseColor.g,materialColor.diffuseColor.b };
				uint32_t floatsize = sizeof(float);

				for (size_t i = 0; i < meshVetCnt; i++)
				{
					memcpy(m_ColorBuffer + baseVert * 3 + i * 3, diffuse, sizeof(float) * 3);
				}

				uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;
				memcpy(m_IndexBuffer + baseIndex, indexBuffer, singleNode->GetIndexCount() * sizeof(uint32_t));
				for (size_t i = 0; i < meshIndexCnt; i++)
				{
					m_IndexBuffer[baseIndex + i] = indexBuffer[i];
					GE_CORE_INFO("i = {0}, m_IndexBuffer[{1}] = {2} ", i, baseIndex + i, m_IndexBuffer[baseVert + i]);
				}

				AddObjTransformToBuffer(baseInstance, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
				baseVert += singleNode->GetVertexCount();
				baseIndex += singleNode->GetIndexCount();
				baseInstance += 1;

			}


		}


		GE_CORE_INFO("sizeof(IndirectVertex)={0},sizeof(IndirectVertex) / sizeof(float) ={1} ", sizeof(IndirectVertex), sizeof(IndirectVertex) / sizeof(float));

		for (size_t i = 0; i < m_IndexCnt; i++)
		{
			//	m_IndexBuffer[ i] = m_IndexBuffer[i];
			GE_CORE_INFO("i = {0}, m_IndexBuffer[{1}] = {2} ", i, i, m_IndexBuffer[i]);
		}


		for (size_t i = 0; i < m_VertexCnt; i++)
		{
			uint32_t base = i;
			GE_CORE_INFO("i = {0}, m_InterleaveVertexBuffer[{1}] = {2},{3},{4},{5},{6},{7},{8},{9} ", i, i,
				m_InterleaveVertexBuffer[i * 8],
				m_InterleaveVertexBuffer[i * 8 + 1],
				m_InterleaveVertexBuffer[i * 8 + 2],
				m_InterleaveVertexBuffer[i * 8 + 3],
				m_InterleaveVertexBuffer[i * 8 + 4],
				m_InterleaveVertexBuffer[i * 8 + 5],
				m_InterleaveVertexBuffer[i * 8 + 6],
				m_InterleaveVertexBuffer[i * 8 + 7]
			);

		}

		for (size_t i = 0; i < m_ObjsCnt; i++)
		{
			uint32_t base = i;
			GE_CORE_INFO("i = {0}, m_ObjsTransformBuffer[{1}] = {2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17}", i, i,
				m_ObjsTransformBuffer[i * 16],
				m_ObjsTransformBuffer[i * 16 + 1],
				m_ObjsTransformBuffer[i * 16 + 2],
				m_ObjsTransformBuffer[i * 16 + 3],
				m_ObjsTransformBuffer[i * 16 + 4],
				m_ObjsTransformBuffer[i * 16 + 5],
				m_ObjsTransformBuffer[i * 16 + 6],
				m_ObjsTransformBuffer[i * 16 + 7],
				m_ObjsTransformBuffer[i * 16 + 8],
				m_ObjsTransformBuffer[i * 16 + 9],
				m_ObjsTransformBuffer[i * 16 + 10],
				m_ObjsTransformBuffer[i * 16 + 11],
				m_ObjsTransformBuffer[i * 16 + 12],
				m_ObjsTransformBuffer[i * 16 + 13],
				m_ObjsTransformBuffer[i * 16 + 14],
				m_ObjsTransformBuffer[i * 16 + 15]

			);

		}
		//glGenVertexArrays(1, &gVAO);
		//glBindVertexArray(gVAO);
		////Create a vertex buffer object
		//glGenBuffers(1, &gVertexBuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, gVertexBuffer);
		//glBufferData(GL_ARRAY_BUFFER, m_VertexCnt * sizeof(IndirectVertex), m_InterleaveVertexBuffer, GL_STATIC_DRAW);

		////Specify vertex attributes for the shader
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(IndirectVertex), (GLvoid*)(offsetof(IndirectVertex, pos)));
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(IndirectVertex), (GLvoid*)(offsetof(IndirectVertex, normal)));
		//glEnableVertexAttribArray(2);
		//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(IndirectVertex), (GLvoid*)(offsetof(IndirectVertex, texcoords)));

		////Create an element buffer and populate it
		//glGenBuffers(1, &gElementBuffer);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementBuffer);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCnt * sizeof(uint32_t), m_IndexBuffer, GL_STATIC_DRAW);

		//// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, quad_bytes, gQuadIndex.data());
		//// glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, quad_bytes, triangle_bytes, gTriangleIndex.data());

		////Setup per instance matrices
		////Method 1. Use Vertex attributes and the vertex attrib divisor
		//glGenBuffers(1, &gMatrixBuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, gMatrixBuffer);
		//glBufferData(GL_ARRAY_BUFFER, m_ObjsCnt * sizeof (float) *16, m_ObjsTransformBuffer, GL_STATIC_DRAW);
		////A matrix is 4 vec4s
		//glEnableVertexAttribArray(4 + 0);
		//glEnableVertexAttribArray(4 + 1);
		//glEnableVertexAttribArray(4 + 2);
		//glEnableVertexAttribArray(4 + 3);

		//glVertexAttribPointer(4 + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(0));
		//glVertexAttribPointer(4 + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(16));
		//glVertexAttribPointer(4 + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(32));
		//glVertexAttribPointer(4 + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(48));
		////Only apply one per instance
		//glVertexAttribDivisor(4 + 0, 1);
		//glVertexAttribDivisor(4 + 1, 1);
		//glVertexAttribDivisor(4 + 2, 1);
		//glVertexAttribDivisor(4 + 3, 1);
	}



	void IndirectRendererTest::CopyMeshBufferToIndirectBuffer(uint32_t vertexCnt, float* indirectBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, uint32_t itemCnt)
	{
		if (meshBuffer.first) {
			memcpy(indirectBuffer + vertexCnt * itemCnt, mesh->GetPositionBuffer().first, mesh->GetPositionBuffer().second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}
	}

	void IndirectRendererTest::CreateIndirectBuffer()
	{
		std::shared_ptr<IndirectBuffer> indirectBuffer = std::make_shared<IndirectBuffer>(m_Commands);

		m_VertexArray->SetIndirectBuffer(3, indirectBuffer);

	}


	void IndirectRendererTest::CreateAttributesBuffer()
	{
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(DBG_NEW IndexBuffer(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//如果每个vertexbuffer 一种attribute,那么 stride = 0, offset 也 = 0
		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(m_PositionBuffer, m_VertexCnt * sizeof(float) * 3));
		vertexBuffer->SetBufferLayout({ {ElementDataType::Float3, "aPos", false, POS_SLOT} });
		std::shared_ptr<VertexBuffer> normalBuffer(DBG_NEW VertexBuffer(m_NormalBuffer, m_VertexCnt * sizeof(float) * 3));
		normalBuffer->SetBufferLayout({ { ElementDataType::Float3,"aNormal",false,NORMAL_SLOT} });
		std::shared_ptr<VertexBuffer> texcoordsBuffer(DBG_NEW VertexBuffer(m_TexCordBuffer, m_VertexCnt * sizeof(float) * 2));
		texcoordsBuffer->SetBufferLayout({ { ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT} });
		std::shared_ptr<VertexBuffer> tangentBuffer(DBG_NEW VertexBuffer(m_TangentBuffer, m_VertexCnt * sizeof(float) * 3));
		tangentBuffer->SetBufferLayout({ { ElementDataType::Float3,"aTangent",false,TANGENT_SLOT} });
		std::shared_ptr<VertexBuffer> bitangentBuffer(DBG_NEW VertexBuffer(m_BitangentBuffer, m_VertexCnt * sizeof(float) * 3));
		bitangentBuffer->SetBufferLayout({ { ElementDataType::Float3,"aBitangent",false,BITANGENT_SLOT} });

		std::shared_ptr<VertexBuffer> jointBuffer(DBG_NEW VertexBuffer(m_JointIndicesBuffer, m_VertexCnt * sizeof(uint32_t) * 4));
		jointBuffer->SetBufferLayout({ { ElementDataType::Int4,"aJointIndices",false,JOINTINDICES_SLOT} });
		std::shared_ptr<VertexBuffer> joint1Buffer(DBG_NEW VertexBuffer(m_JointIndices1Buffer, m_VertexCnt * sizeof(uint32_t) * 4));
		joint1Buffer->SetBufferLayout({ { ElementDataType::Int4,"aJointIndices1",false,JOINTINDICES1_SLOT} });
		std::shared_ptr<VertexBuffer> weightBuffer(DBG_NEW VertexBuffer(m_WeightBuffer, m_VertexCnt * sizeof(float) * 4));
		weightBuffer->SetBufferLayout({ { ElementDataType::Float4,"aWeights",false,WEIGHT_SLOT} });
		std::shared_ptr<VertexBuffer> weight1Buffer(DBG_NEW VertexBuffer(m_Weight1Buffer, m_VertexCnt * sizeof(float) * 4));
		weight1Buffer->SetBufferLayout({ { ElementDataType::Float4,"aWeights1",false,WEIGHT1_SLOT} });

		std::shared_ptr<VertexBuffer> objIdBuffer(DBG_NEW VertexBuffer(m_ObjIdBuffer, m_VertexCnt * sizeof(float)));
		objIdBuffer->SetBufferLayout({ { ElementDataType::Float,"aObjId",false,OBJID_SLOT} });
		std::shared_ptr<VertexBuffer> colorBuffer(DBG_NEW VertexBuffer(m_ColorBuffer, m_VertexCnt * sizeof(float) * 3));
		colorBuffer->SetBufferLayout({ { ElementDataType::Float3,"aColor",false,COLOR_SLOT} });

		std::shared_ptr<VertexBuffer> matrixBuffer(DBG_NEW VertexBuffer(m_ObjsTransformBuffer, m_ObjsCnt * sizeof(float) * 16));
		matrixBuffer->SetBufferLayout({ { ElementDataType::Float4,"aModels",false,MODELS_SLOT},
			{ ElementDataType::Float4,"aModels",false,MODELS_SLOT + 1} ,
			{ ElementDataType::Float4,"aModels",false,MODELS_SLOT + 2} ,
			{ ElementDataType::Float4,"aModels",false,MODELS_SLOT + 3} });

		/*std::shared_ptr<VertexBuffer> matrixV0Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV0Buffer, m_ObjsCnt * sizeof(float) * 4));
		matrixV0Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec0",false,MODELSV0_SLOT} });
		std::shared_ptr<VertexBuffer> matrixV1Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV1Buffer, m_ObjsCnt * sizeof(float) * 4));
		matrixV1Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec1",false,MODELSV1_SLOT} });
		std::shared_ptr<VertexBuffer> matrixV2Buffer(DBG_NEW VertexBuffer(m_ObjsTransformV2Buffer, m_ObjsCnt * sizeof(float) * 4));
		matrixV2Buffer->SetBufferLayout({ { ElementDataType::Float4,"aModelsVec2",false,MODELSV2_SLOT} });*/

		m_VertexArray->AddAttributeVertexBuffer(vertexBuffer);
		m_VertexArray->AddAttributeVertexBuffer(normalBuffer);
		m_VertexArray->AddAttributeVertexBuffer(texcoordsBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(tangentBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(bitangentBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(jointBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(joint1Buffer);
		//m_VertexArray->AddAttributeVertexBuffer(weightBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(weight1Buffer);
		//m_VertexArray->AddAttributeVertexBuffer(colorBuffer);
		//m_VertexArray->AddAttributeVertexBuffer(objIdBuffer);
		/*m_VertexArray->AddVertexBuffer(matrixBuffer);
		glVertexAttribDivisor(MODELS_SLOT, 1);
		glVertexAttribDivisor(MODELS_SLOT+1, 1);
		glVertexAttribDivisor(MODELS_SLOT+2, 1);
		glVertexAttribDivisor(MODELS_SLOT+3, 1);*/

		/*	m_VertexArray->AddAttributeVertexBuffer(matrixV0Buffer);
			glVertexAttribDivisor(MODELSV0_SLOT, 1);

			m_VertexArray->AddAttributeVertexBuffer(matrixV1Buffer);
			glVertexAttribDivisor(MODELSV1_SLOT, 1);

			m_VertexArray->AddAttributeVertexBuffer(matrixV2Buffer);
			glVertexAttribDivisor(MODELSV2_SLOT, 1);*/


		m_VertexArray->UnBind();
	}

	void IndirectRendererTest::CreateInterleaveVertexBuffer()
	{
		m_VertexArray->Bind();
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(DBG_NEW IndexBuffer(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);


		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(m_InterleaveVertexBuffer, m_VertexCnt * sizeof(IndirectVertex)));

		vertexBuffer->SetBufferLayout({ { ElementDataType::Float3,"aPos",false,POS_SLOT},
			{ ElementDataType::Float3,"aNormal",false,NORMAL_SLOT} ,
			{ ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT}
			});
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		std::shared_ptr<VertexBuffer> matBuffer(DBG_NEW VertexBuffer(m_ObjsTransformBuffer, m_ObjsCnt * 16 * sizeof(float)));
		matBuffer->SetBufferLayout(
			{ { ElementDataType::Float4,"aModels",false,4 },
			{ ElementDataType::Float4,"aModels",false,4 + 1 },
			{ ElementDataType::Float4,"aModels",false,4 + 2 },
			{ ElementDataType::Float4,"aModels",false,4 + 3 } });
		m_VertexArray->AddVertexBuffer(matBuffer);
		GE_ERROR_JUDGE();

		//Only apply one per instance
		glVertexAttribDivisor(4 + 0, 1);
		glVertexAttribDivisor(4 + 1, 1);
		glVertexAttribDivisor(4 + 2, 1);
		glVertexAttribDivisor(4 + 3, 1);
		GE_ERROR_JUDGE();
		//m_VertexArray->UnBind();


	}

}

