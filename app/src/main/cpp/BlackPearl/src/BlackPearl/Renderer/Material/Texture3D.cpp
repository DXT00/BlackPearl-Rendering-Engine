#include "pch.h"
#include "Renderer/Material/Texture3D.h"
#include "BlackPearl/Core.h"
//#include "glad/glad.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"

namespace BlackPearl {

	Texture3D::Texture3D(const std::vector<float>& textureBuffer, const int width, const int height, const int depth, const bool generateMipmaps)
		:m_Width(width),m_Height(height),m_Depth(depth)//,m_ClearData(4*width*height*depth,0.0f)
	{
		//Generate texture on GPU
		glGenTextures(1, &m_TextureID);
		GE_ERROR_JUDGE();
		glBindTexture(GL_TEXTURE_3D, m_TextureID);
		GE_ERROR_JUDGE();
		// Parameter options.
		const auto wrap = GL_CLAMP_TO_BORDER;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);
		GE_ERROR_JUDGE();

		const auto filter = GL_LINEAR_MIPMAP_LINEAR;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		GE_ERROR_JUDGE();

		//Upload texture buffer
		glTexStorage3D(GL_TEXTURE_3D, m_MipLevel, GL_RGBA8, m_Width, m_Height, m_Depth);
		GE_ERROR_JUDGE();

		glTexSubImage3D(GL_TEXTURE_3D, 0,0,0,0, m_Width, m_Height, m_Depth,  GL_RGBA, GL_FLOAT, &textureBuffer[0]);//Upload level0
		GE_ERROR_JUDGE();

		if (generateMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
		GE_ERROR_JUDGE();

		glBindTexture(GL_TEXTURE_3D, 0);
		GE_ERROR_JUDGE();

	}
	
	Texture3D::~Texture3D()
	{
		glDeleteTextures(1, &m_TextureID);
	}

	void Texture3D::Clear(float clearColor[4])
	{
		GLint previousBoundTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousBoundTextureID);
		glBindTexture(GL_TEXTURE_3D, m_TextureID);
		//gl4.4 �汾����, һ��������mipmap
		// 	glClearTexImage(m_TextureID, 0, GL_RGBA, GL_FLOAT, &clearColor);
		// 
		// 
		//�ĳ�compute shader
		//Upload texture buffer
		glTexStorage3D(GL_TEXTURE_3D, m_MipLevel, GL_RGBA8, m_Width, m_Height, m_Depth);
		GE_ERROR_JUDGE();

		std::vector<GLubyte> clearData(m_Width*m_Height*m_Depth * 4);
		for (size_t i = 0; i < clearData.size(); i += 4) {
			memcpy(&clearData[i], clearColor, 4);
		}


		// �ϴ����ݵ�����
		glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 64, 64, 64, GL_RGBA, GL_UNSIGNED_BYTE, clearData.data());
		glBindTexture(GL_TEXTURE_3D, previousBoundTextureID);
	}
	//void Texture3D::Clear(GLuint clearColor[4])
	//{
	//	GLint previousBoundTextureID;
	//	glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousBoundTextureID);
	//	glBindTexture(GL_TEXTURE_3D, m_TextureID);
	//	glClearTexImage(m_TextureID, 0, GL_R, GL_UNSIGNED_BYTE, &clearColor);
	//	glBindTexture(GL_TEXTURE_3D, previousBoundTextureID);
	//}
	void Texture3D::Bind()
	{
		glBindTexture(GL_TEXTURE_3D, m_TextureID);
	}

	void Texture3D::UnBind()
	{
		glBindTexture(GL_TEXTURE_3D, 0);
	}

}
