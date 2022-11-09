#include "pch.h"
#include "VertexArray.h"
#include <glad/glad.h>
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
	VertexArray::VertexArray()
	{
		m_IndirectBuffer = nullptr;
		m_IndexBuffer = nullptr;
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::Bind()
	{
		glBindVertexArray(m_RendererID);
	}

	void VertexArray::UnBind()
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, bool divisor, uint32_t perInstance)
	{
		GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		for (BufferElement element : layout.GetElements()) {
			if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), layout.GetStride(), (void*)element.Offset);
			else 
				glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, layout.GetStride(), (void*)element.Offset);
			glEnableVertexAttribArray(element.Location);
			if (divisor) {
				glVertexAttribDivisor(element.Location, perInstance);
			}
		}
		
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::AddAttributeVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		GE_ASSERT(layout.GetElements().size() == 1, "element size  > 1 in attribute vbo");

		for (BufferElement element : layout.GetElements()) {
			if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), 0, (void*)0);
			else //GL_FLOAT
				glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(element.Location);
			
		}
		m_AttributeVertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::UpdateVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer, bool divisor, uint32_t perInstance)
	{
		GE_ASSERT(std::find(m_VertexBuffers.begin(), m_VertexBuffers.end(), vertexBuffer) != m_VertexBuffers.end(), "cannot find vertexBuffer");
		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		for (BufferElement element : layout.GetElements()) {
			if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), layout.GetStride(), (void*)element.Offset);
			else
				glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, layout.GetStride(), (void*)element.Offset);
			glEnableVertexAttribArray(element.Location);
			if (divisor) {
				glVertexAttribDivisor(element.Location, perInstance);
			}
		}


	}


	void VertexArray::SetIndirectBuffer(uint32_t location, const std::shared_ptr<IndirectBuffer>& indirectBuffer)
	{
		m_IndirectBuffer = indirectBuffer;

		glBindBuffer(GL_ARRAY_BUFFER, m_IndirectBuffer->GetID());
		glEnableVertexAttribArray(location);

		glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));

		glVertexAttribDivisor(location, 1); //only once per instance
	}

	void VertexArray::UpdateVertexBuffers()
	{
		glBindVertexArray(m_RendererID);

		for (auto vertexBuffer : m_VertexBuffers) {
			GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
			vertexBuffer->Bind();
			auto layout = vertexBuffer->GetBufferLayout();
			for (BufferElement element : layout.GetElements()) {
				if (ShaderDataTypeToBufferType(element.Type) == GL_INT)
					glVertexAttribIPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), layout.GetStride(), (void*)element.Offset);
				else //GL_FLOAT
					glVertexAttribPointer(element.Location, element.GetElementCount(), ShaderDataTypeToBufferType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, layout.GetStride(), (void*)element.Offset);
				glEnableVertexAttribArray(element.Location);
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