#pragma once
#include <glm/glm.hpp>
class MaterialColor
{
public:
	enum Type {
		AmbientColor,
		DiffuseColor,
		SpecularColor,
		EmissionColor

	};
	MaterialColor(Type type, const glm::vec3& color)
		:m_Type(type),m_Color(color){}


	~MaterialColor() = default;
	inline Type GetType() const{ return m_Type; }
	inline glm::vec3 Get() const { return m_Color; }

private:
	Type m_Type;
	glm::vec3 m_Color;
};

