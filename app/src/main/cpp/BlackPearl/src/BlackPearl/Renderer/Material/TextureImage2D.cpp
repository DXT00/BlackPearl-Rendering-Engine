#include "pch.h"
#ifdef GE_PLATFORM_ANDRIOD
#include "GLES3/gl32.h"
#endif
#ifdef GE_PLATFORM_WINDOWS
#include "glad/glad.h"
#endif
#include "TextureImage2D.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {




	TextureImage2D::TextureImage2D(
		const std::vector<float>& textureBuffer,
		const int width,
		const int height,
		unsigned int minFilter,
		unsigned int maxFilter,
		int internalFormat,
		int format,
		int wrap,
		unsigned int dataType,
		unsigned int access
		):Texture(Texture::Type::ImageMap)
	{
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	/*	glActiveTexture(GL_TEXTURE0 + textureID);
		glBindImageTexture( textureID, m_RendererID, 0, GL_FALSE, 0, m_Acess, m_InternalFormat);*/
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);//������Сʱ���ڽ�����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);//����Ŵ�ʱҲ���ڽ�����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		if(textureBuffer.empty())
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, NULL);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, &textureBuffer[0]);
		m_InternalFormat = internalFormat;
		m_Format = format;
		m_DataType = dataType;
		m_Acess = access;
		m_Width = width;
		m_Height = height;
		ShowProperties();
		m_Initial = true;
	}

	TextureImage2D::~TextureImage2D()
	{
		glDeleteTextures(1, &m_RendererID);	
	}

	void TextureImage2D::Bind(unsigned int textureID)
	{
		glActiveTexture(GL_TEXTURE0 + textureID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glBindImageTexture(textureID, m_RendererID, 0, GL_FALSE, 0, m_Acess, m_InternalFormat);

	}

	void TextureImage2D::BindImage(unsigned int textureID)
	{
		glBindImageTexture(textureID, m_RendererID, 0, GL_FALSE, 0, m_Acess, m_InternalFormat);
	}

	void TextureImage2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void TextureImage2D::Clear(float clearColor[4])
	{
		GE_ASSERT(m_Initial,"please Initial first!")
		//GLint previousBoundTextureID;
	//	glGetIntegerv(GL_TEXTURE_BINDING_2D, &previousBoundTextureID);
		//glBindTexture(GL_TEXTURE_2D, m_RendererID);
		//glClearTexImage(m_RendererID, 0, GL_RGBA, GL_FLOAT, &clearColor);
		//glBindTexture(GL_TEXTURE_2D, previousBoundTextureID);
		unsigned int fbo;
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_RendererID, 0); //Only need to do this once.
#ifdef GE_PLATFORM_WINDOWS
        glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
#endif
		glClearBufferfv(GL_COLOR, 0, clearColor);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	}

	void TextureImage2D::ShowProperties()
	{
#ifdef GE_PLATFORM_WINDOWS
		/*check what the maximum size of the total work group that we give to glDispatchCompute() is.
		We can get the x, y, and z extents of this*/
		int workGroupCnt[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCnt[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCnt[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCnt[2]);

		GE_CORE_INFO("max global (total) work group size x:{0} y:{1} z:{2}\n",
			workGroupCnt[0], workGroupCnt[1], workGroupCnt[2]);

		/* We can also check the maximum size of a local work group (sub-division of the total number of jobs).
		This is defined in the compute shader itself*/
		int workGroupSize[3];
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);

		GE_CORE_INFO("max local (in one shader) work group sizes x:{0} y:{1} z:{2}\n",
			workGroupSize[0], workGroupSize[1], workGroupSize[2]);
#endif
	}

}