#include "pch.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace BlackPearl {

	Texture::Texture(Type type)
		:TextureStateExtension(desc)
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
	) : TextureStateExtension(desc)
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
	) :TextureStateExtension(desc)
	{
		m_Width = width;
		m_Height = height;
		m_Path = "";
		m_Type = type;
		glGenTextures(1, &m_TextureID);
		GE_ERROR_JUDGE();//出现error的原因：很可能m_TextureID用在了别的target上，例如CUBEMAP,不行的话运行前加个断点 = = 

		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		GE_ERROR_JUDGE();
		Init(width, height, minFilter, maxFilter, internalFormat, format, wrap, dataType, generateMipmap, data);
		GE_ERROR_JUDGE();


	}
	//Use in CubeMap
	Texture::Texture(Type type, std::vector<std::string> faces)
		:TextureStateExtension(desc) {
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
		switch (nrChannels) //注意不同图片有不同的通道数！
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
		bool generateMipmap, float* data)
	{
		m_Width = width;
		m_Height = height;
		/*	FBO(
				GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
				GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);*/
				//纹理过滤---邻近过滤和线性过滤

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, dataType, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);//纹理缩小时用邻近过滤
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);//纹理放大时也用邻近过滤
		if (wrap != -1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		}

		if (generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理
		//	UnBind();
	}

	void Texture::SetSizeFilter(GLenum min_filter, GLenum mag_filter) {

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, min_filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, mag_filter);
	}
	void Texture::SetWrapFilter(GLenum filter) {

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, filter);
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

		glTextureStorage2D(m_TextureID, levels, internal_format, width, height);
	}

}