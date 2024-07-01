#include "pch.h"
#include "ShadowMapRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "BlackPearl/Renderer/Mesh/Mesh.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
namespace BlackPearl {

	float ShadowMapRenderer::s_NearPlane = 1.0f;
	float ShadowMapRenderer::s_FarPlane = 7.5f;
	int   ShadowMapRenderer::s_ShadowMapWidth = 1024;
	int   ShadowMapRenderer::s_ShadowMapHeight = 1024;

	ShadowMapRenderer::~ShadowMapRenderer()
	{
	}
	void ShadowMapRenderer::Render(const std::vector<Object*>& objs, ParallelLight* sun, const std::vector<Object*>&exceptObjs)
	{

		//glm::mat4 lightProjection, lightView;

		//m_LightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,s_NearPlane,s_FarPlane);
		//m_LightView = glm::lookAt(sun->GetDirection(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		//m_LightProjectionViewMatrix = m_LightProjection * m_LightView;
		m_LightPos = sun->GetDirection();
		m_ShadowBox->Update();
		UpdateLightOrthoProjectionMatrix(m_ShadowBox->GetWidth(), m_ShadowBox->GetHeight(), m_ShadowBox->GetLength());
		UpdateLightViewMatrix(Math::ToVec3(sun->GetDirection()), m_ShadowBox->GetCenter());
		m_LightProjectionViewMatrix = m_LightProjection * m_LightView;

		glViewport(0, 0, s_ShadowMapWidth, s_ShadowMapHeight);
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		for (auto exceptObj : exceptObjs) {
			exceptObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
		}
		m_SimpleDepthShader->Bind();
		m_SimpleDepthShader->SetUniformVec3f("u_LightPos", m_LightPos);
		m_SimpleDepthShader->SetUniformMat4f("u_LightProjectionViewMatrix", m_LightProjectionViewMatrix);
		DrawObjects(objs, m_SimpleDepthShader);


		
		m_FrameBuffer->UnBind();

		for (auto exceptObj : exceptObjs) {
			exceptObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		}

	}
	//void ShadowMapRenderer::PrepareShaderParameters(Mesh mesh, glm::mat4 transformMatrix, std::shared_ptr<Shader> shader, bool isLight) //RenderScene 会调用该虚函数
	//{
	//	PrepareBasicShaderParameters(mesh, transformMatrix, shader, isLight);

	//	shader->SetUniformVec3f("u_LightPos", m_LightPos);
	//	shader->SetUniformMat4f("u_LightProjectionViewMatrix", m_LightProjectionViewMatrix);

	//}

	void ShadowMapRenderer::UpdateLightOrthoProjectionMatrix(float boxWidth, float boxHeight, float boxLength)
	{
		//orthographic matrix definition: http://learnwebgl.brown37.net/08_projections/projections_ortho.html
		glm::mat4 orthoProjectionMatrix(1.0f);
		orthoProjectionMatrix[0][0] = 2.0f / boxWidth;
		orthoProjectionMatrix[1][1] = 2.0f / boxHeight;
		orthoProjectionMatrix[2][2] = -2.0f / boxLength;
		orthoProjectionMatrix[3][3] = 1.0f;
		m_LightProjection = orthoProjectionMatrix;
	}

	void ShadowMapRenderer::UpdateLightViewMatrix(glm::vec3 lightDirection, glm::vec3 boxCenter)
	{
		m_LightView = glm::lookAt(lightDirection, boxCenter, glm::vec3(0.0, 1.0, 0.0));

	}

}