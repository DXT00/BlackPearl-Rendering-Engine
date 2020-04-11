#pragma once
namespace BlackPearl {
	class TextureImage2D
	{
	public:
		TextureImage2D(
			const std::vector<float>& textureBuffer,
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
		~TextureImage2D();
		unsigned int GetRendererID() const { return m_RendererID; }
		void Bind(unsigned int textureID);
		void BindImage(unsigned int textureID);//do not need to activate
		void UnBind();
		void Clear(float clearColor[4]);
		void ShowProperties();

		unsigned int GetWidth()const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
	private:
		unsigned int m_RendererID;
		int m_InternalFormat;
		int m_Acess;
		unsigned int m_Format;
		unsigned int m_DataType;
		bool m_Initial = true;

		unsigned int m_Width, m_Height;

	};


}

