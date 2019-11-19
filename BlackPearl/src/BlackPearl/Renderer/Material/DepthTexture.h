#pragma once
#include "BlackPearl/Renderer/Material/Texture.h"
#include <glad/glad.h>
namespace BlackPearl {
	class DepthTexture:public Texture
	{
	public:
		DepthTexture(Type type, const int width, const int height)
			:Texture(type,width,height, true){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		};
		

		~DepthTexture();
	};

}


