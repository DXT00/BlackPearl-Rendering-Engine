#pragma once
#include "pch.h"
#include <glad/glad.h>
#include "OpenGLCubeMapTexture.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace BlackPearl {
	


	CubeMapTexture::~CubeMapTexture()
	{

	}
	void CubeMapTexture::LoadCubeMap()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		for (unsigned int i = 0; i < 6; i++)
		{
	
			if (!desc.faces.empty())
			{
				int width, height, nrChannels;
				unsigned char* data = stbi_load(desc.faces[i].c_str(), &width, &height, &nrChannels, 0);
				m_Width = width;
				m_Height = height;
				//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, data);

				stbi_image_free(data);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, NULL);

				//std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				//stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_MinFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_MagFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Wrap);

		if (desc.generateMipmap)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}
	//void CubeMapTexture::LoadCubeMap(const int width, const int height, unsigned int minFilter, unsigned int maxFilter, int wrap,int internalFormat,int format, int dataType,bool generateMipmap)
	//{
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
	//	for (unsigned int i = 0; i < 6; i++)
	//		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, dataType, NULL);

	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, m_MinFilter);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, m_MagFilter);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, m_Wrap);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, m_Wrap);
	//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, m_Wrap);

	//	if(generateMipmap)
	//		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	//}
	void CubeMapTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	}
	void CubeMapTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}
}

