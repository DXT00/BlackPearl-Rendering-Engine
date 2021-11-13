#pragma once
#include <string>
#include <vector>
#include<glad/glad.h>


namespace BlackPearl {

	class Texture
	{
	public:
		enum Type {
			None,
			DiffuseMap,
			SpecularMap,
			EmissionMap,
			NormalMap,
			HeightMap,
			CubeMap,
			DepthMap,
			AoMap,
			RoughnessMap,
			MentallicMap,
			OpacityMap,
			ImageMap


		};
		/*默认值
		,GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE
		*/
		Texture(Type type);
		Texture(Type type,
			const std::string& image,
			unsigned int minFilter = GL_LINEAR,
			unsigned int maxFilter = GL_LINEAR,
			int internalFormat = GL_RGBA,
			int wrap = GL_CLAMP_TO_EDGE,
			unsigned int dataType = GL_UNSIGNED_BYTE,
			bool generateMipmap = false);
		//没有Image的texture
		//internalFormat: Specifies the number of color components in the texture
		//format:Specifies the format of the pixel data
		Texture(
			Type type, const int width, const int height,
			bool isDepth,
			unsigned int minFilter,
			unsigned int maxFilter,
			int internalFormat,
			int format,
			int wrap,
			unsigned int dataType,
			bool generateMipmap = false
		);

		/*
		防止出错不要用默认值
		false,GL_LINEAR,GL_LINEAR,GL_RGBA8,GL_RGBA,GL_CLAMP_TO_EDGE,GL_UNSIGNED_BYTE
		bool isDepth = false,
			unsigned int minFilter= GL_LINEAR,
			unsigned int maxFilter= GL_LINEAR,
			int internalFormat= GL_RGBA8,
			int format= GL_RGBA,
			int wrap = GL_CLAMP_TO_EDGE,
			unsigned int dataType= GL_UNSIGNED_BYTE


		*/
		//用于CubeMap初始化
		Texture(Type type, std::vector<std::string> faces);

		virtual ~Texture();

		void Init(
			const std::string& image,
			unsigned int minFilter,
			unsigned int maxFilter,
			int internalFormat,
			int wrap,
			unsigned int dataType,
			bool generateMipmap
		);
		void Init(
			const int width,
			const int height,
			unsigned int minFilter,
			unsigned int maxFilter,
			GLint internalFormat,
			GLenum format,
			int wrap,
			unsigned int dataType,
			bool generateMipmap);

		virtual void Bind();
		virtual void UnBind();
		void Storage(GLsizei width, GLsizei height, GLenum internal_format, GLsizei levels = 1);
		void SetSizeFilter(GLenum min_filter, GLenum mag_filter);
		void SetWrapFilter(GLenum filter);
		inline Type GetType() { return m_Type; }
		unsigned int GetRendererID() { return m_TextureID; }
		//std::vector<std::string> GetFacesPath() { return m_FacesPath; }
		std::string GetPath() { return m_Path; }
		int GetWidth()const { return m_Width; }
		int GetHeight()const { return m_Height; }

	protected:
		unsigned int m_TextureID;
		int m_Width = 0;
		int m_Height = 0;
		std::string m_Path;
		Type m_Type;
	};



}