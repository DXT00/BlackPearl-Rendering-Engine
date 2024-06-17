#include "pch.h"
#include "CloudRenderer.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Renderer/Material/Texture3D.h"
#include "BlackPearl/Core.h"

namespace BlackPearl {

	float CloudRenderer::s_rayStep = 0.1;
	float CloudRenderer::s_step = 0.1;
	glm::vec3 CloudRenderer::s_colA = glm::vec3(1.0, 0.64274913, 0.3443396);
	glm::vec3 CloudRenderer::s_colB = glm::vec3(0.4717871, 0.54310095, 0.5849056);
	float CloudRenderer::s_colorOffset1= 1.0;
	float CloudRenderer::s_colorOffset2 = 1.0;
	float CloudRenderer::s_NoiseTexture3DSize = 128.0;
	float CloudRenderer::s_densityOffset = 0.0;
	glm::vec3 CloudRenderer::s_boundsMin = glm::vec3(-10.0, 20.0, -10.0);// glm::vec3(-10.0);
	glm::vec3 CloudRenderer::s_boundsMax = glm::vec3(23.0,23.0,23.0);//glm::vec3(10.0);
	float CloudRenderer::s_lightAbsorptionTowardSun = 1.0;
	float CloudRenderer::s_densityMultiplier = 53.0;
	CloudRenderer::CloudRenderer()
	{

		m_CloudShader.reset(DBG_NEW Shader("assets/shaders/cloud/cloudRayMarching.glsl"));
		m_DepthShader.reset(DBG_NEW Shader("assets/shaders/occlusionCulling/depthMap.glsl"));
		m_NoiseGenShader.reset(DBG_NEW Shader("assets/shaders/cloud/noiseGenerate3D.glsl"));
		m_NoiseDebugShader.reset(DBG_NEW Shader("assets/shaders/cloud/noiseDebug3D.glsl"));
		m_SceneRenderShader.reset(DBG_NEW Shader("assets/shaders/cloud/sceneRender.glsl"));

		const std::vector<GLfloat> texture3D(4 * s_NoiseTexture3DSize * s_NoiseTexture3DSize * s_NoiseTexture3DSize, 0.0f);
		m_NoiseTexture = DBG_NEW Texture3D(texture3D, s_NoiseTexture3DSize, s_NoiseTexture3DSize, s_NoiseTexture3DSize, true);

		m_WeatherTexture.reset(DBG_NEW Texture(BlackPearl::Texture::Type::HeightMap, "assets/texture/weather3.png"));
	}


	void CloudRenderer::Init(Scene* scene)
	{
		m_Scene = scene;

		m_CloudFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_CloudTexture.reset(DBG_NEW BlackPearl::Texture(BlackPearl::Texture::Type::None, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_CloudFrameBuffer->Bind();
		m_CloudFrameBuffer->AttachRenderBuffer(BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
		m_CloudFrameBuffer->AttachColorTexture(m_CloudTexture, 0);
		m_CloudFrameBuffer->BindRenderBuffer();
		m_CloudFrameBuffer->UnBind();

		m_DepthFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_DepthTexture.reset(DBG_NEW Texture(Texture::Type::DepthMap, Configuration::WindowWidth, Configuration::WindowHeight, true/*isDepth*/, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST, GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_CLAMP_TO_EDGE, GL_FLOAT, true/*genmipmap*/));
		m_DepthFrameBuffer->Bind();
		m_DepthFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		glDrawBuffer(GL_NONE); // No color buffer is drawn to.
		glReadBuffer(GL_NONE);
		m_DepthFrameBuffer->UnBind();

		_CreateNoise3DTexture();


	}

	void CloudRenderer::Render(MainCamera* mainCamera,Object* obj, Object* BoundingBoxObj, const std::shared_ptr<Texture> postProcessTexture)
	{
		//m_CloudFrameBuffer->Bind();

		m_CloudShader->Bind();
		m_CloudShader->SetUniform1i("u_CameraDepthTexture", 4);
		m_CloudShader->SetUniform1i("u_FinalScreenTexture", 5);
		m_CloudShader->SetUniform1i("u_Texture3DNoise", 6);
		m_CloudShader->SetUniform1i("m_WeatherTexture", 7);

		m_CloudShader->SetUniform1f("u_TextureScale", s_NoiseTexture3DSize);

		m_CloudShader->SetUniform1f("u_rayStep", s_rayStep);
		m_CloudShader->SetUniform1f("u_step", s_step);
		m_CloudShader->SetUniformVec3f("u_colA", s_colA);
		m_CloudShader->SetUniformVec3f("u_colB", s_colB);
		m_CloudShader->SetUniform1f("u_colorOffset1", s_colorOffset1);
		m_CloudShader->SetUniform1f("u_colorOffset2", s_colorOffset2);
		m_CloudShader->SetUniform1f("u_densityOffset", s_densityOffset);

		/*if (BoundingBoxObj->HasComponent<BoundingBox>()) {
			m_CloudShader->SetUniformVec3f("u_boundsMax", BoundingBoxObj->GetComponent<BoundingBox>()->Get().GetMinP());
			m_CloudShader->SetUniformVec3f("u_boundsMin", BoundingBoxObj->GetComponent<BoundingBox>()->Get().GetMaxP());
		}
		else */
		{
			m_CloudShader->SetUniformVec3f("u_boundsMax", s_boundsMax);
			m_CloudShader->SetUniformVec3f("u_boundsMin", s_boundsMin);
		}

		m_CloudShader->SetUniform1f("u_lightAbsorptionTowardSun", s_lightAbsorptionTowardSun);
		m_CloudShader->SetUniform1f("u_densityMultiplier", s_densityMultiplier);

		m_CloudShader->SetUniformMat4f("u_InverseProjectionMatrix", glm::inverse(mainCamera->GetObj()->GetComponent<PerspectiveCamera>()->GetProjectionMatrix()));
		m_CloudShader->SetUniformMat4f("u_InverseViewMatrix", glm::inverse(mainCamera->GetObj()->GetComponent<PerspectiveCamera>()->GetViewMatrix()));

		glActiveTexture(GL_TEXTURE4);
		m_DepthTexture->Bind();

		glActiveTexture(GL_TEXTURE5);
		postProcessTexture->Bind();

		glActiveTexture(GL_TEXTURE6);
		m_NoiseTexture->Bind();

		glActiveTexture(GL_TEXTURE7);
		m_WeatherTexture->Bind();
		//glBindImageTexture(0, m_NoiseTexture->GetRendererID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);

		DrawObject(obj, m_CloudShader);

		m_CloudShader->Unbind();

		//m_CloudFrameBuffer->UnBind();
	}

	void CloudRenderer::RenderCombineScene(MainCamera* mainCamera, Object* obj, const std::shared_ptr<Texture> postProcessTexture)
	{
		m_SceneRenderShader->Bind();
		m_SceneRenderShader->SetUniform1i("u_CloudTexture", 8);
		m_SceneRenderShader->SetUniform1i("u_FinalScreenTexture", 9);
		glActiveTexture(GL_TEXTURE8);
		m_CloudTexture->Bind();

		glActiveTexture(GL_TEXTURE9);
		postProcessTexture->Bind();
		DrawObject(obj, m_SceneRenderShader);
		m_SceneRenderShader->Unbind();
	}

	void CloudRenderer::RenderScene(MainCamera* mainCamera, Object* obj, const std::shared_ptr<Texture> postProcessTexture)
	{
		m_SceneRenderShader->Bind();
		m_SceneRenderShader->SetUniform1i("u_CloudTexture", 8);
		m_SceneRenderShader->SetUniform1i("u_FinalScreenTexture", 9);
		glActiveTexture(GL_TEXTURE8);
		m_CloudTexture->Bind();

		glActiveTexture(GL_TEXTURE9);
		postProcessTexture->Bind();
		DrawObject(obj, m_SceneRenderShader);
		m_SceneRenderShader->Unbind();

	}

	void CloudRenderer::RenderDepthMap()
	{
		m_DepthShader->Bind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_DepthFrameBuffer->Bind();
		glClear(GL_DEPTH_BUFFER_BIT);
		//m_HizFrameBuffer->AttachDepthTexture(m_DepthTexture, 0);
		//render scene to depth buffer
		for (size_t i = 0; i < m_Scene->GetSingleNodes().size(); i++)
		{
			auto obj = dynamic_cast<SingleNode*>(m_Scene->GetSingleNodes(i))->GetObj();
			DrawObject(obj, m_DepthShader);
		}

		m_DepthFrameBuffer->UnBind();

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	CloudRenderer::~CloudRenderer()
	{
	}

	void CloudRenderer::_CreateNoise3DTexture()
	{
		{
			float clearColor[4] = { 0.0,0.0,0.0,0.0 };//-1.0
			m_NoiseTexture->Clear(clearColor);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, s_NoiseTexture3DSize, s_NoiseTexture3DSize);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_NoiseGenShader->Bind();
		GE_ERROR_JUDGE();

		m_NoiseGenShader->SetUniform1f("u_Tex3D2Res", s_NoiseTexture3DSize);
		GE_ERROR_JUDGE();
		
		glBindImageTexture(0, m_NoiseTexture->GetRendererID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		GE_ERROR_JUDGE();

		/*m_NoiseGenShader->SetUniform1i("u_IsPBRObjects", 0);
		GE_ERROR_JUDGE();*/
		DiscpatchCompute(32, 32, 32);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//DrawObject(skybox, m_VoxelizationShader);
		m_NoiseGenShader->Unbind();
	}

	void CloudRenderer::ShowNoise3DTexture(Object* cube) {

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_NoiseDebugShader->Bind();
		m_NoiseDebugShader->SetUniform1i("u_Texture3DNoise", 4);
		glActiveTexture(GL_TEXTURE4);
		m_NoiseTexture->Bind();

		DrawObject(cube, m_NoiseDebugShader);


		m_NoiseDebugShader->Unbind();
	}


}
