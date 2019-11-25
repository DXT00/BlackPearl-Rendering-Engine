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
		//empty CubeMap,use in CubeShadowMap
		CubeMapTexture(Type type, const int width, const int height)
			:Texture(type, std::vector<std::string>()) {
			LoadCubeMap(width,height);
		};
		virtual ~CubeMapTexture();
		void LoadCubeMap(std::vector<std::string> faces);
		void LoadCubeMap(const int width, const int height);

		virtual void Bind() override ;
		virtual void UnBind()override;
	};

}

