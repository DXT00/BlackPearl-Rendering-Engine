#include "pch.h"
#include "Renderer.h"
#include<glad/glad.h>

Renderer::SceneData * Renderer::m_SceneData = new Renderer::SceneData;

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}

void Renderer::Init()
{
	glEnable(GL_DEPTH_TEST);
}

void Renderer::BeginScene(const Camera & camera)
{
	m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4 & model)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ProjectionView", m_SceneData->ViewProjectionMatrix);
	shader->SetUniformMat4f("u_Model", model);
	vertexArray->Bind();





}
