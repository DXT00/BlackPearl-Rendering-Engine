#include "pch.h"
#include<glad/glad.h>
#include "Renderer.h"
#include "BlackPearl/Component/LightComponent/Light.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Mesh/Mesh.h"
using namespace BlackPearl::math;
#include <hlsl/core/view_cb.h>

namespace BlackPearl {

	SceneData * Renderer::s_SceneData = DBG_NEW SceneData();
	SceneData* Renderer::s_PreSceneData = DBG_NEW SceneData();

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
		s_PreSceneData = s_SceneData;
		s_SceneData->LightSources = lightSources;
		s_SceneData->ProjectionViewMatrix = camera.GetProjectionViewMatrix();
		s_SceneData->CameraPosition = camera.GetPosition();
		s_SceneData->CameraRotation = camera.GetRotation();

		s_SceneData->CameraFront = camera.Front();
		s_SceneData->ViewMatrix = camera.GetViewMatrix();
		s_SceneData->ProjectionMatrix = camera.GetProjectionMatrix();
		s_SceneData->SetViewport(RHIViewport(Configuration::WindowWidth, Configuration::WindowHeight));

		s_SceneData->ViewFrustum = math::frustum(Math::ToFloat4x4(s_SceneData->ViewMatrix * s_SceneData->ProjectionMatrix), s_SceneData->ReverseZ);
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
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));

		shader->SetUniformMat4f("u_ProjectionView", sceneData->ProjectionViewMatrix);
		shader->SetUniformMat4f("u_Projection", sceneData->ProjectionMatrix);
		shader->SetUniformMat4f("u_View", sceneData->ViewMatrix);

		shader->SetUniformMat4f("u_Model", model);
		shader->SetUniformVec3f("u_CameraViewPos", sceneData->CameraPosition);

		vertexArray->Bind();


	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, float* model, uint32_t objCnt, SceneData* sceneData)
	{

		////shader->Bind();
		//for (size_t id = 0; id < objCnt; id++)
		//{
		//	for (size_t i = 0; i < 12; i++)
		//	{
		//		GE_CORE_INFO("objid = " + std::to_string(id) + ", i=" + std::to_string(i) + ", m[i]=" + std::to_string(model[id * 12 + i]));

		//	}
		//}


		shader->SetUniformMat4f("u_ProjectionView", sceneData->ProjectionViewMatrix);
		shader->SetUniformMat4f("u_Projection", sceneData->ProjectionMatrix);
		shader->SetUniformMat4f("u_View", sceneData->ViewMatrix);

		if (model != nullptr && objCnt != 0) {
			shader->SetUniformMat3x4f("u_Model", model, objCnt);
		}
		//shader->SetUniformMat4f("u_Model", model, objCnt);

		shader->SetUniformVec3f("u_CameraViewPos", sceneData->CameraPosition);

		vertexArray->Bind();


	}
	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Shader>& shader, SceneData* sceneData)
	{
		shader->SetUniformMat4f("u_ProjectionView", sceneData->ProjectionViewMatrix);
		shader->SetUniformMat4f("u_Projection", sceneData->ProjectionMatrix);
		shader->SetUniformMat4f("u_View", sceneData->ViewMatrix);
		
		shader->SetUniformVec3f("u_CameraViewPos", sceneData->CameraPosition);

		vertexArray->Bind();
	}

	void SceneData::SetViewport(RHIViewport viewport)
	{
		if (viewport == m_Viewport)
			return;
		m_Viewport = viewport;
		m_ScissorRect = RHIRect(viewport);
	}

	ViewportState SceneData::GetViewportState() const
	{
		ViewportState vs;
		vs.addViewport(m_Viewport);
		vs.addScissorRect(m_ScissorRect);
		return vs;
	}
	VariableRateShadingState SceneData::GetVariableRateShadingState() const
	{
		return m_ShadingRateState;
	}
	math::frustum SceneData::GetViewFrustum() const
	{
		return ViewFrustum;
	}

	void SceneData::FillPlanarViewConstants(ForwardShadingViewConstants& constants) const
	{

		//constants.matWorldToView = affineToHomogeneous(GetViewMatrix());
		//constants.matViewToClip = GetProjectionMatrix(true);
		//constants.matWorldToClip = GetViewProjectionMatrix(true);
		//constants.matClipToView = GetInverseProjectionMatrix(true);
		//constants.matViewToWorld = affineToHomogeneous(GetInverseViewMatrix());
		//constants.matClipToWorld = GetInverseViewProjectionMatrix(true);
		//constants.matViewToClipNoOffset = GetProjectionMatrix(false);
		//constants.matWorldToClipNoOffset = GetViewProjectionMatrix(false);
		//constants.matClipToViewNoOffset = GetInverseProjectionMatrix(false);
		//constants.matClipToWorldNoOffset = GetInverseViewProjectionMatrix(false);

		//ViewportState viewportState = GetViewportState();
		//const RHIViewport& viewport = viewportState.viewports[0];
		//constants.viewportOrigin = float2(viewport.minX, viewport.minY);
		//constants.viewportSize = float2(viewport.width(), viewport.height());
		//constants.viewportSizeInv = 1.f / constants.viewportSize;

		//constants.clipToWindowScale = float2(0.5f * viewport.width(), -0.5f * viewport.height());
		//constants.clipToWindowBias = constants.viewportOrigin + constants.viewportSize * 0.5f;

		//constants.windowToClipScale = 1.f / constants.clipToWindowScale;
		//constants.windowToClipBias = -constants.clipToWindowBias * constants.windowToClipScale;

		//constants.cameraDirectionOrPosition = IsOrthographicProjection()
		//	? float4(GetViewDirection(), 0.f)
		//	: float4(GetViewOrigin(), 1.f);

		//constants.pixelOffset = GetPixelOffset();
		constants.matProjectionView =  Math::ToFloat4x4(ProjectionViewMatrix);
		constants.matView = Math::ToFloat4x4(ViewMatrix);
		constants.matProjection = Math::ToFloat4x4(ProjectionMatrix);
		constants.cameraPos = Math::ToFloat3(CameraPosition);
		constants.cameraRot = Math::ToFloat3(CameraRotation);
		constants.viewportSize = math::float2(m_Viewport.width(), m_Viewport.height());
		constants.viewportOrigin = math::float2(m_Viewport.minX, m_Viewport.minY);

	}
	
}