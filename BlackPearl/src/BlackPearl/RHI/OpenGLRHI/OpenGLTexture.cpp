#include "pch.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/RHIDefinitions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace BlackPearl {

	//Texture::Texture(const TextureDesc& desc)
	//	:TextureStateExtension(desc)
	//{
	//	glGenTextures(1, &m_TextureID);
	//}



	/*
	internalforamt指的是纹理数据在OpenGL中是如何表示的，如GL_RGB就表示纹理的像素在OpenGL里面以红绿蓝三个分量表示，

	format指的是载入纹理的格式，它告诉OpenGL外部数据是如何存储每个像素数据的。
	
	*/
	Texture::Texture(
		TextureDesc& desc,
		float* data
	) : desc(desc),
		TextureStateExtension(desc)
	{
		
		glGenTextures(1, &m_TextureID);
		GE_ERROR_JUDGE();//出现error的原因：很可能m_TextureID用在了别的target上，例如CUBEMAP,不行的话运行前加个断点 = = 

		Bind();
		GE_ERROR_JUDGE();
		Init(desc, data);
		/*if (data != nullptr) {
			
		}
		else {
			assert(!m_Path.empty());
			Init(desc);
		}*/
		GE_ERROR_JUDGE();


	}
	//Use in ShadowMap (DepthMap) or FrameBuffer's empty ColorMap
	//Texture::Texture(
	//	Type type,
	//	const int width,
	//	const int height,
	//	bool isDepth,
	//	unsigned int minFilter,
	//	unsigned int maxFilter,
	//	int internalFormat,
	//	int format,
	//	int wrap,
	//	unsigned int dataType,
	//	bool generateMipmap,
	//	float* data
	//) :TextureStateExtension(desc)
	//{
	//	m_Width = width;
	//	m_Height = height;
	//	m_Path = "";
	//	m_Type = type;
	//	glGenTextures(1, &m_TextureID);

	//	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	//	GE_ERROR_JUDGE();
	//	Init(width, height, minFilter, magFilter, internalFormat, format, wrap, dataType, generateMipmap, data);
	//	GE_ERROR_JUDGE();


	//}
	//Use in CubeMap
	//Texture::Texture(TextureType type, std::vector<std::string> faces)
	//	:desc(desc),
	//	TextureStateExtension(desc) {
	//	m_Path = "";
	//	//m_FacesPath = faces;
	//	m_Type = type;
	//	glGenTextures(1, &m_TextureID);

	//}

	Texture::~Texture()
	{
		UnBind();
		glDeleteTextures(1, &m_TextureID);
	}


	void Texture::Init(
		TextureDesc& desc, float* data)
	{
		fillTextureInfo(desc);
		if (data != nullptr) {
			GE_ASSERT(m_Path.size() != 0, "texture image is empty!");

			int width, height, nrChannels;

			stbi_set_flip_vertically_on_load(true);


			unsigned char* data = stbi_load(m_Path.c_str(), &width, &height, &nrChannels, 0);
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

		if (desc.generateMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理

		if (data != nullptr)
			stbi_image_free(data);
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

	void Texture::SetSizeFilter(GLenum min_filter, GLenum mag_filter) {

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, min_filter);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, mag_filter);
	}
	void Texture::SetWrapFilter(GLenum filter) {

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, filter); //GL_REPEAT 表示纹理X方向循环使用纹理
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, filter); //GL_REPEAT 表示纹理y方向循环使用纹理
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_R, filter);  //GL_REPEAT 表示纹理z方向循环使用纹理
	}

	//return <format, dataType>
	std::pair<GLenum, GLenum> Texture::_ConvertFormat(Format format)
	{
		//https://www.khronos.org/opengl/wiki/OpenGL_Type
		//GL_UNSIGNED_BYTE --> 8 bit
		//GL_BYTE --> 8 bit

		//GL_UNSIGNED_INT --> 32bit
		// GL_INT --> 32bit

		//GL_UNSIGNED_SHORT -->16bit
		//GL_SHORT --> 16bit

		switch (format)
		{
		case Format::R8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RED, GL_UNSIGNED_BYTE);;
		case Format::R32_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RED, GL_FLOAT);

		case Format::RG16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RG, GL_FLOAT);

		case Format::RGB8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RGB, GL_UNSIGNED_BYTE);

		case Format::RGB32_FLOAT:
		case Format::RGB16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RGB, GL_FLOAT);

		case Format::RGBA8_UNORM:
			return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_UNSIGNED_BYTE);

		case Format::RGBA32_FLOAT:
		case Format::RGBA16_FLOAT:
			return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_FLOAT);

		case Format::D16:
		case Format::D32:
			return std::make_pair<GLenum, GLenum>(GL_DEPTH_COMPONENT, GL_FLOAT);

		default:
			GE_ASSERT(0, "unsupport now");
			break;
		}
		return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_UNSIGNED_BYTE);

	}

	GLenum Texture::_ConvertInnerFormat(Format format)
	{
		switch (format)
		{

		case Format::R8_UNORM:
			return GL_RED;
		case Format::R32_FLOAT:
			return GL_R32F;
		case Format::RG16_FLOAT:
			return GL_RG16F;

		case Format::RGB8_UNORM:
			return GL_RGB8;

		case Format::RGB8_FLOAT:
			return GL_RGB8;

		case Format::RGB32_FLOAT:
			return GL_RGB32F;

		case Format::RGB16_FLOAT:
			return GL_RGB16F;

		case Format::RGBA8_UNORM:
			return GL_RGBA;

		case Format::RGBA32_FLOAT:
			return GL_RGBA32F;
		case Format::RGBA16_FLOAT:
			return GL_RGBA16F;

			//return std::make_pair<GLenum, GLenum>(GL_RGBA, GL_FLOAT);
		case Format::D16:
			return GL_DEPTH_COMPONENT16;
		case Format::D32:
			return GL_DEPTH_COMPONENT32;

		default:
			GE_ASSERT(0, "unsupport now");
			break;
	}
		return GL_RGBA;
	}

	GLint Texture::_ConvertFilter(FilterMode filter)
	{
		switch (filter)
		{
		case BlackPearl::FilterMode::Linear:
			return GL_LINEAR;
		case BlackPearl::FilterMode::Linear_Mip_Linear:
			return GL_LINEAR_MIPMAP_LINEAR;
		case BlackPearl::FilterMode::Nearest:
			return GL_NEAREST;
		case BlackPearl::FilterMode::Nearest_Mip_Nearnest:
			return GL_NEAREST_MIPMAP_NEAREST;

		default:
			assert(0);
			break;
		}
		return GL_LINEAR;

	}

	GLint Texture::_ConvertWarp(SamplerAddressMode warp)
	{
		switch (warp) {
		case SamplerAddressMode::ClampToEdge:
			return GL_CLAMP_TO_EDGE;
		case SamplerAddressMode::ClampToBorder:
			return GL_CLAMP_TO_BORDER;
		default:
			assert(0);
		}

		return GL_CLAMP_TO_EDGE;

	}



	void Texture::fillTextureInfo(const TextureDesc& desc)
	{
		m_Path = desc.path;
		m_Type = desc.type;
		m_Width = desc.width;
		m_Height = desc.height;
		auto fm = _ConvertFormat(desc.format);
		m_Format = fm.first;
		m_DataType = fm.second;
		m_InnerFormat = _ConvertInnerFormat(desc.format);
		m_MinFilter = _ConvertFilter(desc.minFilter);
		m_MagFilter = _ConvertFilter(desc.magFilter);
		m_Wrap = _ConvertWarp(desc.wrap);
		m_MipMapLevel = desc.mipLevels;

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