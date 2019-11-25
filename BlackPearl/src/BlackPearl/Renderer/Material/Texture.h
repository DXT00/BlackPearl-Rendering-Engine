#pragma once
#include <string>
#include <vector>



namespace BlackPearl {

	class Texture
	{
	public:
		enum Type {
			DiffuseMap,
			SpecularMap,
			EmissionMap,
			NormalMap,
			HeightMap,
			CubeMap,
			DepthMap
		};
		Texture(Type type, const std::string &image);
		//没有Image的texture
		Texture(Type type, const int width,const int height, bool isDepth = false);

		//用于CubeMap初始化
		Texture(Type type, std::vector<std::string> faces);


		virtual ~Texture() = default;

		 void Init(const std::string &image);
		 void Init(const int width, const int height);


		void LoadTexture(const std::string &image);
		virtual void Bind();
		virtual void UnBind();
		inline Type GetType() { return m_Type; }
		unsigned int GetRendererID() { return m_TextureID; }
		std::vector<std::string> GetFacesPath() { return m_FacesPath; }
		std::string GetPath() { return m_Path; }

	protected:
		unsigned int m_TextureID;
		std::vector<std::string> m_FacesPath;
		std::string m_Path;
		Type m_Type;
	};



}