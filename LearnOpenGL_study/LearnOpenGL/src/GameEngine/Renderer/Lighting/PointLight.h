#pragma once
#include "Light.h"
class PointLight :public Light
{
public:
	PointLight(const glm::vec3& position)
		:m_Position(position) {
		Init();
	}
	~PointLight();
	virtual void Init() override;

	inline void SetPosition(const glm::vec3& position) { m_Position = position; }
	inline glm::vec3 GetPosition() { return m_Position; }

	std::shared_ptr<VertexArray> GetVertexArray() { return m_VertexArray; };
	std::shared_ptr<Shader> GetShader() { return m_Shader; };
private:
	glm::vec3 m_Position;
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_Shader;

};

