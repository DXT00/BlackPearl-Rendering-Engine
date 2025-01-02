#pragma once
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLTexture.h"

namespace BlackPearl {
	class ImageTexture2D: public Texture
	{
	public:
		ImageTexture2D(
			TextureDesc& desc,
			float* textureBuffer
		/*	const int width,
			const int height,
			unsigned int minFilter,
			unsigned int maxFilter,
			int internalFormat,
			int format,
			int wrap,
			unsigned int dataType,
			unsigned int access*/
			
		);
		~ImageTexture2D();

		virtual void Bind() override;
		virtual void Init(
			TextureDesc& desc,
			float* data = 0) override;


		void Bind(unsigned int textureID);
		void BindImage(unsigned int textureID);//do not need to activate
		void UnBind();
		void Clear(float clearColor[4]);
		void ShowProperties();

	private:
		GLint _ConvertAccess(bool isUav);

		int m_Acess;

		bool m_Initial = true;


	};


}

