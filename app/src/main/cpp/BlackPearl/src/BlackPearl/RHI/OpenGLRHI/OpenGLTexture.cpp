#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "RHI/OpenGLRHI/OpenGLSampler.h"
#include "BlackPearl/RHI/Common/stb_util.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
namespace BlackPearl {

	//Texture::Texture(const TextureDesc& desc)
	//	:TextureStateExtension(desc)
	//{
	//	glGenTextures(1, &m_TextureID);
	//}

	/** Caching it here, to avoid getting it every time we create a texture. 0 is no multisampling. */
	GLint GMaxOpenGLColorSamples = 0;
	GLint GMaxOpenGLDepthSamples = 0;
	GLint GMaxOpenGLIntegerSamples = 0;

	// in bytes, never change after RHI, needed to scale game features
	int64_t GOpenGLDedicatedVideoMemory = 0;
	// In bytes. Never changed after RHI init. Our estimate of the amount of memory that we can use for graphics resources in total.
	int64_t GOpenGLTotalGraphicsMemory = 0;

	/*
	internalforamt指的是纹理数据在OpenGL中是如何表示的，如GL_RGB就表示纹理的像素在OpenGL里面以红绿蓝三个分量表示，

	format指的是载入纹理的格式，它告诉OpenGL外部数据是如何存储每个像素数据的。
	
	*/
	Texture::Texture(
		const TextureDesc& _desc,
		float* data
	) : desc(_desc),
		TextureStateExtension(_desc)
	{
		
		glGenTextures(1, &m_TextureID);
		GE_ERROR_JUDGE();//出现error的原因：很可能m_TextureID用在了别的target上，例如CUBEMAP,不行的话运行前加个断点 = = 

	

	}
	
	Texture::~Texture()
	{
		UnBind();
		glDeleteTextures(1, &m_TextureID);
	}


	void Texture::Init(
		const TextureDesc& _desc, float* data)
	{
		Bind();
		fillTextureInfo(_desc);
		if (data != nullptr || !m_Path.empty()) {
			GE_ASSERT(m_Path.size() != 0, "texture image is empty!");

			int width, height, nrChannels;

			stbi_set_flip_vertically_on_load_util(true);


			unsigned char* data = stbi_load_util(m_Path.c_str(), &width, &height, &nrChannels, 0);
			GE_ASSERT(data, "fail to load texture data!");
			GLenum format;
			switch (nrChannels) //注意不同图片有不同的通道数！
			{
			case 1:
				format = GL_RED;
				desc.format = Format::R8_UNORM;

				break;
			case 2:
				format = GL_RG;
				desc.format = Format::RG8_UNORM;


				break;
			case 3:
				format = GL_RGB;
				desc.format = Format::RGB8_UNORM;

				break;
			case 4:
				format = GL_RGBA;
				desc.format = Format::RGBA8_UNORM;
				break;
			default:
				GE_CORE_ERROR("Channel {0} has unknown format!", nrChannels)
					break;
			}
			m_Width = width;
			m_Height = height;
//			_desc.width = m_Width;
//			_desc.height = m_Height;
//			_desc.format = desc.format;

			desc.width = m_Width;
			desc.height = m_Height;

			m_Format = format;
		}
		//LoadTexture(image, minFilter, magFilter, internalFormat, format, wrap, dataType);
		
	

		//glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, data);
		
		if (m_Wrap != -1) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);

		if (_desc.generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理

		if (data != nullptr)
			stbi_image_free_util(data);
	}

	//void Texture::Init(
	//	const TextureDesc& desc, float* data)
	//{

	//	/*	FBO(
	//			GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
	//			GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);*/
	//			//纹理过滤---邻近过滤和线性过滤
	//	fillTextureInfo(desc);
	//	glTexImage2D(GL_TEXTURE_2D, 0, m_InnerFormat, m_Width, m_Height, 0, m_Format, m_DataType, data);

	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_MinFilter);//纹理缩小时用邻近过滤
	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_MagFilter);//纹理放大时也用邻近过滤
	//	//TODO:: 是不是可以删掉？
	//	if (m_Wrap != -1) {
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Wrap);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Wrap);
	//	}

	//	if (desc.generateMipmap)
	//		glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理
	//	//	UnBind();
	//}

	void Texture::SetSizeFilter(GLenum min_filter, GLenum mag_filter)
	{
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	}

	void Texture::SetWrapFilter(GLenum filter) {
		Bind();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, filter); //GL_REPEAT 表示纹理X方向循环使用纹理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, filter); //GL_REPEAT 表示纹理y方向循环使用纹理
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, filter);  //GL_REPEAT 表示纹理z方向循环使用纹理
	
		//glTextureParameteri  是 OpenGL 4.5 引入的 Direct State Access (DSA) 函数，提供了更直接的纹理操作方式，减少了状态切换的开销
	}






	void Texture::fillTextureInfo(const TextureDesc& desc)
	{
		m_Path = desc.path;
		m_Type = desc.type;
		m_Width = desc.width;
		m_Height = desc.height;
		auto fm = OpenGLUtil::convertTextureFormatAndDataType(desc.format);
		m_Format = fm.first;
		m_DataType = fm.second;
		m_InnerFormat = OpenGLUtil::convertTextureInnerFormat(desc.format);
		m_MinFilter = OpenGLUtil::convertTextureFilter(desc.minFilter);
		m_MagFilter = OpenGLUtil::convertTextureFilter(desc.magFilter);
		m_Wrap = OpenGLUtil::convertTextureWarp(desc.wrap);
		m_MipMapLevel = desc.mipLevelsCnt;

	

	}

	void Texture::createDefaultSampler(const TextureDesc& desc)
	{
		SamplerDesc samplerDesc;

		samplerDesc.magFilter = desc.magFilter;
		samplerDesc.minFilter = desc.minFilter;
		samplerDesc.mipFilter = desc.mipFilter;
		samplerDesc.addressU = desc.wrap;
		samplerDesc.addressV = desc.wrap;
		samplerDesc.addressW = desc.wrap;

		sampler = DBG_NEW Sampler(samplerDesc);
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

		//glTextureStorage2D(m_TextureID, levels, internal_format, width, height);

		//无需绑定，直接操作纹理 ID , OpenGL 4.5 引入
		glTexStorage2D(GL_TEXTURE_2D, levels, internal_format, width, height);

	}

}