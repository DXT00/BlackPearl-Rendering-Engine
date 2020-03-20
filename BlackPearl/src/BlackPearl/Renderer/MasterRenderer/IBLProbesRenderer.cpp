#include "pch.h"
#include "BlackPearl/Core.h"
#include "IBLProbesRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/LightProbes/SphericalHarmonics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
namespace BlackPearl {
	IBLProbesRenderer::IBLProbesRenderer()
	{
		//	m_FrameBuffer.reset(DBG_NEW FrameBuffer());
		
		m_LightProbeShader.reset(DBG_NEW Shader("assets/shaders/lightProbes/lightProbe.glsl"));
		m_IBLShader.reset(DBG_NEW Shader("assets/shaders/lightProbes/iblSHTexture.glsl"));
		m_IrradianceShader.reset(DBG_NEW Shader("assets/shaders/ibl/irradianceConvolution.glsl"));
		m_SpecularPrefilterShader.reset(DBG_NEW Shader("assets/shaders/ibl/prefilterMap.glsl"));
		m_SpecularBRDFLutShader.reset(DBG_NEW Shader("assets/shaders/ibl/brdf.glsl"));
		m_PbrShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/pbr/PbrTexture.glsl"));
		m_NonPbrShader.reset(DBG_NEW BlackPearl::Shader("assets/shaders/IronMan.glsl"));
		//m_SHShader.reset(DBG_NEW Shader("assets/shaders/lightProbes/SH.glsl"));
	}
	IBLProbesRenderer::~IBLProbesRenderer()
	{
	}
	void IBLProbesRenderer::Init(Object* brdfLUTQuadObj, const LightSources& lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		GE_ASSERT(brdfLUTQuadObj, "brdfLUTQuadObj is nullptr!");
		m_BrdfLUTQuadObj = brdfLUTQuadObj;
		//m_SHQuadObj = SHQuadObj;
		RenderSpecularBRDFLUTMap();
		m_IsInitial = true;

	}

	void IBLProbesRenderer::UpdateProbesMaps(const LightSources* lightSources,  std::vector<Object*> objects, Object* skyBox, LightProbe* probe)
	{
		GE_CORE_INFO("calculating environment map...");
		RenderEnvironmerntCubeMaps(lightSources, objects, probe, skyBox);
		//RenderDiffuseIrradianceMap(lightSources, objects, probe);
		GE_CORE_INFO("calculating SH coeffs...");
		RenderSHImage(probe);
		GE_CORE_INFO("calculating specular map...");
		RenderSpecularPrefilterMap(lightSources, probe);
	}
	void IBLProbesRenderer::Render(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes, Object* skyBox)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");
		//RenderSpecularBRDFLUTMap();

		/*只渲染一次即可*/
	/*	if (!m_IsRenderSpecularBRDFLUTMap) {
			RenderSpecularBRDFLUTMap();
			m_IsRenderSpecularBRDFLUTMap = true;
		}*/
		
		//SphericalHarmonics::InitialCubeMapVector(probes[0]->GetHdrEnvironmentCubeMap()->GetWidth());
	
		for (auto it = probes.begin(); it != probes.end(); it++) {

			LightProbe* probe = *it;
			UpdateProbesMaps(lightSources, objects, skyBox, probe);

		}
		
		//if (!m_UpdateFinished) {
		//	//int id = m_CurrentProbeIndex;
		//	for (int i = 0; i < m_KperFrame; i++)
		//	{
		//		UpdateProbesMaps(lightSources, objects, skyBox, probes[m_CurrentProbeIndex++]);
		//		if (m_CurrentProbeIndex >= probes.size()) {
		//			m_UpdateFinished = true;
		//			m_CurrentProbeIndex = 0;
		//			break;
		//		}

		//	}

		//}


		
		

	}

	void IBLProbesRenderer::RenderProbes(const std::vector<LightProbe*> probes)
	{
		for (auto it = probes.begin(); it != probes.end(); it++)
		{
			LightProbe* probe = *it;

			m_LightProbeShader->Bind();
		/*	m_LightProbeShader->SetUniform1i("u_Image", 0);
			probe->GetSHImage()->Bind(0);*/
			
			for (int i = 0; i < 9; i++)
			{
				m_LightProbeShader->SetUniformVec3f("u_SHCoeffs["+std::to_string(i)+"]", glm::vec3(probe->GetCoeffis()[i][0], probe->GetCoeffis()[i][1],probe->GetCoeffis()[i][2]));
			}




			//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetSpecularPrefilterCubeMap());
			//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(m_SpecularBrdfLUTTexture);

			//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetDiffuseIrradianceCubeMap());
			probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetHdrEnvironmentCubeMap());

			probe->GetObj()->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe->GetObj(), m_LightProbeShader);
			//probe->GetSpecularPrefilterCubeMap()->UnBind();
			//probe->GetSHImage()->UnBind();
		}

	}

	void IBLProbesRenderer::RenderSpecularObjects(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<LightProbe*> probes)
	{
		 

		for (Object* obj : objects) {

			/* render specular Map for each objects*/




			glm::vec3 pos = obj->GetComponent<Transform>()->GetPosition();

			std::vector<LightProbe*> kProbes = FindKnearProbes(pos, probes);

			unsigned int k = m_K;
			std::vector<float> distances;
			float distancesSum = 0.0;
			for (auto probe : kProbes)
			{
				distances.push_back(glm::length(probe->GetPosition() - pos));
				distancesSum += glm::length(probe->GetPosition() - pos);
			}
			m_IBLShader->Bind();
			m_IBLShader->SetUniform1i("u_Kprobes", k);
			unsigned int textureK = 0;
			m_IBLShader->SetUniform1i("u_BrdfLUTMap", textureK);
			glActiveTexture(GL_TEXTURE0 + textureK);
			m_SpecularBrdfLUTTexture->Bind();
			textureK++;
			for (int i = 0; i < k; i++)
			{
				m_IBLShader->SetUniform1f("u_ProbeWeight[" + std::to_string(i) + "]", (float)distances[i] / distancesSum);

				//m_IBLShader->SetUniform1i("u_IrradianceMap[" + std::to_string(i) + "]", textureK);
				//m_IBLShader->SetUniform1i("u_Image", textureK);
				for (int sh = 0; sh < 9; sh++)
				{
					int index = sh + 9 * i;
					m_IBLShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(index) + "]", glm::vec3(kProbes[i]->GetCoeffis()[sh][0], kProbes[i]->GetCoeffis()[sh][1], kProbes[i]->GetCoeffis()[sh][2]));
				}

				//glActiveTexture(GL_TEXTURE0 + textureK);
				//kProbes[i]->GetDiffuseIrradianceCubeMap()->Bind();
				//kProbes[i]->GetSHImage()->Bind(textureK);
				//textureK++;
				m_IBLShader->SetUniform1i("u_PrefilterMap[" + std::to_string(i) + "]", textureK);
				glActiveTexture(GL_TEXTURE0 + textureK);
				kProbes[i]->GetSpecularPrefilterCubeMap()->Bind();
				textureK++;

			}

			obj->GetComponent<MeshRenderer>()->SetShaders(m_IBLShader);
			DrawObject(obj, m_IBLShader);

			/*m_SpecularBrdfLUTTexture->UnBind();
			for (int i = 0; i < k; i++)
				kProbes[i]->GetSpecularPrefilterCubeMap()->UnBind();*/


		}

	}

	void IBLProbesRenderer::RenderEnvironmerntCubeMaps(const LightSources* lightSources, std::vector<Object*> objects, LightProbe* probe, Object* skyBox)
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
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->AttachCubeMapColorTexture(0, environmentCubeMap);
		//TODO::
		frameBuffer->AttachRenderBuffer(environmentCubeMap->GetWidth(), environmentCubeMap->GetHeight());
		for (unsigned int mip = 0; mip < environmentCubeMap->GetMipMapLevel(); mip++)
		{

			frameBuffer->BindRenderBuffer();
			/*glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipMapSize.x, mipMapSize.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer->GetRenderBufferID());*/

			glViewport(0, 0, mipMapSize.x, mipMapSize.y);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene to environment cubemap (single pass).


			for (unsigned int i = 0; i < 6; i++)
			{
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),{},cameraComponent->Front(),*lightSources });

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentCubeMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//BasicRenderer::DrawLightSources(lightSources, scene);
				glDepthFunc(GL_LEQUAL);
				//DrawObject(skyBox, scene);
				glDepthFunc(GL_LESS);

				for (auto obj : objects) {
					if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
						m_PbrShader->Bind();
						DrawObject(obj,m_PbrShader,scene);

					}
					else {
						m_NonPbrShader->Bind();
						DrawObject(obj, m_NonPbrShader, scene);

					}

				}
				delete scene;
				scene = nullptr;

			}

			mipMapSize.x /= 2.0f;
			mipMapSize.y /= 2.0f;

		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


	}

	void IBLProbesRenderer::RenderDiffuseIrradianceMap(const LightSources* lightSources, std::vector<Object*> objects, LightProbe* probe)
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
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetHeight());

		glViewport(0, 0, diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetWidth()); // don't forget to configure the viewport to the capture dimensions.
		frameBuffer->Bind();

		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
		for (unsigned int i = 0; i < 6; ++i)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),{},cameraComponent->Front(),*lightSources });

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//remember bind shader before drawcall!!
			m_IrradianceShader->Bind();
			m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 2);
			glActiveTexture(GL_TEXTURE2);
			probe->GetHdrEnvironmentCubeMap()->Bind();

			DrawObject(probe->GetObj(), m_IrradianceShader, scene);
			probe->GetHdrEnvironmentCubeMap()->UnBind();

			delete scene;
			scene = nullptr;

		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
	}


	void IBLProbesRenderer::RenderSpecularPrefilterMap(const LightSources* lightSources, LightProbe* probe)
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
		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 3);
		glActiveTexture(GL_TEXTURE3);
		probe->GetHdrEnvironmentCubeMap()->Bind();

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());


		frameBuffer->AttachCubeMapColorTexture(0, specularIrradianceMap);
		frameBuffer->AttachRenderBuffer(specularIrradianceMap->GetWidth(), specularIrradianceMap->GetHeight());
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),{},cameraComponent->Front(),*lightSources });
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularIrradianceMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				probe->GetObj()->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);

				DrawObject(probe->GetObj(), m_SpecularPrefilterShader, scene);
				delete scene;
				scene = nullptr;

			}
		}
		probe->GetHdrEnvironmentCubeMap()->UnBind();

		frameBuffer->UnBind();
		frameBuffer->CleanUp();

	}
	void IBLProbesRenderer::RenderSpecularBRDFLUTMap()
	{
		m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution);

		//m_FrameBuffer->Bind();
		//m_FrameBuffer->BindRenderBuffer();
		frameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);
		frameBuffer->BindRenderBuffer();
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		glViewport(0, 0, Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


	}

	void IBLProbesRenderer::RenderSHImage(LightProbe* probe)
	{

		std::shared_ptr<CubeMapTexture> environmentMap = probe->GetHdrEnvironmentCubeMap();
		auto coeffs = SphericalHarmonics::UpdateCoeffs(environmentMap);
		probe->SetSHCoeffs(coeffs);
	/*		m_SHShader->Bind();
			m_SHShader->SetUniform1f("u_FaceIdx", i);
			m_SHShader->SetUniform1f("u_MapSize", probe->GetHdrEnvironmentCubeMap()->GetWidth());
			m_SHShader->SetUniform1i("u_EnvironmentMap", 0);
			m_SHShader->SetUniform1i("u_image", 1);
			*/

			//glActiveTexture(GL_TEXTURE0);
			//probe->GetHdrEnvironmentCubeMap()->Bind();
			////glActiveTexture(GL_TEXTURE1);
			//probe->GetSHImage()->Bind(1);
			////glViewport(0, 0, 512, 512);
			//DrawObject(m_SHQuadObj, m_SHShader);
			//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);

		//for (int i = 0; i < 6; i++) {
		//	m_SHShader->Bind();
		//	m_SHShader->SetUniform1f("u_FaceIdx", i);
		//	m_SHShader->SetUniform1f("u_MapSize", probe->GetHdrEnvironmentCubeMap()->GetWidth());
		//	m_SHShader->SetUniform1i("u_EnvironmentMap", 0);
		//	m_SHShader->SetUniform1i("u_image", 1);
		//	

		//	glActiveTexture(GL_TEXTURE0);
		//	probe->GetHdrEnvironmentCubeMap()->Bind();
		//	//glActiveTexture(GL_TEXTURE1);
		//	probe->GetSHImage()->Bind(1);
		//	//glViewport(0, 0, 512, 512);
		//	DrawObject(m_SHQuadObj, m_SHShader);
		//	//glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);



		//}
		

	}


	std::vector<LightProbe*> IBLProbesRenderer::FindKnearProbes(glm::vec3 objPos, std::vector<LightProbe*> probes)
	{

		glm::vec3 pos = objPos;
		std::vector<LightProbe*> kProbes;
		std::sort(probes.begin(), probes.end(), [=](LightProbe* pa, LightProbe* pb)
			{return glm::length(pa->GetPosition() - pos) < glm::length(pb->GetPosition() - pos); });

		GE_ASSERT(m_K <= (int)probes.size(), "m_K larger than probes' number!");

		for (int i = 0; i < m_K; i++)
		{
			kProbes.push_back(probes[i]);
		}


		return kProbes;
	}
	
}