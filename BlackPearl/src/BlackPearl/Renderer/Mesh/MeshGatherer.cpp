#include "pch.h"
#include "MeshGatherer.h"
#include "BlackPearl/Node/SingleNode.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/BoundingBoxComponent/BoundingBox.h"

namespace BlackPearl {
	MeshGatherer::MeshGatherer(Scene* scene, GatherType type)
	{
		m_Scene = scene;
		if (type == GatherSingleNode) {
			GatherSingle();
		}
		m_VAO = std::make_shared<VertexArray>(LayOutSlot::COUNT, false/*interleaved*/);
		FillVAO();
	}
	MeshGatherer::~MeshGatherer()
	{
		GE_SAVE_DELETE(m_PositionBuffer);
		GE_SAVE_DELETE(m_NormalBuffer);
		GE_SAVE_DELETE(m_TexCordBuffer);
		GE_SAVE_DELETE(m_TangentBuffer);
		GE_SAVE_DELETE(m_BitangentBuffer);
		GE_SAVE_DELETE(m_IndexBuffer);

	}

	void MeshGatherer::InitMeshBuffers()
	{
		m_VertexBufferLayout = {
		{ElementDataType::Float3,"aPos",false,POS_SLOT},
		{ElementDataType::Float3,"aNormal",false,NORMAL_SLOT},
		{ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT},
		{ElementDataType::Float3,"aTangent",false,TANGENT_SLOT},
		{ElementDataType::Float3,"aBitangent",false,BITANGENT_SLOT}
		};

		m_PositionBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_NormalBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_TexCordBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 2);
		m_TangentBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_BitangentBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
		m_ObjsTransformBuffer = (glm::mat4*)malloc(m_ObjsCnt * sizeof(glm::mat4));
 		m_IndexBuffer = (uint32_t*)malloc(m_IndexCnt * sizeof(uint32_t));

		//m_ObjInfoBuffers = (ObjInfo*)malloc(m_ObjsCnt * sizeof(ObjInfo));
		m_ObjInfoBuffers = (float*)malloc(m_ObjsCnt * sizeof(float) * 8);

		memset(m_PositionBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_NormalBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_TexCordBuffer, 0, m_VertexCnt * sizeof(float) * 2);
		memset(m_TangentBuffer, 0, m_VertexCnt * sizeof(float) * 3);
		memset(m_BitangentBuffer, 0, m_VertexCnt * sizeof(float) * 3);

	}
	void MeshGatherer::GatherSingle()
	{
		for (size_t i = 0; i < m_Scene->GetSingleNodes().size(); i++)
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i));
			m_VertexCnt += singleNode->GetVertexCount();
			m_IndexCnt += singleNode->GetIndexCount();
			m_MeshCnt += singleNode->GetObj()->GetComponent<MeshRenderer>()->GetMeshes().size();
			m_ObjsCnt++;
		}

		InitMeshBuffers();

		uint32_t baseVert = 0;
		uint32_t baseMesh = 0;
		uint32_t baseIndex = 0;
		uint32_t baseObj = 0;


		for (auto node : m_Scene->GetSingleNodes())
		{
			SingleNode* singleNode = dynamic_cast<SingleNode*>(node);
			Object* obj = singleNode->GetObj();
			auto& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
			std::vector<uint32_t> matIds;
			uint32_t startMesh = baseMesh;
		

			for (auto mesh : meshes) {
				matIds.push_back(mesh->GetMaterial()->GetId());

				uint32_t meshVetCnt = mesh->GetVertexCount();
				uint32_t meshIndexCnt = mesh->GetIndicesConut();

				CopyMeshBufferToBatchBuffer(baseVert, m_PositionBuffer, mesh->GetPositionBuffer(), mesh, POS_SLOT, 3);
				CopyMeshBufferToBatchBuffer(baseVert, m_NormalBuffer, mesh->GetNormalBuffer(), mesh, NORMAL_SLOT, 3);
				CopyMeshBufferToBatchBuffer(baseVert, m_TexCordBuffer, mesh->GetTexCoordsBuffer(), mesh, TEXCOORD_SLOT, 2);
				CopyMeshBufferToBatchBuffer(baseVert, m_TangentBuffer, mesh->GetTangentBuffer(), mesh, TANGENT_SLOT, 3);
				CopyMeshBufferToBatchBuffer(baseVert, m_BitangentBuffer, mesh->GetBitangentBuffer(), mesh, BITANGENT_SLOT, 3);


				uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;
				for (size_t i = 0; i < meshIndexCnt; i++)
				{
					uint32_t idex = indexBuffer[i];
					m_IndexBuffer[baseIndex + i] = indexBuffer[i] + baseVert;
				}
	
				baseVert += mesh->GetVertexCount();
				baseIndex += mesh->GetIndicesConut();
				baseMesh++;
			}

			AddObjTransformToBuffer(baseObj, singleNode->GetObj()->GetComponent<Transform>()->GetTransformMatrix());
			
			glm::vec3 bondingBox = glm::vec3{ obj->GetComponent<BoundingBox>()->Get().GetExtent() };
			ObjInfo info = {
				glm::vec2(startMesh,meshes.size()),
				bondingBox,
				obj->GetComponent<Transform>()->GetPosition()
			};
			AppendObjsBuf(baseObj, info);
			baseObj++;

		//	AppendMeshIdxBuf({ baseMesh, static_cast<uint32_t>(meshes.size())});
		}

	}
	void MeshGatherer::CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, float* batchBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt)
	{

		if (mesh->GetVertexBufferLayout().HasElement(elementSlot) && meshBuffer.first) {
			memcpy(batchBuffer + vertexCnt * itemCnt, meshBuffer.first, meshBuffer.second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}
	}

	void MeshGatherer::FillVAO()
	{
		std::shared_ptr<VertexBuffer> PosVBO = std::make_shared<VertexBuffer>(m_PositionBuffer, m_VertexCnt * sizeof(float) * 3, false/*interleaved*/);
		PosVBO->SetBufferLayout({ {ElementDataType::Float3,"aPos",false,POS_SLOT } });

		std::shared_ptr<VertexBuffer> NormalVBO = std::make_shared<VertexBuffer>(m_NormalBuffer, m_VertexCnt * sizeof(float) * 3, false/*interleaved*/);
		NormalVBO->SetBufferLayout({ {ElementDataType::Float3,"aNormal",false,NORMAL_SLOT } });


		std::shared_ptr<VertexBuffer> TexCoordsVBO = std::make_shared<VertexBuffer>(m_TexCordBuffer, m_VertexCnt * sizeof(float) * 2, false/*interleaved*/);
		TexCoordsVBO->SetBufferLayout({ {ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT } });

		std::shared_ptr<VertexBuffer> TangentVBO = std::make_shared<VertexBuffer>(m_TangentBuffer, m_VertexCnt * sizeof(float) * 3, false/*interleaved*/);
		TangentVBO->SetBufferLayout({ {ElementDataType::Float3,"aTangent",false,TANGENT_SLOT } });

		std::shared_ptr<VertexBuffer> BitangentVBO = std::make_shared<VertexBuffer>(m_BitangentBuffer, m_VertexCnt * sizeof(float) * 3, false/*interleaved*/);
		BitangentVBO->SetBufferLayout({ {ElementDataType::Float3,"aBitangent",false,BITANGENT_SLOT } });

		std::shared_ptr<VertexBuffer> ModelVBO = std::make_shared<VertexBuffer>(reinterpret_cast<float*>(m_ObjsTransformBuffer), m_ObjsCnt * sizeof(glm::mat4), true/*interleaved*/, true/*divisor*/, 1/*perInstance*/);
		ModelVBO->SetBufferLayout({ { ElementDataType::Float4,"aModel",false,MODEL_SLOT},
			{ ElementDataType::Float4,"aModelV1",false,MODEL_SLOT + 1} ,
			{ ElementDataType::Float4,"aModelV2",false,MODEL_SLOT + 2},
			{ ElementDataType::Float4,"aModelV3",false,MODEL_SLOT + 3} });

		m_VAO->SetVertexBuffer(POS_SLOT, PosVBO);
		m_VAO->SetVertexBuffer(NORMAL_SLOT, NormalVBO);
		m_VAO->SetVertexBuffer(TEXCOORD_SLOT, TexCoordsVBO);
		m_VAO->SetVertexBuffer(TANGENT_SLOT, TangentVBO);
		m_VAO->SetVertexBuffer(BITANGENT_SLOT, BitangentVBO);

		m_VAO->SetVertexBuffer(MODEL_SLOT, ModelVBO);
		m_VAO->SetIndexBuffer(std::make_shared<IndexBuffer>(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t)));

	}

	void MeshGatherer::AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat)
	{
		m_ObjsTransformBuffer[objId] = modelMat;
	}

	void MeshGatherer::AppendObjsBuf(uint32_t objId, ObjInfo info)
	{
		m_ObjInfoBuffers[objId * 8] = info.mesh.x;
		m_ObjInfoBuffers[objId * 8 + 1] = info.mesh.y;
		m_ObjInfoBuffers[objId * 8 + 2] = info.boundingBox.x;
		m_ObjInfoBuffers[objId * 8 + 3] = info.boundingBox.y;
		m_ObjInfoBuffers[objId * 8 + 4] = info.boundingBox.z;
		m_ObjInfoBuffers[objId * 8 + 5] = info.pos.x;
		m_ObjInfoBuffers[objId * 8 + 6] = info.pos.y;
		m_ObjInfoBuffers[objId * 8 + 7] = info.pos.z;

	}

	void MeshGatherer::AppendMeshIdxBuf(MeshIdxInfo meshInfo)
	{
		//m_MeshIdxBuffers.push_back(meshInfo);
	}


	
}