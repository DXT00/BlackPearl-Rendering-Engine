#pragma once
#include "Light.h"

class ParallelLight :public Light
{
public:
	ParallelLight(const glm::vec3& direction)
		:m_Direction(direction) {
		Init();
	}
	~ParallelLight();
	virtual void Init() override;

	inline void SetDirection(const glm::vec3& direction) { m_Direction = direction; }
	inline glm::vec3 GetDirection() { return m_Direction; }

	virtual std::shared_ptr<VertexArray> GetVertexArray()override { return m_VertexArray; };
	virtual std::shared_ptr<Shader> GetShader() override { return m_Shader; };
private:
	glm::vec3 m_Direction;

	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_Shader;
};

