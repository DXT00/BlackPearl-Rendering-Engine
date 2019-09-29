#include "pch.h"
#include "Buffer.h"
#include "glad/glad.h"

VertexBuffer::VertexBuffer(const std::vector<float>&vertices)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float) ,&vertices[0], GL_STATIC_DRAW);
}
VertexBuffer::VertexBuffer(float*vertices,uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ARRAY_BUFFER, size,vertices, GL_STATIC_DRAW);
}
void VertexBuffer::Bind() {

	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);

}

void VertexBuffer::UnBind() {

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
//------------------------IndexBuffer-----------------//
IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int),&indices[0], GL_STATIC_DRAW);

}
IndexBuffer::IndexBuffer(unsigned int * indices, uint32_t size)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size , indices, GL_STATIC_DRAW);

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
