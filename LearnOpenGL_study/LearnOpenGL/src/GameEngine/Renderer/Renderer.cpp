#include "pch.h"
#include "Renderer.h"
#include<glad/glad.h>
#include "Lighting/Light.h"
#include "Lighting/ParallelLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "Lighting/LightSources.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Mesh.h"
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::BeginScene(const Camera & camera,const LightSources& lightSources)
{
	m_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
	m_SceneData->CameraPosition = camera.GetPosition();
	m_SceneData->CameraFront = camera.Front();
	for (auto lightSource : lightSources.Get()) {
		switch (lightSource->GetType()) {
			case LightType::ParallelLight:
				break;
			case LightType::PointLight:
				std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader()->Bind();//一定要记得先Bind()指定是哪一个Shader!
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, std::dynamic_pointer_cast<PointLight>(lightSource)->GetPosition());
				model = glm::scale(model, glm::vec3(0.5f));
				std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader()->SetUniformVec3f("u_LightColor", lightSource->GetLightProps().diffuse);

				Renderer::Submit(
					std::dynamic_pointer_cast<PointLight>(lightSource)->GetVertexArray(),
					std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader(),
					model
				);
				glDrawArrays(GL_TRIANGLES, 0, 36);
				break;
			case LightType::SpotLight:
				std::dynamic_pointer_cast<SpotLight>(lightSource)->UpdatePositionAndDirection(camera.GetPosition(), camera.Front());
				break;

			default:
				GE_CORE_ERROR("BeginScene failed! Unknown Light Type")
					break;

		}
	}
	
}

void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4 & model)
{
	shader->Bind();
	shader->SetUniformMat4f("u_ProjectionView", m_SceneData->ViewProjectionMatrix);
	shader->SetUniformMat4f("u_Model", model);
	shader->SetUniformVec3f("u_CameraViewPos", m_SceneData->CameraPosition);

	vertexArray->Bind();


}
