#include "pch.h"
#include "Buffer.h"
#include "glad/glad.h"

VertexBuffer::VertexBuffer(float* vertices,uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size ,vertices, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() {

	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

}

void VertexBuffer::UnBind() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
//------------------------IndexBuffer-----------------//
IndexBuffer::IndexBuffer(float* indices, uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size,indices, GL_STATIC_DRAW);

}

void IndexBuffer::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}
//---------------------VertexBufferLayout----------------//
void VertexBufferLayout::CalculateStrideAndOffset()
{
	uint32_t strides = 0;
	for (BufferElement &element : m_Elememts) {
		element.Offset = strides;
		strides += element.ElementSize;
	}
	m_Stride = strides;
}
