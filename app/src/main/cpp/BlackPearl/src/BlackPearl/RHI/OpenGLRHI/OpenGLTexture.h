#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>

#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/RHIState.h"
//#include "BlackPearl/RHI/Common/stb_util.h"
namespace BlackPearl {

	// opengl 不需要sampler， filter在texture desc 里配置
	//TODO:: 使用同样的名字 TextureDesc，cmake根据API选 .h
	/*struct GLTextureDesc : public TextureDesc 
	{
		GLTextureDesc() {
			minFilter = GL_LINEAR;
			magFilter = GL_LINEAR;
			internalFormat = GL_RGBA;
			wrap = GL_CLAMP_TO_EDGE;
			dataType = GL_UNSIGNED_BYTE;
			generateMipmap = false;
		}
		unsigned int minFilter;
		unsigned int magFilter;
		int internalFormat;
		int wrap;
		unsigned int dataType;
		bool generateMipmap;
	};*/



	class Texture :public RefCounter<ITexture>, public TextureStateExtension
	{
	public:

		TextureDesc m_desc;

	
		/*默认值
		,GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE
		*/
		//Texture(const TextureDesc& desc);
		//没有Image的texture
		//internalFormat: Specifies the number of color components in the texture
		//format:Specifies the format of the pixel data
		Texture(
			TextureDesc& desc,
			float* data = NULL
		);

		//用于CubeMap初始化
		Texture(TextureType type, std::vector<std::string> faces);

		~Texture() override;
		const TextureDesc& getDesc() const override { return m_desc; }
		
		const TextureType& GetType() const override {
			return m_desc.type;
		}

		//void Init(
		//	TextureDesc& desc
		//);

		virtual void Init(
			TextureDesc& desc,
			float* data = nullptr);
		virtual void Bind();
		virtual void UnBind();
		void Storage(GLsizei width, GLsizei height, GLenum internal_format, GLsizei levels = 1);
		void SetSizeFilter(GLenum min_filter, GLenum mag_filter);
		void SetWrapFilter(GLenum filter);
		inline TextureType GetType() { return m_Type; }
		unsigned int GetRendererID() { return m_TextureID; }
		//std::vector<std::string> GetFacesPath() { return m_FacesPath; }
		std::string GetPath() { return m_Path; }
		int GetWidth()const { return m_Width; }
		int GetHeight()const { return m_Height; }
		int GetCurrentLod() const { return m_CurLod; }
		unsigned int	GetMipMapLevel() const { return m_MipMapLevel; }

	protected:
		unsigned int m_TextureID;
		int m_Width = 0;
		int m_Height = 0;
		int m_CurLod = 0;
		std::string m_Path;
		TextureType m_Type;

		GLint m_InnerFormat;
		GLenum m_Format;
		GLenum m_DataType;
		GLint m_MinFilter;
		GLint m_MagFilter;
		GLint m_Wrap = -1;
		unsigned int m_MipMapLevel = 0;
	private:
		std::pair<GLenum, GLenum> _ConvertFormat(Format format);
		GLenum _ConvertInnerFormat(Format format);
		GLint _ConvertFilter(FilterMode filter);
		GLint _ConvertWarp(SamplerAddressMode warp);
	protected:
		void fillTextureInfo(const TextureDesc& desc);
	};



}

