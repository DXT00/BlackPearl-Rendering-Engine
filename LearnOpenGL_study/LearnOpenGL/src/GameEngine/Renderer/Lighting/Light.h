#pragma once
#include"GameEngine/Renderer/VertexArray.h"
#include "GameEngine/Renderer/Shader.h"



class Light
{
public:
	struct Props {
		glm::vec3 color;//光源本身的颜色
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		Props() :color({1.0f,1.0f,1.0f}), ambient({ 1.0f,1.0f,1.0f }), diffuse({ 1.0f,1.0f,1.0f }), specular({ 1.0f,1.0f,1.0f }) {}
	};
	//position{2.2f,1.0f,2.0f}

	

	inline Props GetLightProps() const { return  m_LightProp; }
//	virtual std::shared_ptr<VertexArray> GetVertexArray() = 0;
	//virtual std::shared_ptr<Shader> GetShader() = 0;

	virtual void Init() = 0;
	static Light* Create(const glm::vec3& position = { 2.2f,1.0f,2.0f }, const glm::vec3& direction = { -0.2f, -1.0f, -0.3f },Props props=Props());

protected:
	Props m_LightProp;
	
};

