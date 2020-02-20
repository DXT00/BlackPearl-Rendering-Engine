#include "pch.h"
#include "ShadowMapPointLightRenderer.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {
	int ShadowMapPointLightRenderer::s_ShadowMapPointLightWidth = 1024;
	int ShadowMapPointLightRenderer::s_ShadowMapPointLightHeight = 1024;
	float ShadowMapPointLightRenderer::s_FarPlane = 60.0f;
	float ShadowMapPointLightRenderer::s_FOV = 90.0f;
	//NearPlane 不可改必须保证是1 ，shader中要转换到[0,1]坐标系
	//CubeMapDepthShader.glsl
	const float ShadowMapPointLightRenderer::s_NearPlane = 1.0f;

	ShadowMapPointLightRenderer::~ShadowMapPointLightRenderer()
	{
	}
	void ShadowMapPointLightRenderer::Render(const std::vector<Object*>& objs, Object* pointLight,const std::vector<Object*>& exceptObjs)
	{
		float aspect = (float)s_ShadowMapPointLightWidth / (float)s_ShadowMapPointLightHeight;
		glm::mat4 pointLightProjection = glm::perspective(glm::radians(s_FOV), aspect, s_NearPlane, s_FarPlane);
		
		GE_ASSERT(pointLight->HasComponent<PointLight>(), "this object is not a pointlight!");
		m_LightPos = pointLight->GetComponent<Transform>()->GetPosition();
		
		m_LightProjectionViewMatries[0]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[1]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[2]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 1.0f, 0.0), glm::vec3(0.0, 0.0, 1.0f)));
		m_LightProjectionViewMatries[3]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, -1.0f, 0.0), glm::vec3(0.0, 0.0, -1.0f)));
		m_LightProjectionViewMatries[4]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0, -1.0f, 0.0)));
		m_LightProjectionViewMatries[5]=(pointLightProjection * glm::lookAt(m_LightPos, m_LightPos + glm::vec3(0.0, 0.0, -1.0f), glm::vec3(0.0, -1.0f, 0.0)));
		
		glViewport(0, 0, s_ShadowMapPointLightWidth, s_ShadowMapPointLightHeight);
		m_FrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		if (exceptObjs.empty())
			DrawObjects(objs);
		else
			DrawObjectsExcept(objs, exceptObjs);
		m_FrameBuffer->UnBind();

	}
	void ShadowMapPointLightRenderer::PrepareShaderParameters(Mesh mesh, glm::mat4 transformMatrix,std::shared_ptr<Shader>shader, bool isLight)
	{
		PrepareBasicShaderParameters(mesh, transformMatrix, shader,isLight);
		//std::shared_ptr<Shader> shader = mesh.GetMaterial()->GetShader();
		for (int i = 0; i < 6; i++)
			shader->SetUniformMat4f("shadowMatrices[" + std::to_string(i) + "]", m_LightProjectionViewMatries[i]);
		shader->SetUniform1f("u_FarPlane", s_FarPlane);
		shader->SetUniformVec3f("u_LightPos", m_LightPos);

	}
}

