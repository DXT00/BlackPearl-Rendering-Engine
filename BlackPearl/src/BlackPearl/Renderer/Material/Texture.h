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
		~Texture() = default;

		void Bind();
		void UnBind();
		inline Type GetType() { return m_Type; }
	private:
		unsigned int m_TextureID;
		std::string m_Path;
		Type m_Type;
	};



}