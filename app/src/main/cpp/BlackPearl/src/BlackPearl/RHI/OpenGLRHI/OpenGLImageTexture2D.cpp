#include "pch.h"
#include <glad/glad.h>
#include "OpenGLImageTexture2D.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {

	//What is the difference between glBindImageTexture() and glBindTexture()
	//https://stackoverflow.com/questions/37136813/what-is-the-difference-between-glbindimagetexture-and-glbindtexture

	ImageTexture2D::ImageTexture2D(
		TextureDesc& desc,
		float* textureBuffer
	) :Texture(desc)
	{

	}

	ImageTexture2D::~ImageTexture2D()
	{
	}

	void ImageTexture2D::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);

	}

	void ImageTexture2D::Init(TextureDesc& desc, float* data)
	{
		fillTextureInfo(desc);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);//纹理缩小时用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);//纹理放大时也用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap);

		if (!data)
			glTexImage2D(GL_TEXTURE_2D, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, NULL);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, &data);
		m_Acess = _ConvertAccess(desc.isUAV);

		ShowProperties();
		m_Initial = true;
	}

	void ImageTexture2D::Bind(unsigned int textureID)
	{
		glActiveTexture(GL_TEXTURE0 + textureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glBindImageTexture(textureID, m_TextureID, 0, GL_FALSE, 0, m_Acess, m_InnerFormat);

	}

	void ImageTexture2D::BindImage(unsigned int textureID)
	{
		glBindImageTexture(textureID, m_TextureID, 0, GL_FALSE, 0, m_Acess, m_InnerFormat);
	}

	void ImageTexture2D::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void ImageTexture2D::Clear(float clearColor[4])
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
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_TextureID, 0); //Only need to do this once.
		glDrawBuffer(GL_COLOR_ATTACHMENT0); //Only need to do this once.
		glClearBufferfv(GL_COLOR, 0, clearColor);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	}

	void ImageTexture2D::ShowProperties()
	{
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
	}

	GLint ImageTexture2D::_ConvertAccess(bool isUAV)
	{

		if (isUAV) {
			return GL_READ_WRITE;
		}
		else {
			return GL_READ_ONLY;
		}

	}

}