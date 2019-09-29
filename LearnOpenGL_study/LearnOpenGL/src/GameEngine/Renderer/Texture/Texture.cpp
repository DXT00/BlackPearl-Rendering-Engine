#include"pch.h"
#include "Texture.h"
#include"GameEngine/Core.h"
#include <glad/glad.h>
#include "stb_image.h"



Texture::Texture(Type type, const std::string &image)
{
	m_Path = image;
	m_Type = type;
	glGenTextures(1, &m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	//set the texture warpping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);//设置s轴
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//设置t轴

	//纹理过滤---邻近过滤和线性过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//纹理缩小时用邻近过滤
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//纹理放大时也用邻近过滤

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);//纹理缩小的时候，使用最邻近Mipmap匹配像素大小，并使用邻近插值进行纹理采样
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//纹理放大时也用邻近过滤

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(m_Path.c_str(), &width, &height, &nrChannels, 0);
	GE_ASSERT(data, "fail to load texture data!")

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
		GE_CORE_ERROR("Channel {0} has unknown format!",nrChannels)
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);//为当前绑定的纹理自动生成所有需要的多级渐远纹理
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_image_free(data);


}


void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::UnBind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
