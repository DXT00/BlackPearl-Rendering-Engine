#include "pch.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <BlackPearl/Renderer/Shader/Shader.h>
#include "BlackPearl/Renderer/Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext/matrix_transform.hpp"

namespace BlackPearl {


	Mesh::~Mesh()
	{
		GE_SAVE_FREE(m_Indices);
		GE_SAVE_FREE(m_Vertices);
		GE_SAVE_FREE(m_Positions);
		GE_SAVE_FREE(m_TexCoords);
		GE_SAVE_FREE(m_Tangents);
		GE_SAVE_FREE(m_Bitangents);
		GE_SAVE_FREE(m_JointIndices);
		GE_SAVE_FREE(m_JointIndices1);
		GE_SAVE_FREE(m_Weight);
		GE_SAVE_FREE(m_Weight1);
		m_Material.reset();

		IndexResource.Reset();
		MeshletResource.Reset();
		UniqueVertexIndexResource.Reset();
		PrimitiveIndexResource.Reset();
		CullDataResource.Reset();
		MeshInfoResource.Reset();
		for (size_t i = 0; i < VertexResources.size(); i++)
		{
			VertexResources[i].Reset();
		}
	}

	uint32_t Mesh::GetVerticesSize(unsigned int vertexBufferId)
	{
		return m_VertexArray->GetVertexBuffers()[vertexBufferId]->GetVertexSize();
	}

	void Mesh::SetTessellation(uint32_t verticesPerTessPatch)
	{
		if (g_RHIType == DynamicRHI::Type::OpenGL)
			glPatchParameteri(GL_PATCH_VERTICES, verticesPerTessPatch);

	}

	void Mesh::SetVertexBufferLayout(const VertexBufferLayout& layout)
	{
		m_VertexBufferLayout = layout;	
	}

	void Mesh::Init(uint32_t verticesSize)
	{

		m_VertexArray.reset(DBG_NEW VertexArray());

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(DBG_NEW VertexBuffer(m_Vertices, verticesSize));

		if (m_IndicesSize != 0) {
			std::shared_ptr<IndexBuffer> indexBuffer;
			//indexBuffer.reset(new IndexBuffer(m_Indices));
			indexBuffer.reset(DBG_NEW IndexBuffer(m_Indices, m_IndicesSize));
			m_VertexArray->SetIndexBuffer(indexBuffer);
		}
		vertexBuffer->SetBufferLayout(m_VertexBufferLayout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		//calculate vertex cnt
		uint32_t attributeSizeofOneVertex = 0;
		for (size_t i = 0; i < m_VertexBufferLayout.GetElements().size(); i++)
		{
			BufferElement& element = m_VertexBufferLayout.GetElement(i);
			attributeSizeofOneVertex += element.GetElementCount();
		}
		m_VerticeCount = m_VerticeArrayCount / attributeSizeofOneVertex;


		ParseAttributes(m_VertexBufferLayout);

	}

	void Mesh::ParseAttributes(const VertexBufferLayout& layout)
	{
		uint32_t pos_cnt = 0;
		uint32_t normal_cnt = 0;
		uint32_t tex_cnt = 0;
		uint32_t tangent_cnt = 0;
		uint32_t bitangent_cnt = 0;
		//for animation model
		uint32_t jointindices_cnt = 0;
		uint32_t jointindices1_cnt = 0;
		uint32_t weight_cnt = 0;
		uint32_t weight1_cnt = 0;

		uint32_t vertexCount = 0;
		//uint32_t attributeSizeofOneVertex;
		//for (size_t i = 0; i < layout.GetElements().size(); i++)
		//{
		//	BufferElement& element = layout.GetElement(i);
		//	attributeSizeofOneVertex += element.GetElementCount();
		//}
		//m_VerticeCount = m_VerticeArrayCount / attributeSizeofOneVertex;
		std::map<uint32_t, std::string> slot_to_attribute;
		for (size_t i = 0; i < layout.GetElements().size(); i++)
		{
			BufferElement& element = layout.GetElement(i);
			slot_to_attribute[element.Location] = element.Name;
			if (element.Name == "aPos" || element.Name == "POSITION") {
				pos_cnt = element.GetElementCount();
				m_Positions = (float*)malloc(m_VerticeCount * sizeof(float) * pos_cnt);
				m_PositionsSize = m_VerticeCount * sizeof(float) * pos_cnt;
			}
			else if (element.Name == "aNormal" || element.Name == "NORMAL") {
				normal_cnt = element.GetElementCount();
				m_Normals = (float*)malloc(m_VerticeCount * sizeof(float) * normal_cnt);
				m_NormalsSize = m_VerticeCount * sizeof(float) * normal_cnt;
			}
			else if (element.Name == "aTexCoords" || element.Name == "TEXCOORD") {
				tex_cnt = element.GetElementCount();
				m_TexCoords = (float*)malloc(m_VerticeCount * sizeof(float) * tex_cnt);
				m_TexCoordsSize = m_VerticeCount * sizeof(float) * tex_cnt;
			}
			else if (element.Name == "aTangent" || element.Name == "TANGENT") {
				tangent_cnt = element.GetElementCount();
				m_Tangents = (float*)malloc(m_VerticeCount * sizeof(float) * tangent_cnt);
				m_TangentsSize = m_VerticeCount * sizeof(float) * tangent_cnt;
			}
			else if (element.Name == "aBitangent" || element.Name == "BITANGENT") {
				bitangent_cnt = element.GetElementCount();
				m_Bitangents = (float*)malloc(m_VerticeCount * sizeof(float) * bitangent_cnt);
				m_BitangentsSize = m_VerticeCount * sizeof(float) * bitangent_cnt;
			}
			else if (element.Name == "aJointIndices" ) {
				jointindices_cnt = element.GetElementCount();
				m_JointIndices = (uint32_t*)malloc(m_VerticeCount * sizeof(uint32_t) * jointindices_cnt);
				m_JointIndicesSize = m_VerticeCount * sizeof(float) * jointindices_cnt;
			}
			else if (element.Name == "aJointIndices1") {
				jointindices1_cnt = element.GetElementCount();
				m_JointIndices1 = (uint32_t*)malloc(m_VerticeCount * sizeof(uint32_t) * jointindices1_cnt);
				m_JointIndices1Size = m_VerticeCount * sizeof(float) * jointindices1_cnt;
			}
			else if (element.Name == "aWeights") {
				weight_cnt = element.GetElementCount();
				m_Weight = (float*)malloc(m_VerticeCount * sizeof(float) * weight_cnt);
				m_WeightSize = m_VerticeCount * sizeof(float) * weight_cnt;
			}
			else if (element.Name == "aWeights1") {
				weight1_cnt = element.GetElementCount();
				m_Weight1 = (float*)malloc(m_VerticeCount * sizeof(float) * weight1_cnt);
				m_Weight1Size = m_VerticeCount * sizeof(float) * weight1_cnt;
			}
		}
		uint32_t stride = layout.GetStride();
		uint32_t attributeSizeofOneVertex = stride / sizeof(float);
		
		for (size_t i = 0; i < m_VerticeCount; i++)
		{
			uint32_t offset = 0;
			std::map<uint32_t, std::string>::iterator it = slot_to_attribute.begin();
			for (; it != slot_to_attribute.end(); it++)
			{
				if (it->second == "aPos" || it->second == "POSITION") {
					memcpy(m_Positions + i * pos_cnt, m_Vertices + i * attributeSizeofOneVertex + offset,  pos_cnt * sizeof(float));
					offset += pos_cnt;
				}
				else if (it->second == "aNormal" || it->second == "NORMAL") {
					memcpy(m_Normals + i * normal_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, normal_cnt * sizeof(float));
					offset += normal_cnt;
				}
				else if (it->second == "aTexCoords" || it->second == "TEXCOORD") {
					memcpy(m_TexCoords + i * tex_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, tex_cnt * sizeof(float));
					offset += tex_cnt;
				}
				else if (it->second == "aTangent" || it->second == "TANGENT") {
					memcpy(m_Tangents + i * tangent_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, tangent_cnt * sizeof(float));
					offset += tangent_cnt;
				}
				else if (it->second == "aBitangent" || it->second == "BITANGENT") {
					memcpy(m_Bitangents + i * bitangent_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, bitangent_cnt * sizeof(float));
					offset += bitangent_cnt;
				}
				else if (it->second == "aJointIndices") {
					memcpy(m_JointIndices + i * jointindices_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, jointindices_cnt * sizeof(float));
					offset += jointindices_cnt;
				}
				else if (it->second == "aJointIndices1") {
					memcpy(m_JointIndices1 + i * jointindices1_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, jointindices1_cnt * sizeof(float));
					offset += jointindices1_cnt;
				}
				else if (it->second == "aWeights") {
					memcpy(m_Weight + i * weight_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, weight_cnt * sizeof(float));
					offset += weight_cnt;
				}
				else if (it->second == "aWeights1") {
					memcpy(m_Weight1 + i * weight1_cnt, m_Vertices + i * attributeSizeofOneVertex + offset, weight1_cnt * sizeof(float));
					offset += weight1_cnt;
				}
			}
		}
	}

	/*one vertexBuffer*/
	Mesh::Mesh(
		float* vertices,
		uint32_t verticesSize,
		unsigned int* indices,
		uint32_t indicesSize,
		std::shared_ptr<Material> material,
		const VertexBufferLayout& layout,
		bool tessellation,
		uint32_t verticesPerTessPatch)
		: m_Vertices(vertices),
		m_Indices(indices),
		m_IndicesSize(indicesSize),
		m_Material(material),
		m_VertexBufferLayout(layout),
		m_VerticeSize(verticesSize),
		m_NeedTessellation(tessellation)
	{
		m_VerticeArrayCount = verticesSize / sizeof(float);
		m_IndicesCount = indicesSize / sizeof(uint32_t);
		Init(m_VerticeSize);
		if (tessellation)
			SetTessellation(verticesPerTessPatch);
	};

	/*one vertexBuffer*/
	Mesh::Mesh(
		std::vector<float> vertices,
		std::vector<uint32_t> indices,
		std::shared_ptr<Material> material,
		const VertexBufferLayout& layout,
		bool tessellation,
		uint32_t verticesPerTessPatch)
		: m_Material(material),
		m_VertexBufferLayout(layout),
		m_NeedTessellation(tessellation)
	{
		
		m_IndicesCount = indices.size();
		m_IndicesSize = m_IndicesCount * sizeof(uint32_t);
		m_VerticeArrayCount = vertices.size();
		m_VerticeSize = m_VerticeArrayCount * sizeof(float);

		if (m_IndicesCount <= 0) {
			GE_CORE_WARN("indices cnt <=0 ");
		}
		else {
			m_Indices = (uint32_t*)malloc(m_IndicesCount * sizeof(uint32_t));
			memcpy(m_Indices, indices.data(), m_IndicesSize);
		}

		GE_ASSERT(m_VerticeArrayCount > 0, "vertices cnt <=0 ");
		m_Vertices = DBG_NEW float[m_VerticeArrayCount];
		memcpy(m_Vertices, &vertices[0], m_VerticeSize);//注意memcpy最后一个参数是字节数!!!

		Init(m_VerticeSize);
		if (tessellation)
			SetTessellation(verticesPerTessPatch);
	};

	/*multiple vertexBuffers*/
	Mesh::Mesh(std::shared_ptr<Material> material,
		std::shared_ptr<IndexBuffer> indexBuffer,
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers,
		bool tessellation,
		uint32_t verticesPerTessPatch) {
		m_VertexArray.reset(DBG_NEW VertexArray());
		m_IndicesSize = indexBuffer->GetIndicesSize();
		m_Material = material;
		m_NeedTessellation = tessellation;

		m_VertexArray->SetIndexBuffer(indexBuffer);

		//TODO:: 解析m_VerticeCount
		for (auto vertexBuffer : vertexBuffers) {
			m_VertexBufferLayout = vertexBuffer->GetBufferLayout();
			m_VertexArray->AddVertexBuffer(vertexBuffer);

			ParseAttributes(m_VertexBufferLayout);
		}

		if (tessellation)
			SetTessellation(verticesPerTessPatch);
	}
}