#include "pch.h"
#ifdef GE_PLATFORM_ANDRIOD
#include "GLES3/gl3.h"
#endif
#ifdef GE_PLATFORM_WINDOWS
#include "glad/glad.h"
#endif
#include "HDRTexture.h"

#include "BlackPearl/Core.h"

#include "stb_image.h"

namespace BlackPearl {
	

	HDRTexture::HDRTexture(const std::string & image)
	{
		glGenTextures(1, &m_RendererID);
		Bind();
		LoadHdrTexture(image);
		UnBind();
	}

	HDRTexture::~HDRTexture()
	{
	}
	void HDRTexture::LoadHdrTexture(const std::string & image)
	{
		GE_ASSERT(image.size() != 0, "texture image is empty!");

		int width, height, nrChannels;

		stbi_set_flip_vertically_on_load(true);


		float  *data = stbi_loadf(image.c_str(), &width, &height, &nrChannels, 0);
		GE_ASSERT(data, "fail to load texture data!");
		GLenum format;
		switch (nrChannels) //ע�ⲻͬͼƬ�в�ͬ��ͨ������
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
		//lPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, format, GL_FLOAT, data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, format, GL_FLOAT, data);

		glGenerateMipmap(GL_TEXTURE_2D);//Ϊ��ǰ�󶨵������Զ�����������Ҫ�Ķ༶��Զ����
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		UnBind();

		stbi_image_free(data);

	}
	void HDRTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	void HDRTexture::UnBind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}