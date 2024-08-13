#pragma once
#include <string>
#include <vector>
#ifdef GE_PLATFORM_ANDRIOD
#include "GLES3/gl3.h"
#endif
#ifdef GE_PLATFORM_WINDOWS
#include "glad/glad.h"
#endif


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
		/*Ĭ��ֵ
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
		//û��Image��texture
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
			bool generateMipmap = false,
			float* data = NULL
		);

		/*
		��ֹ����Ҫ��Ĭ��ֵ
		false,GL_LINEAR,GL_LINEAR,GL_RGBA8,GL_RGBA,GL_CLAMP_TO_EDGE,GL_UNSIGNED_BYTE
		bool isDepth = false,
			unsigned int minFilter= GL_LINEAR,
			unsigned int maxFilter= GL_LINEAR,
			int internalFormat= GL_RGBA8,
			int format= GL_RGBA,
			int wrap = GL_CLAMP_TO_EDGE,
			unsigned int dataType= GL_UNSIGNED_BYTE


		*/
		//����CubeMap��ʼ��
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
			bool generateMipmap, float* data = NULL);

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
		int GetCurrentLod() const { return m_CurLod; }

	protected:
		unsigned int m_TextureID;
		int m_Width = 0;
		int m_Height = 0;
		int m_CurLod = 0;
		std::string m_Path;
		Type m_Type;
	};



}