#pragma once
#include"GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Shader.h"
class Renderer
{
public:
	Renderer();
	~Renderer();
	static void Init();

	void Submit(const std::shared_ptr<VertexArray>& vertexArray,const std::shared_ptr<Shader>& shader,const glm::mat4 &model = glm::mat4(1.0f));


};

