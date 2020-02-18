#pragma once
#include<vector>
#include<string>
#include"BlackPearl/Renderer/Shader/Shader.h"
#include"BlackPearl/Renderer/Buffer.h"
#include"BlackPearl/Component/LightComponent/LightSources.h"
#include"BlackPearl/Renderer/Material/MaterialColor.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include <initializer_list>
namespace BlackPearl {

	struct Vertex {

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitTangent;

	};
	class Mesh
	{
	public:
		Mesh() {};
		/*one vertexBuffer*/
		Mesh(
			float* vertices,
			uint32_t verticesSize,
			unsigned int* indices,
			uint32_t indicesSize,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout
		)
			:m_Vertices(vertices),m_Indices(indices), m_IndicesSize(indicesSize),
			m_Material(material), m_VertexBufferLayout(layout) {
			Init( verticesSize);
		};
		/*multiple vertexBuffers*/
		Mesh(std::shared_ptr<Material>& material,
			std::shared_ptr<IndexBuffer> indexBuffer,
			std::vector< std::shared_ptr<VertexBuffer>> vertexBuffers) {
			m_VertexArray.reset(DBG_NEW VertexArray());
			m_IndicesSize = indexBuffer->GetIndicesSize();
			m_Material = material;
			m_VertexArray->SetIndexBuffer(indexBuffer);
			
			for (auto vertexBuffer : vertexBuffers) {
				m_VertexBufferLayout = vertexBuffer->GetBufferLayout();
				m_VertexArray->AddVertexBuffer(vertexBuffer);
			}
		}
		/*one vertexBuffer*/
		Mesh(
			std::vector<float>vertices,
			std::vector<unsigned int>indices,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout
		)
			: m_IndicesSize(indices.size() * sizeof(unsigned int)),
			m_Material(material), m_VertexBufferLayout(layout) {

			m_Vertices = DBG_NEW float[vertices.size()];
			if (vertices.size() > 0)
				memcpy(m_Vertices, &vertices[0], vertices.size() * sizeof(float));//注意memcpy最后一个参数是字节数!!!

			m_Indices = DBG_NEW unsigned int[indices.size()];
			if (indices.size() > 0)
				memcpy(m_Indices, &indices[0], indices.size() * sizeof(unsigned int));
			Init(vertices.size() * sizeof(float));
		};
		~Mesh();
	
		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		uint32_t GetIndicesSize()const { return m_IndicesSize; }
		unsigned int* GetIndicesAddress()const {
			return m_Indices;
		}
		/*vertices size = vertices.size()*sizeof(type)*/
		unsigned int GetVerticesSize(unsigned int vertexBufferId)const { 
			return m_VertexArray->GetVertexBuffers()[vertexBufferId]->GetVertexSize();
		
		}
		std::shared_ptr<Material> GetMaterial()const { return m_Material; }
		VertexBufferLayout GetVertexBufferLayout() const { return m_VertexBufferLayout; }
		////Draw Light
		//void Draw(const glm::mat4 & model);
		//void Draw(const glm::mat4 & model, const LightSources& lightSources);

		void SetTexture(const std::shared_ptr<Texture>& texture) { m_Material->SetTexture(texture); }
		void SetShader(const std::string& image) { m_Material->SetShader(image);};
		void SetShader(const std::shared_ptr<Shader> &shader) { m_Material->SetShader(shader);};
		void SetMaterialColor(MaterialColor::Color color) { m_Material->SetMaterialColor(color); }


	private:
		void Init(uint32_t verticesSize);
		std::shared_ptr<VertexArray> m_VertexArray;
		VertexBufferLayout           m_VertexBufferLayout;
		float*                       m_Vertices = nullptr;
		unsigned int*                m_Indices = nullptr;
		uint32_t                     m_IndicesSize = 0;
		std::shared_ptr<Material>    m_Material;

	};

}