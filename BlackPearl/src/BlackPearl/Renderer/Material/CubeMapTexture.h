#pragma once
#include "Texture.h"
namespace BlackPearl {
	class CubeMapTexture:public Texture
	{
	public:
		CubeMapTexture(
			Type type, 
			std::vector<std::string> faces,
			unsigned int minFilter,
			unsigned int maxFilter,
			int wrap,
			int internalFormat, 
			int format, 
			int dataType,
			bool generateMipmap = false,
			unsigned int mipmapLevel = 5
		)
			:Texture(type, faces) {
			LoadCubeMap(faces, minFilter, maxFilter, wrap,internalFormat,format,dataType, generateMipmap);
			m_MipMapLevel = mipmapLevel;
		};
		//empty CubeMap,use in CubeShadowMap
		CubeMapTexture(
			Type type,
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
		};
		virtual ~CubeMapTexture();
		void	LoadCubeMap(std::vector<std::string> faces, unsigned int minFilter,unsigned int maxFilter,int wrap,int internalFormat, int format, int dataType, bool generateMipmap);
		void	LoadCubeMap(const int width, const int height, unsigned int minFilter, unsigned int maxFilter,int wrap, int internalFormat,int format, int dataType, bool generateMipmap);
		
		int				GetWidth()const { return m_Width; }
		int				GetHeight()const { return m_Height; }
		unsigned int	GetMipMapLevel() const { return m_MipMapLevel; }

		virtual void	Bind() override ;
		virtual void	UnBind()override;
	private:
		int m_Width;
		int m_Height;
		unsigned int m_MipMapLevel;
	};

}

