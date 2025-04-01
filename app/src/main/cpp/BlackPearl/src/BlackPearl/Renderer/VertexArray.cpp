#include "pch.h"
//#include <glad/glad.h>
#include "VertexArray.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLThirdParty.h"
//#include "Buffer/Buffer.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLBuffer.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLBufferResource.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLInputLayout.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLUtil.h"
namespace BlackPearl {

	VertexArray::VertexArray(bool interleaved, uint32_t target)
	{
		m_IndirectBuffer = nullptr;
		m_IndexBuffer = nullptr;
		m_InterleavedVAO = interleaved;
#ifdef GE_API_OPENGL
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
#endif
	}

	VertexArray::VertexArray(uint32_t vboNum, bool interleaved, uint32_t target)
	{
		m_IndirectBuffer = nullptr;
		m_IndexBuffer = nullptr;
		m_InterleavedVAO = interleaved;
#ifdef GE_API_OPENGL
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
#endif
		if (target == GL_ARRAY_BUFFER) {
			m_VertexBuffers.assign(vboNum, nullptr);
		}
		else {

		}
	}

	void VertexArray::Bind()
	{
#ifdef GE_API_OPENGL
		glBindVertexArray(m_RendererID);
#endif
	}

	void VertexArray::UnBind()
	{
#ifdef GE_API_OPENGL
		glBindVertexArray(0);
#endif
	}

	void VertexArray::SetVertexBuffer(uint32_t location, const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(location < m_VertexBuffers.size(), "location {0} exceed vbos size", location);
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");
#ifdef GE_API_OPENGL
		SetAttributes(vertexBuffer);
#endif
		m_VertexBuffers[location] = vertexBuffer;	
	}

	void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");
#ifdef GE_API_OPENGL
		glBindVertexArray(m_RendererID);
		SetAttributes(vertexBuffer);
#endif
		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::UpdateVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		GE_ASSERT(std::find(m_VertexBuffers.begin(), m_VertexBuffers.end(), vertexBuffer) != m_VertexBuffers.end(), "cannot find vertexBuffer");
		//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");
#ifdef GE_API_OPENGL
		glBindVertexArray(m_RendererID);
		SetAttributes(vertexBuffer);
#endif
		
	}


	void VertexArray::SetIndirectBuffer(uint32_t location, const std::shared_ptr<IndirectBuffer>& indirectBuffer)
	{
		m_IndirectBuffer = indirectBuffer;
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, m_IndirectBuffer->GetID());
		glEnableVertexAttribArray(location);
		glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));
		glVertexAttribDivisor(location, 1); //only once per instance
#endif
	}

	void VertexArray::SetIndirectBuffer(uint32_t location, uint32_t indirectBufferID)
	{
#ifdef GE_API_OPENGL
		glBindBuffer(GL_ARRAY_BUFFER, indirectBufferID);
		glEnableVertexAttribArray(location);
		glVertexAttribIPointer(location, 1, GL_UNSIGNED_INT, sizeof(IndirectCommand), (void*)(offsetof(IndirectCommand, startInstance)));
		glVertexAttribDivisor(location, 1); //only once per instance
#endif
	}

	void VertexArray::UpdateVertexBuffers()
	{
#ifdef GE_API_OPENGL
		glBindVertexArray(m_RendererID);

		for (auto vertexBuffer : m_VertexBuffers) {
			if (!vertexBuffer)
				continue;
		//	GE_ASSERT(vertexBuffer->GetBufferLayout().GetElements().size(), "Vertex Buffer has no layout!!");
			//GE_ASSERT(m_InterleavedVAO == vertexBuffer->GetInterleaved(), "vbo interleave attribute is different from vao");
			glBindVertexArray(m_RendererID);
			SetAttributes(vertexBuffer);
		}
#endif
	}

	void VertexArray::SetAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
#ifdef GE_API_OPENGL
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
			if (OpenGLUtil::convertInputElementDataType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), OpenGLUtil::convertInputElementDataType(element.Type), stride, (void*)offset);
			else
				glVertexAttribPointer(element.Location, element.GetElementCount(), OpenGLUtil::convertInputElementDataType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, stride, (void*)offset);
			glEnableVertexAttribArray(element.Location);
			if (vertexBuffer->GetDivisor()) {
				glVertexAttribDivisor(element.Location, vertexBuffer->GetDivPerInstance());
			}
		}
#endif
	}

	void VertexArray::SetNonInterleavedAttributes(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
#ifdef GE_API_OPENGL

		vertexBuffer->Bind();
		auto layout = vertexBuffer->GetBufferLayout();
		GE_ASSERT(layout.GetElements().size() == 1, "element size  > 1 in attribute vbo");

		for (BufferElement element : layout.GetElements()) {
			if (OpenGLUtil::convertInputElementDataType(element.Type) == GL_INT)
				glVertexAttribIPointer(element.Location, element.GetElementCount(), OpenGLUtil::convertInputElementDataType(element.Type), 0, (void*)0);
			else
				glVertexAttribPointer(element.Location, element.GetElementCount(), OpenGLUtil::convertInputElementDataType(element.Type), element.Normalized == true ? GL_TRUE : GL_FALSE, 0, (void*)0);
			glEnableVertexAttribArray(element.Location);
			if (vertexBuffer->GetDivisor()) {
				glVertexAttribDivisor(element.Location, vertexBuffer->GetDivPerInstance());
			}
		}
#endif

	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
#ifdef GE_API_OPENGL
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();
#endif
		m_IndexBuffer = indexBuffer;
	}
}