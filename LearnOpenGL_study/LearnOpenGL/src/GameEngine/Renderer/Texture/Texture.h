#pragma once
#include <string>

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
	Texture(Type type,const std::string &image);
	~Texture() {};

	void Bind();
	void UnBind();
	inline Type GetType() { return m_Type; }
private:
	unsigned int m_TextureID;
	std::string m_Path;
	Type m_Type;
};



