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
	void IBLProbesRenderer::Init(Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(brdfLUTQuadObj, "brdfLUTQuadObj is nullptr!");
		m_BrdfLUTQuadObj = brdfLUTQuadObj;


		m_IsInitial = true;

	}
	void IBLProbesRenderer::Render(const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");
		//	RenderSpecularBRDFLUTMap();

			//auto cameraComponent = ProbesCamera->GetComponent<PerspectiveCamera>();
			//float zNear = cameraComponent->GetZnear();
			//float zFar = 10.0f;// cameraComponent->GetZfar();
			//float fov = cameraComponent->GetFov();
			//m_ProbeProjection = glm::perspective(glm::radians(fov), 1.0f, zNear, zFar);


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
			RenderEnvironmerntCubeMaps(lightSources, objects, probe);
			RenderDiffuseIrradianceMap(lightSources, objects, probe);
			RenderSpecularPrefilterMap(lightSources, objects, probe);





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

			m_LightProbeShader->Bind();
			probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetSpecularPrefilterCubeMap());

			//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetDiffuseIrradianceCubeMap());
			//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetHdrEnvironmentCubeMap());

			probe->GetObj()->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe->GetObj(), m_LightProbeShader);

		}

	}

	void IBLProbesRenderer::RenderEnvironmerntCubeMaps(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		//	cameraComponent->SetPosition(probe->GetPosition());

		std::vector<glm::mat4> ProbeView = {
		   glm::lookAt(center, center - camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
		   glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()

		   glm::lookAt(center, center - camera->Right(), -camera->Up()),
		   glm::lookAt(center, center + camera->Right(), -camera->Up()),

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};






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
			//cameraComponent->SwitchToFace(i);
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });
			//Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ m_ProbeProjectionViews[i] ,m_ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });

			
			/*Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({
				cameraComponent->GetViewProjectionMatrix(),
				cameraComponent->GetViewMatrix(),
				cameraComponent->GetProjectionMatrix(),
				cameraComponent->GetPosition(),
				cameraComponent->Front(),
				lightSources });*/

				//m_SpecularPrefilterShader->SetUniformMat4f("u_CubeMapProjectionView", captureProjectionViews[i]);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//BasicRenderer::DrawLightSources(lightSources, scene);

			BasicRenderer::DrawObjects(objects, scene);
			delete scene;
			scene = nullptr;

		}

		//	mipMapSize.x /= 2.0f;
		//mipMapSize.y /= 2.0f;

			//	}
		m_FrameBuffer->UnBind();

	}

	void IBLProbesRenderer::RenderDiffuseIrradianceMap(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, probe->GetHdrEnvironmentCubeMap()->GetRendererID());
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		
		glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();

		std::vector<glm::mat4> ProbeView = {
		   glm::lookAt(center, center - camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Front(),-camera->Up()),
		   glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
		   glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()
		   glm::lookAt(center, center - camera->Right(), -camera->Up()),
		   glm::lookAt(center, center + camera->Right(), -camera->Up()),

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};

		std::shared_ptr<CubeMapTexture> diffuseIrradianceCubeMap = probe->GetDiffuseIrradianceCubeMap();

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetHeight());

		glViewport(0, 0, diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetWidth()); // don't forget to configure the viewport to the capture dimensions.
		frameBuffer->Bind();

		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		for (unsigned int i = 0; i < 6; ++i)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//remember bind shader before drawcall!!
			m_IrradianceShader->Bind();
			m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 2);
			glActiveTexture(GL_TEXTURE2);
			probe->GetHdrEnvironmentCubeMap()->Bind();

			DrawObject(probe->GetObj(), m_IrradianceShader, scene);
			delete scene;
			scene = nullptr;

		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
	}


	void IBLProbesRenderer::RenderSpecularPrefilterMap(const LightSources& lightSources, std::vector<Object*> objects, LightProbe* probe)
	{
		glm::vec3 center = probe->GetPosition();
		probe->UpdateCamera();
		auto camera = probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		std::vector<glm::mat4> ProbeView = {
			glm::lookAt(center, center - camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
			glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()
			glm::lookAt(center, center - camera->Right(), -camera->Up()),
			glm::lookAt(center, center + camera->Right(), -camera->Up())

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};
		auto specularIrradianceMap = probe->GetSpecularPrefilterCubeMap();
		m_SpecularPrefilterShader->Bind();
		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		probe->GetHdrEnvironmentCubeMap()->Bind();

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());

		
		frameBuffer->AttachCubeMapColorTexture(0, specularIrradianceMap);
		frameBuffer->AttachRenderBuffer(specularIrradianceMap->GetWidth(), specularIrradianceMap->GetHeight());
		frameBuffer->Bind();
		unsigned int maxMipMapLevels = 5;
		for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
		{
			//resize framebuffer according to mipmap-level size;
			unsigned int mipWidth = specularIrradianceMap->GetWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = specularIrradianceMap->GetHeight() * std::pow(0.5, mip);
			frameBuffer->BindRenderBuffer();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipMapLevels - 1);
			m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);


			for (unsigned int i = 0; i < 6; i++)
			{
				
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),cameraComponent->Front(),lightSources });
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularIrradianceMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				probe->GetObj()->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);

				DrawObject(probe->GetObj(), m_SpecularPrefilterShader, scene);
				delete scene;
				scene = nullptr;

			}
		}


		frameBuffer->UnBind();
		frameBuffer->CleanUp();

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