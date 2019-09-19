#include "pch.h"
#include "Renderer.h"
#include<glad/glad.h>
#include "Lighting/Light.h"
#include "Lighting/ParallelLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "Lighting/LightType.h"
#include "glm/gtc/matrix_transform.hpp"
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

void Renderer::BeginScene(const Camera & camera,const std::shared_ptr<Light> lightSource)
{
	m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	switch (LightType::Get()) {
	case LightType::Type::ParallelLight:
		break;
	case LightType::Type::PointLight:
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, std::dynamic_pointer_cast<PointLight>(lightSource)->GetPosition());
		model = glm::scale(model, glm::vec3(0.5f));
		std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader()->SetUniformVec3f("u_LightColor", lightSource->GetLightProps().color);
		Renderer::Submit(std::dynamic_pointer_cast<PointLight>(lightSource)->GetVertexArray(), std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader(), model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		break;
	case LightType::Type::SpotLight:
		std::dynamic_pointer_cast<SpotLight>(lightSource)->UpdatePositionAndDirection(camera.GetPosition(), camera.Front());
		break;


	 default:
		 GE_CORE_ERROR("BeginScene failed! Unknown Light Type")
			 break;

	}
}

void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4 & model)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ProjectionView", m_SceneData->ViewProjectionMatrix);
	shader->SetUniformMat4f("u_Model", model);
	vertexArray->Bind();





}
