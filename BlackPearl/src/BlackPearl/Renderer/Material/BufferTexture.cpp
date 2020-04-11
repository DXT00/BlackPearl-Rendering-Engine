#include "pch.h"
#include "BufferTexture.h"
#include "glad/glad.h"
namespace BlackPearl {

	BufferTexture::BufferTexture(unsigned int bufferSize, int internalFormat,const void* data)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_TEXTURE_BUFFER, m_RendererID);
		glBufferData(GL_TEXTURE_BUFFER, bufferSize, data, GL_STATIC_DRAW);

		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_BUFFER, m_TextureID);
		glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, m_RendererID);


		glBindBuffer(GL_TEXTURE_BUFFER, 0);


	}
	BufferTexture::~BufferTexture()
	{
		glDeleteTextures(1, &m_TextureID);
		glDeleteBuffers(1, &m_RendererID);
		
	}
	void BufferTexture::Bind()
	{
		glBindBuffer(GL_TEXTURE_BUFFER, m_RendererID);

	}
	void BufferTexture::Unbind()
	{
		glBindBuffer(GL_TEXTURE_BUFFER, 0);

	}
	void BufferTexture::BindTexture()
	{
		glBindTexture(GL_TEXTURE_BUFFER, m_TextureID);

	}
	void BufferTexture::UnbindTexture()
	{
		glBindTexture(GL_TEXTURE_BUFFER, 0);

	}
}