#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"
//#include "BlackPearl/RHI/Common/stb_util.h"
#define STB_IMAGE_IMPLEMENTATION
namespace BlackPearl {
	class CubeMapTexture: public Texture
	{
	public:
		CubeMapTexture(
			//TextureType type,
			TextureDesc& desc
			/*std::vector<std::string> faces,
			unsigned int minFilter,
			unsigned int maxFilter,
			int wrap,
			int internalFormat, 
			int format, 
			int dataType,
			bool generateMipmap = false,
			unsigned int mipmapLevel = 5*/
		)
			:Texture(desc) {
		
		};
		//empty CubeMap,use in CubeShadowMap
		/*CubeMapTexture(
			TextureType type,
			const int width,
			const int height,
			unsigned int minFilter,
			unsigned int maxFilter,
			int wrap,
			int internalFormat,
			int format,
			int dataType,
			bool generateMipmap = false,
			unsigned int mipmapLevel = 5
		)
			:Texture(type, std::vector<std::string>()) {
			LoadCubeMap(width,height, minFilter, maxFilter,wrap,internalFormat,  format, dataType, generateMipmap);
			m_Width = width;
			m_Height = height;
			m_MipMapLevel = mipmapLevel;
		};*/
		virtual void Init(
			TextureDesc& desc,
			float* data = nullptr) override;
		virtual ~CubeMapTexture() override;
		void	LoadCubeMap();
		/*void	LoadCubeMap(std::vector<std::string> faces, unsigned int minFilter,unsigned int maxFilter,int wrap,int internalFormat, int format, int dataType, bool generateMipmap);
		void	LoadCubeMap(const int width, const int height, unsigned int minFilter, unsigned int maxFilter,int wrap, int internalFormat,int format, int dataType, bool generateMipmap);*/
		

		virtual void Bind() override ;
		virtual void UnBind()override;
	
	};

}

