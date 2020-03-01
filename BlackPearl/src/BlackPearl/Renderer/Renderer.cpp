#include "pch.h"
#include "Renderer.h"
#include<glad/glad.h>
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Mesh/Mesh.h"
namespace BlackPearl {

	Renderer::SceneData * Renderer::s_SceneData = DBG_NEW Renderer::SceneData;

	Renderer::Renderer()
	{

	}


	Renderer::~Renderer()
	{
		//delete s_SceneData;
		
	}

	void Renderer::Init()
	{
		glEnable(GL_DEPTH_TEST);
		//多重采样，抗锯齿 // MSAA. Set MSAA level using GLFW (see Config.h).
		glEnable(GL_MULTISAMPLE);
	//	glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Renderer::BeginScene(const Camera & camera, const LightSources& lightSources)
	{
		s_SceneData->LightSources = lightSources;
		s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
		s_SceneData->CameraPosition = camera.GetPosition();
		s_SceneData->CameraRotation = camera.GetRotation();

		s_SceneData->CameraFront = camera.Front();
		s_SceneData->ViewMatrix = camera.GetViewMatrix();
		s_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
		for (Object* lightObj : lightSources.Get()) {
			//std::shared_ptr<Light> lightSource(lightObj->GetComponent<Light>());
			if (lightObj->HasComponent<ParallelLight>()) {
			}
			if (lightObj->HasComponent<PointLight>()) {
				//std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader()->Bind();//一定要记得先Bind()指定是哪一个Shader!
				//glm::mat4 model = glm::mat4(1.0f);
				//model = glm::translate(model, std::dynamic_pointer_cast<PointLight>(lightSource)->GetPosition());
				//model = glm::scale(model, glm::vec3(0.5f));
				//std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader()->SetUniformVec3f("u_LightColor", lightSource->GetLightProps().diffuse);

				//Renderer::Submit(
				//	std::dynamic_pointer_cast<PointLight>(lightSource)->GetVertexArray(),
				//	std::dynamic_pointer_cast<PointLight>(lightSource)->GetShader(),
				//	model
				//);
				//glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			if (lightObj->HasComponent<SpotLight>()) {
				auto lightSource = lightObj->GetComponent<SpotLight>();
				lightSource->UpdatePositionAndDirection(camera.GetPosition(), camera.Front());
			}


		}

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, const glm::mat4 & model, SceneData* sceneData)
	{
		//shader->Bind();
		shader->SetUniformMat4f("u_ProjectionView", sceneData->ViewProjectionMatrix);
		shader->SetUniformMat4f("u_Projection", sceneData->ProjectionMatrix);
		shader->SetUniformMat4f("u_View", sceneData->ViewMatrix);

		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformVec3f("u_CameraViewPos", sceneData->CameraPosition);

		vertexArray->Bind();


	}
}