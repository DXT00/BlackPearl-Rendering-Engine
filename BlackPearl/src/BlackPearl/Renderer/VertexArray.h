#pragma once
#include"BlackPearl/Renderer/Buffer/Buffer.h"
namespace BlackPearl {

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray() = default;
		void Bind();
		void UnBind();

		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void AddAttributeVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);

		void UpdateVertexBuffer();

		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
		std::shared_ptr<IndexBuffer> GetIndexBuffer()const { return m_IndexBuffer; }
		std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const { return m_VertexBuffers; }

	private:
		unsigned int m_RendererID;

		//m_VertexBuffers format like:
		//(xyzrgb0,xyzrgb1...)
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;


		// stride and offset of attribute vbo is 0, this is a tightly packed attribute array which each attribute a VBO.
		// m_AttributeVertexBuffers format like : 
		// vb0:(xyzxyz...)
		// vb1:(rgbrgb...)
		// vb2:(stst....)
		std::vector<std::shared_ptr<VertexBuffer>> m_AttributeVertexBuffers;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
	};

}