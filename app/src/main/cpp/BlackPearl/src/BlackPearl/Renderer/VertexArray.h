#pragma once
#include"BlackPearl/Renderer/Buffer/Buffer.h"
namespace BlackPearl {

	class VertexArray
	{
	public:
		VertexArray(bool interleaved = true, uint32_t target = GL_ARRAY_BUFFER);
		VertexArray(uint32_t vboNum, bool interleaved, uint32_t target = GL_ARRAY_BUFFER);
		~VertexArray() = default;
		void Bind();
		void UnBind();

		void SetVertexBuffer(uint32_t location, const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void UpdateVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void UpdateVertexBuffers();

		void SetAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer);
		void SetNonInterleavedAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer);


		void SetIndirectBuffer(uint32_t location, const std::shared_ptr<IndirectBuffer>& indirectBuffer);
		void SetIndirectBuffer(uint32_t location, uint32_t indirectBufferID);

		void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);
		std::shared_ptr<IndexBuffer> GetIndexBuffer()const { return m_IndexBuffer; }
		std::shared_ptr<IndirectBuffer> GetIndirectBuffer() const { return m_IndirectBuffer; }
		std::vector<std::shared_ptr<VertexBuffer>> GetVertexBuffers() const { return m_VertexBuffers; }

	private:
		unsigned int m_RendererID;

	
		std::vector<std::shared_ptr<VertexBuffer>> m_VertexBuffers;

		//interleaved m_VertexBuffers format like:
		//(xyzrgb0,xyzrgb1...)

		// non interleaved m_VertexBuffers format like :
		// stride and offset of attribute vbo is 0, this is a tightly packed attribute array which each attribute a VBO.
		// m_AttributeVertexBuffers format like : 
		// vb0:(xyzxyz...)
		// vb1:(rgbrgb...)
		// vb2:(stst....)
		bool m_InterleavedVAO;
		std::shared_ptr<IndexBuffer> m_IndexBuffer;
		std::shared_ptr<IndirectBuffer> m_IndirectBuffer;

	};

}