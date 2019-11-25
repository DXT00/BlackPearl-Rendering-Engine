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
	void ShadowMapRenderer::Render(const std::vector<Object*>& objs,  ParallelLight* sun, const std::vector<Object*>&exceptObjs)
	{
		glm::mat4 lightProjection, lightView;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,s_NearPlane,s_FarPlane);
		lightView = glm::lookAt(sun->GetDirection(), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		m_LightProjectionViewMatrix = lightProjection * lightView;
		m_LightPos = sun->GetDirection();

		glViewport(0, 0, s_ShadowMapWidth, s_ShadowMapHeight);
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);

		if (exceptObjs.empty())
			DrawObjects(objs);
		else
			DrawObjectsExcept(objs, exceptObjs);
		m_FrameBuffer->UnBind();

	}
	void ShadowMapRenderer::PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight ) //RenderScene 会调用该虚函数
	{
		PrepareBasicShaderParameters(mesh, transformMatrix,isLight);
		//std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
		//for (int i = 0; i < meshes.size(); i++) {
		//	// Set light uniforms
			std::shared_ptr<Shader> shader = mesh.GetMaterial()->GetShader();
			shader->SetUniformVec3f("u_LightPos", m_LightPos);
			shader->SetUniformMat4f("u_LightProjectionViewMatrix", m_LightProjectionViewMatrix);
		//}
	}

}