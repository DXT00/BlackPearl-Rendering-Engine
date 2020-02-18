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
	}

	void Mesh::Init(uint32_t verticesSize)
	{

		m_VertexArray.reset(DBG_NEW VertexArray());

		std::shared_ptr<VertexBuffer> vertexBuffer;
		//vertexBuffer.reset(new VertexBuffer(m_Vertices));
		vertexBuffer.reset(DBG_NEW VertexBuffer(m_Vertices, verticesSize));

		if (m_IndicesSize != 0) {
			std::shared_ptr<IndexBuffer> indexBuffer;
			//indexBuffer.reset(new IndexBuffer(m_Indices));
			indexBuffer.reset(DBG_NEW IndexBuffer(m_Indices, m_IndicesSize));
			m_VertexArray->SetIndexBuffer(indexBuffer);
		}


		vertexBuffer->SetBufferLayout(m_VertexBufferLayout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

	}
}