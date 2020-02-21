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

	//GLuint gBuffer;
	//	glGenFramebuffers(1, &gBuffer);
	//	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	//	GLuint gPosition, gNormal, gAlbedoSpec;
	//	// - Position color buffer
	//	glGenTextures(1, &gPosition);
	//	glBindTexture(GL_TEXTURE_2D, gPosition);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	//	// - Normal color buffer
	//	glGenTextures(1, &gNormal);
	//	glBindTexture(GL_TEXTURE_2D, gNormal);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	//	// - Color + Specular color buffer
	//	glGenTextures(1, &gAlbedoSpec);
	//	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	//	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	//	GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	//	glDrawBuffers(3, attachments);
	//	// - Create and attach depth buffer (renderbuffer)
	//	//GLuint rboDepth;
	//	// = rboDepth;
	//	glGenRenderbuffers(1, &m_RenderBufferID);
	//	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
	//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
	//	// - Finally check if framebuffer is complete
	//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	//		std::cout << "Framebuffer not complete!" << std::endl;
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
		//UnBind();
		stbi_image_free(data);
	}

	void Texture::Init(
		const int width, 
		const int height,
		unsigned int minFilter,
		unsigned int maxFilter,
		GLint internalFormat,
		GLenum format,
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
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);//纹理缩小时用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);//纹理放大时也用邻近过滤
		if (wrap != -1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		}


	//	UnBind();
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