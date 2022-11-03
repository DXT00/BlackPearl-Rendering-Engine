#include "pch.h"
#include "Batch.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Config.h"

namespace BlackPearl {

	// TODO:: animation model support is under development
	Batch::Batch(const std::vector<Object*>& objs, bool dynamic)
	{
		if (objs.empty())
			return;

		m_IsDynamic = dynamic;
		m_ObjectsList = objs;
		m_ObjsCnt = m_ObjectsList.size();
		GE_ASSERT(m_ObjsCnt < Configuration::MaxObjsInABatch, "objs count exceed Configuration::MaxObjsInABatch ");
		m_VertexBufferLayout = {
			{ElementDataType::Float3,"aPos",false,POS_SLOT},
			{ElementDataType::Float3,"aNormal",false,NORMAL_SLOT},
			{ElementDataType::Float2,"aTexCoords",false,TEXCOORD_SLOT},
			{ElementDataType::Float3,"aTangent",false,TANGENT_SLOT},
			{ElementDataType::Float3,"aBitangent",false,BITANGENT_SLOT},
			{ElementDataType::Int4,"aJointIndices",false,JOINTINDICES_SLOT },
			{ElementDataType::Int4,"aJointIndices1",false,JOINTINDICES1_SLOT},
		    {ElementDataType::Float4,"aWeights",false,WEIGHT_SLOT},
		    {ElementDataType::Float4,"aWeights1",false,WEIGHT1_SLOT},
			{ElementDataType::Float,"aObjId",false,OBJID_SLOT},
			{ElementDataType::Float3,"aColor",false,COLOR_SLOT},
		};

		for (auto& obj : m_ObjectsList) {
			std::vector<std::shared_ptr<Mesh>>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
			for (auto mesh: meshes)
			{
				m_VertexCnt += mesh->GetVertexCount();
				m_IndexCnt += mesh->GetIndicesCount();
			}
		}

		m_VertexBuffer = (float*)malloc(m_VertexCnt * sizeof(float) * 3);
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
		m_ModelTransformBuffer = (float*)malloc(m_ObjsCnt * sizeof(float) * 12);

		memset(m_VertexBuffer, 0, m_VertexCnt * sizeof(float) * 3);
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
		//memset(m_ModelTransformBuffer, 0.0, m_ObjsCnt * sizeof(float) * 12);
		memset(m_ModelTransformBuffer, 0.0, m_ObjsCnt * sizeof(float) * 12);

		SetAttributes();
	}

	void Batch::AddObj(Object* obj)
	{
		GE_ASSERT(m_ObjsCnt < Configuration::MaxObjsInABatch, "objs count exceed Configuration::MaxObjsInABatch ");



	}
	void Batch::SetAttributes()
	{
		uint32_t baseVertexCnt = 0;
		uint32_t baseIndexCnt = 0;
		for (size_t i = 0; i < m_ObjectsList.size(); i++)
		{
			AddMeshToBuffer(i, m_ObjectsList[i]->GetComponent<MeshRenderer>()->GetMeshes(), baseVertexCnt, baseIndexCnt);
			AddObjTransformToBuffer(i, m_ObjectsList[i]->GetComponent<Transform>()->GetTransformMatrix(), baseVertexCnt);
		}

		CreateVertexArray();
	}

	void Batch::UpdateObjTransform(uint32_t objId, glm::mat4 modelMat)
	{



	}

	void Batch::AddMeshToBuffer(uint32_t objId, const std::vector<std::shared_ptr<Mesh>>& meshes, uint32_t& baseVertexCnt, uint32_t& baseIndexCnt)
	{
		for (auto mesh : meshes) {
			uint32_t meshVetCnt = mesh->GetVertexCount();
			uint32_t meshIndexCnt = mesh->GetIndicesConut();

			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_VertexBuffer, mesh->GetPositionBuffer(), mesh, POS_SLOT, 3);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_NormalBuffer, mesh->GetNormalBuffer(), mesh, NORMAL_SLOT, 3);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_TexCordBuffer, mesh->GetTexCoordsBuffer(), mesh, TEXCOORD_SLOT,2);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_TangentBuffer, mesh->GetTangentBuffer(), mesh, TANGENT_SLOT, 3);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_BitangentBuffer, mesh->GetBitangentBuffer(), mesh, BITANGENT_SLOT, 3);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_JointIndicesBuffer, mesh->GetJiontBuffer(), mesh, JOINTINDICES_SLOT,4);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_JointIndices1Buffer, mesh->GetJiont1Buffer(), mesh, JOINTINDICES1_SLOT, 4);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_WeightBuffer, mesh->GetWeightBuffer(), mesh, WEIGHT_SLOT, 4);
			CopyMeshBufferToBatchBuffer(baseVertexCnt, m_Weight1Buffer, mesh->GetWeight1Buffer(), mesh, WEIGHT1_SLOT, 4);
			std::fill(m_ObjIdBuffer + baseVertexCnt, m_ObjIdBuffer + baseVertexCnt + meshVetCnt, objId);
			std::shared_ptr<Material> material = mesh->GetMaterial();
			MaterialColor::Color materialColor = material->GetMaterialColor().Get();
			float diffuse[3] = { materialColor.diffuseColor.r,materialColor.diffuseColor.g,materialColor.diffuseColor.b };
			uint32_t floatsize = sizeof(float);

			for (size_t i = 0; i < meshVetCnt; i++)
			{
				memcpy(m_ColorBuffer + baseVertexCnt * 3 + i * 3, diffuse, sizeof(float) * 3);
			}
			
			uint32_t* indexBuffer = mesh->GetIndicesBuffer().first;
			for (size_t i = 0; i < meshIndexCnt; i++)
			{
				uint32_t idex = indexBuffer[i];
				m_IndexBuffer[baseIndexCnt + i] = indexBuffer[i] + baseVertexCnt;
			}
	/*		for (size_t i = 0; i < meshVetCnt*3; i++)
			{
				GE_CORE_INFO("obj_id={0},i = {1}, baseVertexCnt = {2}, m_VertexBuffer[{3}] = {4} ", objId, i, baseVertexCnt, (baseVertexCnt * 3 + i), m_VertexBuffer[baseVertexCnt*3 + i]);

			}*/
			baseVertexCnt += mesh->GetVertexCount();
			baseIndexCnt += mesh->GetIndicesConut();
		}
	}

	void Batch::AddMaterialIdToBuffer(uint32_t objId, uint32_t matId, uint32_t baseVertexCnt)
	{
	}

	void Batch::AddColorToBuffer(uint32_t objId, glm::vec3 color)
	{
	}

	void Batch::CreateVertexArray()
	{

		m_VertexArray.reset(DBG_NEW VertexArray());
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(DBG_NEW IndexBuffer(m_IndexBuffer, m_IndexCnt * sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		//如果每个vertexbuffer 一种attribute,那么 stride = 0, offset 也 = 0
		std::shared_ptr<VertexBuffer> vertexBuffer(DBG_NEW VertexBuffer(m_VertexBuffer, m_VertexCnt * sizeof(float) * 3));
		vertexBuffer->SetBufferLayout({ {ElementDataType::Float3, "aPos", false, POS_SLOT} });
		std::shared_ptr<VertexBuffer> normalBuffer(DBG_NEW VertexBuffer(m_NormalBuffer, m_VertexCnt * sizeof(float) * 3));
		normalBuffer->SetBufferLayout({{ ElementDataType::Float3,"aNormal",false,NORMAL_SLOT}});
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


		m_VertexArray->AddAttributeVertexBuffer(vertexBuffer);
		m_VertexArray->AddAttributeVertexBuffer(normalBuffer);
		m_VertexArray->AddAttributeVertexBuffer(texcoordsBuffer);
		m_VertexArray->AddAttributeVertexBuffer(tangentBuffer);
		m_VertexArray->AddAttributeVertexBuffer(bitangentBuffer);
		m_VertexArray->AddAttributeVertexBuffer(jointBuffer);
		m_VertexArray->AddAttributeVertexBuffer(joint1Buffer);
		m_VertexArray->AddAttributeVertexBuffer(weightBuffer);
		m_VertexArray->AddAttributeVertexBuffer(weight1Buffer);
		m_VertexArray->AddAttributeVertexBuffer(colorBuffer);
		m_VertexArray->AddAttributeVertexBuffer(objIdBuffer);


		m_VertexArray->UnBind();




	}

	void Batch::CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, float* batchBuffer, const std::pair<float*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt)
	{

		if (mesh->GetVertexBufferLayout().HasElement(elementSlot) && meshBuffer.first) {
			memcpy(batchBuffer + vertexCnt * itemCnt, meshBuffer.first, meshBuffer.second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}

	}
	void Batch::CopyMeshBufferToBatchBuffer(uint32_t vertexCnt, uint32_t* batchBuffer, const std::pair<uint32_t*, uint32_t>& meshBuffer, const std::shared_ptr<Mesh>& mesh, int elementSlot, uint32_t itemCnt)
	{

		if (mesh->GetVertexBufferLayout().HasElement(elementSlot) && mesh->GetPositionBuffer().first) {
			memcpy(batchBuffer + vertexCnt * itemCnt, mesh->GetPositionBuffer().first, mesh->GetPositionBuffer().second);
		}
		else {
			//GE_CORE_WARN("no attirbute buffer found in mesh");
		}

	}
	void Batch::AddObjTransformToBuffer(uint32_t objId, glm::mat4 modelMat, uint32_t baseVertexCnt)
	{
		glm::mat4 tmp = glm::transpose(modelMat);
		/*
		 modelMat =
		( m[0].x, m[1].x, m[2].x, m[3].x
		  m[0].y, m[1].y, m[2].y, m[3].y
		  m[0].z, m[1].z, m[2].z, m[3].z
		  m[0].w, m[1].w, m[2].w, m[3].w
		//0         0       0       1
		  )
		*/

		/*
		tmp =
		 (
		  m[0].x, m[0].y, m[0].z, m[0].w
		  m[1].x, m[1].y, m[1].z, m[1].w
		  m[2].x, m[2].y, m[2].z, m[2].w
		  m[3].x, m[3].y, m[3].z, m[3].w
		  )
		*/

		/*
		m =
		( m[0].x, m[1].x, m[2].x, m[3].x
		  m[0].y, m[1].y, m[2].y, m[3].y
		  m[0].z, m[1].z, m[2].z, m[3].z
		//0         0       0       1
		  )
		*/
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
		memcpy(m_ModelTransformBuffer + objId * 12, &m[0], 12 * sizeof(float));

	}


}