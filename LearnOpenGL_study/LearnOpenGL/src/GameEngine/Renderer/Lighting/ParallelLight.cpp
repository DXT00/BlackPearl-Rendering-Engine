#include "pch.h"
#include "ParallelLight.h"
#include <glm/gtc/matrix_transform.hpp>
#include "GameEngine/Renderer/Renderer.h"


ParallelLight::~ParallelLight()
{
}

void ParallelLight::Init()
{
	//平行光只提供个方向并不需要Shader渲染！也不需要画出来！


	//data
	//float lightVertices[] = {
	//	-0.5f, -0.5f, -0.5f,
	//	 0.5f, -0.5f, -0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	-0.5f,  0.5f, -0.5f,
	//	-0.5f, -0.5f, -0.5f,

	//	-0.5f, -0.5f,  0.5f,
	//	 0.5f, -0.5f,  0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f,  0.5f,
	//	-0.5f, -0.5f,  0.5f,

	//	-0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f, -0.5f,
	//	-0.5f, -0.5f, -0.5f,
	//	-0.5f, -0.5f, -0.5f,
	//	-0.5f, -0.5f,  0.5f,
	//	-0.5f,  0.5f,  0.5f,

	//	 0.5f,  0.5f,  0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	 0.5f, -0.5f, -0.5f,
	//	 0.5f, -0.5f, -0.5f,
	//	 0.5f, -0.5f,  0.5f,
	//	 0.5f,  0.5f,  0.5f,

	//	-0.5f, -0.5f, -0.5f,
	//	 0.5f, -0.5f, -0.5f,
	//	 0.5f, -0.5f,  0.5f,
	//	 0.5f, -0.5f,  0.5f,
	//	-0.5f, -0.5f,  0.5f,
	//	-0.5f, -0.5f, -0.5f,

	//	-0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f, -0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//	 0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f,  0.5f,
	//	-0.5f,  0.5f, -0.5f,
	//};


	//std::shared_ptr<VertexBuffer> lightVertexBuffer;
	//lightVertexBuffer.reset(new VertexBuffer(lightVertices, sizeof(lightVertices)));
	//m_Shader.reset(new Shader("assets/shaders/Light.glsl"));
	//m_VertexArray.reset(new VertexArray());
	//lightVertexBuffer->SetBufferLayout({
	//	{ElementDataType::Float3,"aPos",false}
	//	});
	//m_VertexArray->AddVertexBuffer(lightVertexBuffer);

	//m_Shader->Bind();
	//m_Shader->SetUniformVec3f("u_LightColor", this->GetLightProps().color);

	//

	m_LightProp.ambient = { 0.2f, 0.2f, 0.2f };
	m_LightProp.diffuse = { 0.5f, 0.5f, 0.5f };
	m_LightProp.specular = { 1.0f, 1.0f,1.0f };

}
