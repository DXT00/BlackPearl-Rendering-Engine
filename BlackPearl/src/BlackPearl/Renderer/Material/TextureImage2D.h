#pragma once
namespace BlackPearl {
	class TextureImage2D
	{
	public:
		TextureImage2D(
			const int width,
			const int height,
			unsigned int minFilter,
			unsigned int maxFilter,
			int internalFormat,
			int format,
			int wrap,
			unsigned int dataType,
			unsigned int access
		);
		void Bind(unsigned int textureID);
		void UnBind();
		void ShowProperties();
	private:
		unsigned int m_RendererID;
		int m_InternalFormat;
		int m_Acess;



	};


}

