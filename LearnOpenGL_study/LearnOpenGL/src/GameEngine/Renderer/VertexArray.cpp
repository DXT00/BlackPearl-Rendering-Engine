#include "pch.h"
#include "VertexArray.h"
#include <glad/glad.h>
#include "Buffer.h"
VertexArray::VertexArray()
{
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

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
	GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();
	uint32_t index = 0;
	auto layout = vertexBuffer->GetBufferLayout();
	for (BufferElement element : layout.GetElements()) {
		glVertexAttribPointer(index, element.GetElementCount(),element.GetType(),element.Normalized == true ? GL_TRUE : GL_FALSE, layout.GetStride(), (void*)element.Offset);
		glEnableVertexAttribArray(index);
		index++;
	}
	m_VertexBuffers.push_back(vertexBuffer);
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();
	m_IndexBuffer = indexBuffer;
}
