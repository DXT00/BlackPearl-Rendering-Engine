#pragma once
#include<vector>
#include"BlackPearl/Renderer/Material/Texture.h"
#include"BlackPearl/Renderer/Shader.h"
#include"BlackPearl/Renderer/Buffer.h"
#include"BlackPearl/Component/LightComponent/LightSources.h"
#include"BlackPearl/Renderer/Material/MaterialColor.h"
#include "BlackPearl/Renderer/Material/Material.h"
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
		Mesh(
			float* vertices,
			uint32_t verticesSize,
			unsigned int* indices,
			uint32_t indicesSize,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout
		)
			:m_Vertices(vertices), m_VerticesSize(verticesSize), m_Indices(indices), m_IndicesSize(indicesSize),
			m_Material(material), m_VertexBufferLayout(layout) {
			Init();
		};

		Mesh(
			std::vector<float>vertices,
			std::vector<unsigned int>indices,
			std::shared_ptr<Material> material,
			const VertexBufferLayout& layout
		)
			:m_VerticesSize(vertices.size() * sizeof(float)), m_IndicesSize(indices.size() * sizeof(unsigned int)),
			m_Material(material), m_VertexBufferLayout(layout) {

			m_Vertices = DBG_NEW float[vertices.size()];
			if (vertices.size() > 0)
				memcpy(m_Vertices, &vertices[0], vertices.size() * sizeof(float));//注意memcpy最后一个参数是字节数!!!

			m_Indices = DBG_NEW unsigned int[indices.size()];
			if (indices.size() > 0)
				memcpy(m_Indices, &indices[0], indices.size() * sizeof(unsigned int));
			Init();
		};
		~Mesh();
	
		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		uint32_t GetIndicesSize()const { return m_IndicesSize; }
		uint32_t GetVerticesSize()const { return m_VerticesSize; }
		std::shared_ptr<Material> GetMaterial()const { return m_Material; }
		//Draw Light
		void Draw(const glm::mat4 & model);
		void Draw(const glm::mat4 & model, const LightSources& lightSources);

	private:
		void Init();
		std::shared_ptr<VertexArray> m_VertexArray;
		VertexBufferLayout           m_VertexBufferLayout;
		float*                       m_Vertices = nullptr;
		unsigned int*                m_Indices = nullptr;
		uint32_t                     m_VerticesSize = 0;
		uint32_t                     m_IndicesSize = 0;
		std::shared_ptr<Material>    m_Material;

	};

}