#pragma once
#include <string>
namespace BlackPearl {

	class Texture
	{
	public:
		enum Type {
			DiffuseMap,
			SpecularMap,
			EmissionMap,
			NormalMap,
			HeightMap
		};
		Texture(Type type, const std::string &image);
		//Ã»ÓÐImageµÄtexture
		Texture(Type type, const int width,const int height);

		~Texture() = default;

		void Bind();
		void UnBind();
		inline Type GetType() { return m_Type; }
		unsigned int GetRendererID() { return m_TextureID; }
	private:
		unsigned int m_TextureID;
		std::string m_Path;
		Type m_Type;
	};



}