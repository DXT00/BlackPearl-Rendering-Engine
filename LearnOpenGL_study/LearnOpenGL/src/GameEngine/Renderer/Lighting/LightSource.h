#pragma once
#include"GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Shader.h"

class LightSource
{
public:
	LightSource(const glm::vec3 &position = {1.2f,1.0f,2.0f})
		:m_Position(position){}


private:
	std::shared_ptr<VertexArray> m_VertexArray;
	std::shared_ptr<Shader> m_Shader;
	glm::vec3 m_Position;
};

