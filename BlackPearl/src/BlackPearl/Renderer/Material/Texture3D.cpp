#include "pch.h"
#include "Texture3D.h"
//#include <glad/glad.h>
#define GLEW_STATIC

#include"GLEW/include/GL/glew.h"
#include "GLFW/glfw3.h"
namespace BlackPearl {

	Texture3D::Texture3D(const std::vector<float>& textureBuffer, const int width, const int height, const int depth, const bool generateMipmaps)
		:m_Width(width),m_Height(height),m_Depth(depth),m_ClearData(4*width*height*depth,0.0f)
	{
		//Generate texture on GPU
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_3D, m_TextureID);

		// Parameter options.
		const auto wrap = GL_CLAMP_TO_BORDER;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap);

		const auto filter = GL_LINEAR_MIPMAP_LINEAR;
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//Upload texture buffer
		 int levels = 7;
		glTexStorage3D(GL_TEXTURE_3D, 7, GL_RGBA8, m_Width, m_Height, m_Depth);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, m_Width, m_Height, m_Depth, 0, GL_RGBA, GL_FLOAT, &textureBuffer[0]);//Upload level0
		if (generateMipmaps) glGenerateMipmap(GL_TEXTURE_3D);
		glBindTexture(GL_TEXTURE_3D, 0);
	}

	Texture3D::~Texture3D()
	{
	}

	void Texture3D::Clear(float clearColor[4])
	{
		GLint previousBoundTextureID;
		glGetIntegerv(GL_TEXTURE_BINDING_3D, &previousBoundTextureID);
		glBindTexture(GL_TEXTURE_3D, m_TextureID);
		glClearTexImage(m_TextureID, 0, GL_RGBA, GL_FLOAT, &clearColor);
		glBindTexture(GL_TEXTURE_3D, previousBoundTextureID);
	}

	void Texture3D::Bind()
	{
		glBindTexture(GL_TEXTURE_3D, m_TextureID);
	}

	void Texture3D::UnBind()
	{
		glBindTexture(GL_TEXTURE_3D, 0);
	}

}
