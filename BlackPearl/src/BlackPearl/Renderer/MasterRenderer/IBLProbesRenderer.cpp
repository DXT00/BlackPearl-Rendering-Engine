#include "pch.h"
#include "IBLProbesRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

namespace BlackPearl {
	IBLProbesRenderer::IBLProbesRenderer()
	{
		m_FrameBuffer.reset(DBG_NEW FrameBuffer());
		m_LightProbeShader.reset(DBG_NEW Shader("assets/shaders/lightProbes/lightProbe.glsl"));
	}
	IBLProbesRenderer::~IBLProbesRenderer()
	{
	}
	void IBLProbesRenderer::Init(Object* ProbesCamera, Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(brdfLUTQuadObj, "brdfLUTQuadObj is nullptr!");
		m_BrdfLUTQuadObj = brdfLUTQuadObj;


		m_IsInitial = true;

	}
	void IBLProbesRenderer::Render(Object* ProbesCamera, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");
	//	RenderSpecularBRDFLUTMap();

		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
		float zNear = cameraComponent->GetZnear();
		float zFar = cameraComponent->GetZfar();
		float fov = cameraComponent->GetFov();
		m_ProbeProjection = glm::perspective(glm::radians(fov), 1.0f, zNear, zFar);


		for (auto it = probes.begin(); it != probes.end(); it++)
		{
			LightProbe* probe = *it;
			glm::vec3 center = probe->GetPosition();
			m_ProbeView = {
			   glm::lookAt(center, glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

			};
			m_ProbeProjectionViews = {
			   m_ProbeProjection * m_ProbeView[0],
			   m_ProbeProjection * m_ProbeView[1],
			   m_ProbeProjection * m_ProbeView[2],
			   m_ProbeProjection * m_ProbeView[3],
			   m_ProbeProjection * m_ProbeView[4],
			   m_ProbeProjection * m_ProbeView[5]
			};
			RenderEnvironmerntCubeMaps(ProbesCamera, lightSources, objects, probe);
			RenderDiffuseIrradianceMap(ProbesCamera, lightSources, objects, probe);
				//RenderSpecularPrefilterMap(ProbesCamera, lightSources, objects, probe);
	
	



	/*		probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetDiffuseIrradianceCubeMap());
			probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe->GetCubeObj(), m_LightProbeShader);*/

		}


	//	m_FrameBuffer->UnBind();


	}

	void IBLProbesRenderer::RenderProbes(const std::vector<LightProbe*> probes)
	{
		for (auto it = probes.begin(); it != probes.end(); it++)
		{
			LightProbe* probe = *it;
	/*		glm::vec3 center = probe->GetPosition();
			m_ProbeView = {
			   glm::lookAt(center, glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			   glm::lookAt(center, glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

			};
			m_ProbeProjectionViews = {
			   m_ProbeProjection * m_ProbeView[0],
			   m_ProbeProjection * m_ProbeView[1],
			   m_ProbeProjection * m_ProbeView[2],
			   m_ProbeProjection * m_ProbeView[3],
			   m_ProbeProjection * m_ProbeView[4],
			   m_ProbeProjection * m_ProbeView[5]
			};
			RenderEnvironmerntCubeMaps(ProbesCamera, lightSources, objects, probe);
			RenderDiffuseIrradianceMap(ProbesCamera, lightSources, objects, probe);*/
			//RenderSpecularPrefilterMap(ProbesCamera, lightSources, objects, probe);
		//	probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetSpecularPrefilterCubeMap());
			//probe->GetHdrEnvironmentCubeMap()
			m_LightProbeShader->Bind();
			probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetDiffuseIrradianceCubeMap());
			//probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetHdrEnvironmentCubeMap());

			probe->GetCubeObj()->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe->GetCubeObj(), m_LightProbeShader);

		}

	}

	void IBLProbesRenderer::RenderEnvironmerntCubeMaps(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();

		std::shared_ptr<CubeMapTexture> environmentCubeMap = probe->GetHdrEnvironmentCubeMap();
		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::vec2 mipMapSize = { environmentCubeMap->GetWidth(),environmentCubeMap->GetHeight() };

		m_FrameBuffer->Bind();
		m_FrameBuffer->AttachCubeMapColorTexture(0, environmentCubeMap);
		//TODO::
		m_FrameBuffer->AttachRenderBuffer(environmentCubeMap->GetWidth(), environmentCubeMap->GetHeight());
		//for (unsigned int mip = 0; mip < environmentCubeMap->GetMipMapLevel(); mip++)
		//{

		m_FrameBuffer->BindRenderBuffer();
		//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipMapSize.x, mipMapSize.y);
		///	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_FrameBuffer->GetRenderBufferID());

		glViewport(0, 0, mipMapSize.x, mipMapSize.y);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Render the scene to environment cubemap (single pass).
		for (unsigned int i = 0; i < 6; i++)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ m_ProbeProjectionViews[i] ,m_ProbeView[i],m_ProbeProjection,probe->GetPosition(),cameraComponent->Front(),lightSources });

			//m_SpecularPrefilterShader->SetUniformMat4f("u_CubeMapProjectionView", captureProjectionViews[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//BasicRenderer::DrawLightSources(lightSources, scene);

			BasicRenderer::DrawObjects(objects, scene);

		}

		//	mipMapSize.x /= 2.0f;
		//mipMapSize.y /= 2.0f;

			//	}
		m_FrameBuffer->UnBind();

	}
	//void IBLProbesRenderer::RenderDiffuseIrradianceMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	//{
	//	auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
	//	auto diffuseIrradianceCubeMap = probe->GetDiffuseIrradianceCubeMap();
	//	//probe->GetHdrEnvironmentCubeMap()->Bind();
	//	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//	m_FrameBuffer->Bind();
	//	//m_FrameBuffer->AttachCubeMapColorTexture(0, diffuseIrradianceCubeMap);
	//	//m_FrameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetHeight());

	//	glBindRenderbuffer(GL_RENDERBUFFER, m_FrameBuffer->GetRenderBufferID());
	//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, probe->GetDiffuseCubeMapResolution(), probe->GetDiffuseCubeMapResolution());
	//	m_IrradianceShader->Bind();
	//	m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 0);
	//	glActiveTexture(GL_TEXTURE0);
	//	probe->GetHdrEnvironmentCubeMap()->Bind();

	//	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	//	glViewport(0, 0, probe->GetDiffuseCubeMapResolution(), probe->GetDiffuseCubeMapResolution()); // don't forget to configure the viewport to the capture dimensions.
	//	m_FrameBuffer->Bind();

	//	for (unsigned int i = 0; i < 6; ++i)
	//	{
	//		Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_ProbeProjectionViews[i] ,m_ProbeView[i],m_ProbeProjection,probe->GetPosition(),cameraComponent->Front(),lightSources };
	//		//TODO::
	//		//m_IrradianceShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);
	//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		DrawObject(probe->GetCubeObj(), m_IrradianceShader, scene);
	//	}
	//	m_FrameBuffer->UnBind();
	//}
	//
	void IBLProbesRenderer::RenderDiffuseIrradianceMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
		std::shared_ptr<CubeMapTexture> diffuseIrradianceCubeMap = probe->GetDiffuseIrradianceCubeMap();
		
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(),diffuseIrradianceCubeMap->GetHeight());

		glViewport(0, 0, diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetWidth()); // don't forget to configure the viewport to the capture dimensions.
		
		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		for (unsigned int i = 0; i < 6; ++i)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_ProbeProjectionViews[i] ,m_ProbeView[i],m_ProbeProjection,probe->GetPosition(),cameraComponent->Front(),lightSources };
		
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//remember bind shader before drawcall!!
			m_IrradianceShader->Bind();
			m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 2);
			glActiveTexture(GL_TEXTURE2);
			probe->GetHdrEnvironmentCubeMap()->Bind();

			DrawObject(probe->GetCubeObj(), m_IrradianceShader, scene);
		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
	}

	
	void IBLProbesRenderer::RenderSpecularPrefilterMap(Object* ProbesCamera, const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
		auto specularIrradianceMap = probe->GetSpecularPrefilterCubeMap();
		m_SpecularPrefilterShader->Bind();
		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);

		glActiveTexture(GL_TEXTURE0);
		probe->GetHdrEnvironmentCubeMap()->Bind();

		m_FrameBuffer->Bind();
		unsigned int maxMipMapLevels = 5;
		for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
		{
			//resize framebuffer according to mipmap-level size;
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			m_FrameBuffer->BindRenderBuffer();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipMapLevels - 1);
			m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);
			for (unsigned int i = 0; i < 6; i++)
			{
				//m_SpecularPrefilterShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_ProbeProjectionViews[i] ,m_ProbeView[i],m_ProbeProjection,probe->GetPosition(),cameraComponent->Front(),lightSources };

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, probe->GetSpecularPrefilterCubeMap()->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				DrawObject(probe->GetCubeObj(), m_SpecularPrefilterShader, scene);

			}
		}


		m_FrameBuffer->UnBind();

	}
	void IBLProbesRenderer::RenderSpecularBRDFLUTMap()
	{
		m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, 512, 512, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_FrameBuffer->Bind();
		m_FrameBuffer->BindRenderBuffer();
		m_FrameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);//±ØÐëattach 0 
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		glViewport(0, 0, 512, 512);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		m_FrameBuffer->UnBind();
	}
}