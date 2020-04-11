#pragma once
#include "pch.h"
#include "CubeMapTexture.h"
#include <glad/glad.h>




//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace BlackPearl {
	


	CubeMapTexture::~CubeMapTexture()
	{

	}
	void CubeMapTexture::LoadCubeMap(std::vector<std::string> faces, unsigned int minFilter,unsigned int maxFilter, int wrap, int internalFormat, int format, int dataType,bool generateMipmap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			m_Width = width;
			m_Height = height;
			if (data)
			{
				//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, dataType, data);

				stbi_image_free(data);
			}
			else
			{
				std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
				stbi_image_free(data);
			}
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, maxFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


		
	}
	void CubeMapTexture::LoadCubeMap(const int width, const int height, unsigned int minFilter, unsigned int maxFilter, int wrap,int internalFormat,int format, int dataType,bool generateMipmap)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);
		for (unsigned int i = 0; i < 6; i++)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, dataType, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, maxFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrap);

		if(generateMipmap)
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	}
	void CubeMapTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureID);

	}
	void CubeMapTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	}
}

