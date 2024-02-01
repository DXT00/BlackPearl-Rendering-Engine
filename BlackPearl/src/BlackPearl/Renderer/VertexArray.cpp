#include "pch.h"
#include <glad/glad.h>
#include "VertexArray.h"
#include "Buffer/Buffer.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	static uint32_t ShaderDataTypeToBufferType(const ElementDataType& type) {

		switch (type) {
		case ElementDataType::Int:      return GL_INT;
		case ElementDataType::Int2:     return GL_INT;
		case ElementDataType::Int3:     return GL_INT;
		case ElementDataType::Int4:     return GL_INT;
		case ElementDataType::Float:    return GL_FLOAT;
		case ElementDataType::Float2:   return GL_FLOAT;
		case ElementDataType::Float3:   return GL_FLOAT;
		case ElementDataType::Float4:   return GL_FLOAT;
		case ElementDataType::Mat3:		return GL_FLOAT;
		case ElementDataType::Mat4:		return GL_FLOAT;
		case ElementDataType::False:	return GL_FALSE;
		case ElementDataType::True:		return GL_TRUE;
		}
		GE_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
	}
	VertexArray::VertexArray(bool interleaved, uint32_t target)
	{
		m_IndirectBuffer = nullptr;
		m_IndexBuffer = nullptr;
		m_InterleavedVAO = interleaved;
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
	}

	VertexArray::VertexArray(uint32_t vboNum, bool interleaved, uint32_t target)
	{
		m_IndirectBuffer = nullptr;
		m_IndexBuffer = nullptr;
		m_InterleavedVAO = interleaved;
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
		if (target == GL_ARRAY_BUFFER) {
			m_VertexBuffers.assign(vboNum, nullptr);
		}
		else {

		}
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::UnBind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::SetVertexBuffer(uint32_t location, const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(location < m_VertexBuffers.size(), "location {0} exceed vbos size", location);
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");

		SetAttributes(vertexBuffer);
		m_VertexBuffers[location] = vertexBuffer;	
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");

		glBindVertexArray(m_RendererID);
		SetAttributes(vertexBuffer);
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::UpdateVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(std::find(m_VertexBuffers.begin(), m_VertexBuffers.end(), vertexBuffer) != m_VertexBuffers.end(), "cannot find vertexBuffer");
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");

		glBindVertexArray(m_RendererID);
		SetAttributes(vertexBuffer);
		
	}


	void VertexArray::SetIndirectBuffer(uint32_t location, const std::shared_ptr<IndirectBuffer>& indirectBuffer)
	{
		m_IndirectBuffer = indirectBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, m_IndirectBuffer->GetID());
		glEnableVertexAttribArray(location);
		glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));
		glVertexAttribDivisor(location, 1); //only once per instance
	}

	void VertexArray::SetIndirectBuffer(uint32_t location, uint32_t indirectBufferID)
	{
		glBindBuffer(GL_ARRAY_BUFFER, indirectBufferID);
		glEnableVertexAttribArray(location);
		glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));
		glVertexAttribDivisor(location, 1); //only once per instance

	}

	void VertexArray::UpdateVertexBuffers()
	{
		glBindVertexArray(m_RendererID);

		for (auto vertexBuffer : m_VertexBuffers) {
			if (!vertexBuffer)
				continue;
		//	GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
			//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");
			glBindVertexArray(m_RendererID);
			SetAttributes(vertexBuffer);
		}
	}

	void VertexArray::SetAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		uint32_t stride = 0;
		uint32_t offset = 0;

		if(!vertexBuffer->GetInterleaved())
			GE_ASSERT(layout.GetElements().size() == 1, "element size  != 1 in attribute vbo");
			

		for (BufferElement element : layout.GetElements()) {
			if (vertexBuffer->GetInterleaved()) {
				stride = layout.GetStride();
				offset = element.Offset;
			} 
			if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), stride, (void*)offset);
			else
				glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, stride, (void*)offset);
			glEnableVertexAttribArray(element.Location);
			if (vertexBuffer->GetDivisor()) {
				glVertexAttribDivisor(element.Location, vertexBuffer->GetDivPerInstance());
			}
		}

	}

	void VertexArray::SetNonInterleavedAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{

		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		GE_ASSERT(layout.GetElements().size() == 1, "element size  > 1 in attribute vbo");

		for (BufferElement element : layout.GetElements()) {
			if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), 0, (void*)0);
			else
				glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(element.Location);
			if (vertexBuffer->GetDivisor()) {
				glVertexAttribDivisor(element.Location, vertexBuffer->GetDivPerInstance());
			}
		}


	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
		m_IndexBuffer = indexBuffer;
	}
}