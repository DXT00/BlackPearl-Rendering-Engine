#pragma once
#include "pch.h"
#include "Texture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace BlackPearl {

	Texture::Texture(
		Type type,
		const std::string &image,
		unsigned int minFilter,
		unsigned int maxFilter,
		int internalFormat,
		int wrap,
		unsigned int dataType,
		bool generateMipmap
	)//TODO::构造函数里头不能有虚函数！
	{
		m_Path = image;
		m_Type = type;
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		Init(image, minFilter, maxFilter, internalFormat,wrap, dataType, generateMipmap);
		

	}
	//Use in ShadowMap (DepthMap) or FrameBuffer's empty ColorMap
	Texture::Texture(
		Type type, 
		const int width, 
		const int height,
		bool isDepth,
		unsigned int minFilter, 
		unsigned int maxFilter,
		int internalFormat, 
		int format,
		int wrap,
		unsigned int dataType,
		bool generateMipmap
		)
	{
		
		m_Path = "";
		m_Type = type;
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		if(isDepth==false)
			Init(width, height,minFilter,maxFilter,internalFormat, format, wrap, dataType, generateMipmap);
		


	}
	//Use in CubeMap
	Texture::Texture(Type type, std::vector<std::string> faces)
	{
		m_Path = "";
		m_FacesPath = faces;
		m_Type = type;
		glGenTextures(1, &m_TextureID);
	
	}

	/*
	dataType=GL_UNSIGNED_BYTE

	wrap = GL_CLAMP_TO_EDGE

	minFilter=GL_LINEAR_MIPMAP_LINEAR
	maxFilter=GL_LINEAR
	
	*/
	void Texture::Init(
		const std::string &image,
		unsigned int minFilter, 
		unsigned int maxFilter,
		int internalFormat,
		int wrap,
		unsigned int dataType, 
		bool generateMipmap)
	{
		//LoadTexture(image, minFilter, maxFilter, internalFormat, format, wrap, dataType);
		GE_ASSERT(image.size() != 0, "texture image is empty!");

		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);


		unsigned char* data = stbi_load(image.c_str(), &width, &height, &nrChannels, 0);
		GE_ASSERT(data, "fail to load texture data!");
		GLenum format;
		switch (nrChannels) //注意不同图片有不同的通道数！
		{
		case 1:
			format = GL_RED;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			GE_CORE_ERROR("Channel {0} has unknown format!", nrChannels)
				break;
		}
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);
		if(generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
		UnBind();
		stbi_image_free(data);
	}

	void Texture::Init(
		const int width, 
		const int height,
		unsigned int minFilter,
		unsigned int maxFilter,
		int internalFormat, 
		int format,
		int wrap,
		unsigned int dataType, 
		bool generateMipmap)
	{
	/*	FBO(
			GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
			GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);*/
		//纹理过滤---邻近过滤和线性过滤
		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);//纹理缩小时用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);//纹理放大时也用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, NULL);
		UnBind();
	}

	

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void Texture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}