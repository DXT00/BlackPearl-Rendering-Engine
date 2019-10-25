#pragma once
#include<vector>
#include"BlackPearl/Renderer/Texture/Texture.h"
#include"BlackPearl/Renderer/Shader.h"
#include"BlackPearl/Renderer/Buffer.h"
#include"BlackPearl/Renderer/LightComponent/LightSources.h"
#include"BlackPearl/Renderer/Texture/MaterialColor.h"
//#include "BlackPearl/Renderer/VertexArray.h"
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
		//Mesh(
		//	std::vector<float> vertices,
		//	std::vector<unsigned int>indices,
		//	const std::vector<std::shared_ptr<Texture>>& textures,
		//	const VertexBufferLayout& layout
		//)
		//	:m_Vertices(vertices), m_Indices(indices),
		//	m_Textures(textures),m_VertexBufferLayout(layout){
		//	Init();
		//};
		Mesh(
			float* vertices,
			uint32_t verticesSize,
			unsigned int* indices,
			uint32_t indicesSize,
			const std::vector<std::shared_ptr<Texture>>& textures,
			const std::vector<std::shared_ptr<MaterialColor>>& colors,

			const VertexBufferLayout& layout
		)
			:m_Vertices(vertices), m_VerticesSize(verticesSize), m_Indices(indices), m_IndicesSize(indicesSize),
			m_Textures(textures), m_MaterialColors(colors), m_VertexBufferLayout(layout) {
			Init();
		};
		~Mesh();
		void Draw(const std::shared_ptr<Shader>& shader, const glm::mat4 & model, const LightSources& lightSources);
		std::shared_ptr<VertexArray> GetVertexArray() const { return m_VertexArray; }
		uint32_t GetIndicesSize()const { return m_IndicesSize; }
		uint32_t GetVerticesSize()const { return m_VerticesSize; }

	private:
		void Init();
		std::shared_ptr<VertexArray> m_VertexArray;
		//std::shared_ptr<VertexBuffer>  m_VertexBuffer;
		//std::shared_ptr<IndexBuffer> m_IndexBuffer;
		VertexBufferLayout m_VertexBufferLayout;

		//std::vector<float> m_Vertices;
		//std::vector<unsigned int> m_Indices;
		float*m_Vertices;
		unsigned int*m_Indices;
		uint32_t m_VerticesSize;
		uint32_t m_IndicesSize;
		std::vector<std::shared_ptr<Texture> >m_Textures;
		std::vector<std::shared_ptr<MaterialColor>> m_MaterialColors;


	};

}