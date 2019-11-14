#pragma once
#include "Texture.h"
namespace BlackPearl {
	class CubeMapTexture:public Texture
	{
	public:
		CubeMapTexture(Type type, std::vector<std::string> faces)
			:Texture(type, faces) {
			LoadCubeMap(faces);
		};
		virtual ~CubeMapTexture();

		void LoadCubeMap(std::vector<std::string> faces) ;
		virtual void Bind()override;
		virtual void UnBind()override;
	};

}

