#pragma once
#include"BlackPearl/Renderer/Buffer.h"
namespace BlackPearl {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray() = default;
		void Bind();
		void UnBind();

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void UpdateVertexBuffer();

		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
		std::shared_ptr<IndexBuffer> GetIndexBuffer()const { return m_IndexBuffer; }
		std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const { return m_VertexBuffers; }

	private:
		unsigned int m_RendererID;
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}