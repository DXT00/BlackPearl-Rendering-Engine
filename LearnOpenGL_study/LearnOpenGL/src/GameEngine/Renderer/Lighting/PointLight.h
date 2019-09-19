#pragma once
#include "Light.h"
#include<glm/glm.hpp>
#include"GameEngine/Renderer/Shader.h"
class PointLight :public Light
{
public:
	struct Attenuation {
		float constant;
		float linear;
		float quadratic;
		Attenuation():constant(1.0f), linear(0.22f), quadratic(0.2){}
		//默认距离50 constant(1.0f),linear(0.09f),quadratic(0.032){}
		//默认距离3250constant(1.0f),linear(0.0014f),quadratic(0.000007)
		//查表：https://learnopengl-cn.github.io/02%20Lighting/05%20Light%20casters/
	};
	PointLight(const glm::vec3& position)
		:m_Position(position) {
		Init();
	}
	virtual ~PointLight() = default;
	virtual void Init() override;

	inline void SetPosition(const glm::vec3& position) { m_Position = position; }
	inline glm::vec3 GetPosition() { return m_Position; }
	inline Attenuation GetAttenuation() const{ return m_Attenuation; }

	std::shared_ptr<VertexArray> GetVertexArray() { return m_VertexArray; };
	std::shared_ptr<Shader> GetShader() { return m_Shader; };
private:
	glm::vec3 m_Position;
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_Shader;

	Attenuation m_Attenuation;
};

