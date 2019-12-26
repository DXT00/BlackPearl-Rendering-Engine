#include "pch.h"
#include "Buffer.h"
#include "glad/glad.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Renderer/Material/DepthTexture.h"
namespace BlackPearl {
	//------------------------VertexBuffer-----------------//
	VertexBuffer::VertexBuffer(const std::vector<float>&vertices)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	}
	VertexBuffer::VertexBuffer(float*vertices, uint32_t size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	}
	IndexBuffer::IndexBuffer(unsigned int * indices, uint32_t size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);

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


	//------------------------FrameBuffer-----------------//

	FrameBuffer::FrameBuffer(const int width,int height,std::initializer_list<Attachment> attachment, bool disableColor, Texture::Type colorTextureType)
	{
		m_Width = width;
		m_Height = height;

		GLint previousFrameBuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);//获取之前绑定的Framebuffer

		glGenFramebuffers(1, &m_RendererID);
		Bind();
		if (disableColor) {
			DisableColorBuffer();
		}
		for (Attachment attach:attachment)
		{
			if (attach == Attachment::ColorTexture)
				AttachColorTexture(colorTextureType);
			else if (attach == Attachment::DepthTexture)
				AttachDepthTexture();
			else if (attach == Attachment::CubeMapDepthTexture)
				AttachCubeMapDepthTexture();
			else if (attach == Attachment::CubeMapColorTexture)
				AttachCubeMapColorTexture();
			else if (attach == Attachment::RenderBuffer)
				AttachRenderBuffer();

		}
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			GE_CORE_ERROR("Framebuffer is not complete!");
		UnBind();
		//绑定回原来的FrameBuffer
		glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);

	}

	void FrameBuffer::AttachColorTexture(Texture::Type textureType)
	{
		// create a color attachment texture
		// The texture we're going to render to
		m_TextureColorBuffer.reset(DBG_NEW Texture(textureType, m_Width,m_Height,false, GL_NEAREST, GL_NEAREST, GL_RGB16F, GL_RGBA, GL_REPEAT,GL_FLOAT));
		m_TextureColorBuffer->UnBind();
		//将它附加到当前绑定的帧缓冲对象
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorBuffer->GetRendererID(), 0);
	}

	void FrameBuffer::AttachDepthTexture()
	{	
		m_TextureDepthBuffer.reset(DBG_NEW BlackPearl::DepthTexture(Texture::Type::DepthMap, m_Width, m_Height));
	//	m_TextureDepthBuffer->UnBind();
		Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_TextureDepthBuffer->GetRendererID(), 0);

	}

	void FrameBuffer::AttachCubeMapDepthTexture()
	{
		m_CubeMapDepthBuffer.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_Width, m_Height, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT));
		Bind();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_CubeMapDepthBuffer->GetRendererID(), 0);
	}

	void FrameBuffer::AttachCubeMapColorTexture() {
		m_CubeMapColorBuffer.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_Width, m_Height, GL_RGB16F, GL_RGB, GL_FLOAT));
		Bind();

	}
	void FrameBuffer::AttachRenderBuffer()
	{
		//  create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
		//	unsigned int renderBuffer;
		// The depth buffer
		glGenRenderbuffers(1, &m_RenderBufferID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);

		// Use a single rbo for both depth and stencil buffer.
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);

	}

	void FrameBuffer::DisableColorBuffer()
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	void FrameBuffer::Bind()
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		//glViewport(0, 0, width, height);

	}
	void FrameBuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);

	}
	void FrameBuffer::BindColorTexture()
	{
		m_TextureColorBuffer->Bind();

	}
	void FrameBuffer::UnBindTexture()
	{
		m_TextureColorBuffer->UnBind();

	}
	void FrameBuffer::CleanUp()
	{
	/*	glDeleteFramebuffers(1,m_RendererID);*/

	}
}