#pragma once
#include "pch.h"
#include "Texture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef GE_PLATFORM_ANDRIOD
#include "GLES3/gl32.h"
#endif
namespace BlackPearl {


	Texture::Texture(Type type)
	{
		glGenTextures(1, &m_TextureID);
	}
	Texture::Texture(
		Type type,
		const std::string& image,
		unsigned int minFilter,
		unsigned int maxFilter,
		int internalFormat,
		int wrap,
		unsigned int dataType,
		bool generateMipmap
	)
	{
		m_Path = image;
		m_Type = type;
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		Init(image, minFilter, maxFilter, internalFormat, wrap, dataType, generateMipmap);


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
		bool generateMipmap,
		float* data
	)
	{
		m_Width = width;
		m_Height = height;
		m_Path = "";
		m_Type = type;
		glGenTextures(1, &m_TextureID);
		GE_ERROR_JUDGE();//����error��ԭ�򣺺ܿ���m_TextureID�����˱��target�ϣ�����CUBEMAP,���еĻ�����ǰ�Ӹ��ϵ� = = 

		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		GE_ERROR_JUDGE();
		Init(width, height, minFilter, maxFilter, internalFormat, format, wrap, dataType, generateMipmap, data);
		GE_ERROR_JUDGE();


	}
	//Use in CubeMap
	Texture::Texture(Type type, std::vector<std::string> faces)
	{
		m_Path = "";
		//m_FacesPath = faces;
		m_Type = type;
		glGenTextures(1, &m_TextureID);

	}

	Texture::~Texture()
	{
		UnBind();
		glDeleteTextures(1, &m_TextureID);
	}


	void Texture::Init(
		const std::string& image,
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
		switch (nrChannels) //ע�ⲻͬͼƬ�в�ͬ��ͨ������
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
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
		m_Width = width;
		m_Height = height;
		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);
		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
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
		bool generateMipmap, float* data)
	{
		m_Width = width;
		m_Height = height;
		/*	FBO(
				GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
				GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);*/
				//�������---�ڽ����˺����Թ���
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);//������Сʱ���ڽ�����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);//����Ŵ�ʱҲ���ڽ�����
		if (wrap != -1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		}

		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		//	UnBind();
	}

	void Texture::SetSizeFilter(GLenum min_filter, GLenum mag_filter) {
#ifdef GE_PLATFORM_WINDOWS

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, min_filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, mag_filter);
#endif


	}
	void Texture::SetWrapFilter(GLenum filter) {
#ifdef GE_PLATFORM_WINDOWS

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, filter);
#endif
	}

	void Texture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void Texture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void Texture::Storage(GLsizei width, GLsizei height, GLenum internal_format, GLsizei levels)
	{
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
#ifdef GE_PLATFORM_WINDOWS

		glTextureStorage2D(m_TextureID, levels, internal_format, width, height);
#endif
#ifdef GE_PLATFORM_ANDRIOD

        glTexStorage2D(m_TextureID, levels, internal_format, width, height);
#endif

    }
}