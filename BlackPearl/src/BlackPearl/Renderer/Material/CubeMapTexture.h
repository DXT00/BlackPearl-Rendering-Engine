#pragma once
#include "Texture.h"
namespace BlackPearl {
	class CubeMapTexture:public Texture
	{
	public:
		CubeMapTexture(Type type, std::vector<std::string> faces, int internalFormat, int format, int dataType)
			:Texture(type, faces) {
			LoadCubeMap(faces,internalFormat,format,dataType);
		};
		//empty CubeMap,use in CubeShadowMap
		CubeMapTexture(Type type, const int width, const int height,int internalFormat,int format,int dataType)
			:Texture(type, std::vector<std::string>()) {
			LoadCubeMap(width,height, internalFormat, format, dataType);
		};
		virtual ~CubeMapTexture();
		void LoadCubeMap(std::vector<std::string> faces,int internalFormat, int format, int dataType);
		void LoadCubeMap(const int width, const int height, int internalFormat, int format, int dataType);

		virtual void Bind() override ;
		virtual void UnBind()override;
	};

}

