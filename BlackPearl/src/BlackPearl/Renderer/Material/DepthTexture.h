#pragma once
#include "BlackPearl/Renderer/Material/Texture.h"
#include <glad/glad.h>
namespace BlackPearl {
	class DepthTexture:public Texture
	{
	public:
		DepthTexture(Type type, const int width, const int height)
			:Texture(type,width,height, true, GL_LINEAR, GL_LINEAR, GL_RGBA8, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};
		

		~DepthTexture();
	};

}


