#include "pch.h"
#include "BlackPearl/Core.h"
#include "IBLProbesRenderer.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Component/CameraComponent/PerspectiveCamera.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Timestep/TimeCounter.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/LightProbeComponent/LightProbeComponent.h"
#include "BlackPearl/LightProbes/SphericalHarmonics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <thread>
namespace BlackPearl {
	glm::vec3 IBLProbesRenderer::s_ProbeGridOffset = glm::vec3(2.0f,10.0f,6.7f);
	float IBLProbesRenderer::s_ProbeGridSpace = 5.0f;
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
	
	void IBLProbesRenderer::Init(MainCamera* camera, Object* brdfLUTQuadObj, const LightSources& lightSources,const std::vector<Object*> objects)
	{
		GE_ASSERT(brdfLUTQuadObj, "brdfLUTQuadObj is nullptr!");

		//多个probe共用一个camera
		m_ProbeCamera = camera;
		m_ProbeCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetFov(90.0f);
		m_ProbeCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetWidth(Configuration::EnvironmantMapResolution);
		m_ProbeCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetHeight(Configuration::EnvironmantMapResolution);
		m_ProbeCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()->SetZfar(13.0f);

		m_BrdfLUTQuadObj = brdfLUTQuadObj;
		//m_SHQuadObj = SHQuadObj;
		RenderSpecularBRDFLUTMap();
		
		m_AnimatedModelRenderer = DBG_NEW AnimatedModelRenderer();
	//	std::shared_ptr<Shader> gBufferAnimatedShader(DBG_NEW Shader("assets/shaders/animatedModel/animatedGBufferModel.glsl"));
		//m_AnimatedModelRenderer->SetShader(gBufferAnimatedShader);
		m_SkyboxRenderer = DBG_NEW SkyboxRenderer();

		m_IsInitial = true;

	}

	//void IBLProbesRenderer::UpdateProbesMaps(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox, LightProbe* probe)
	//{
	//	GE_CORE_INFO("calculating environment map...");
	//	RenderEnvironmerntCubeMaps(lightSources, objects, probe, skyBox);
	//	//RenderDiffuseIrradianceMap(lightSources, objects, probe);
	//	GE_CORE_INFO("calculating SH coeffs...");
	//	RenderSHImage(probe);
	//	GE_CORE_INFO("calculating specular map...");
	//	RenderSpecularPrefilterMap(lightSources, probe);
	//}
	void IBLProbesRenderer::UpdateDiffuseProbesMap(const LightSources* lightSources, std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, Object* skyBox, Object* diffuseProbe)
	{
		std::shared_ptr<CubeMapTexture> environmentMap = RenderEnvironmerntCubeMaps(lightSources, objects, dynamicObjs,timeInSecond,diffuseProbe, skyBox);
		//GE_CORE_INFO("calculating SH coeffs...");
		RenderSHImage(diffuseProbe, environmentMap);
		//GE_CORE_INFO("finished");

	}
	void IBLProbesRenderer::UpdateReflectionProbesMap(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, Object* skyBox, Object* reflectionProbe)
	{
		if (reflectionProbe->GetComponent<LightProbe>()->GetDynamicSpecularMap()) {
			std::shared_ptr<CubeMapTexture> environmentMap =RenderEnvironmerntCubeMaps(lightSources, objects, dynamicObjs,timeInSecond,reflectionProbe,skyBox);
			//GE_CORE_INFO("calculating specular map...");
			RenderSpecularPrefilterMap(lightSources, reflectionProbe, environmentMap);
			//GE_CORE_INFO("finished");
		}
	

	}
	void IBLProbesRenderer::Render(const LightSources* lightSources, const std::vector<Object*> objects, 
		const std::vector<Object*> dynamicObjs,
		float timeInSecond,
		const std::vector<Object*> diffuseProbes,
		const std::vector<Object*> reflectionProbes, Object* skyBox)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");

		for (auto it = diffuseProbes.begin(); it != diffuseProbes.end(); it++) {

			Object* probe = *it;
			UpdateDiffuseProbesMap(lightSources, objects, dynamicObjs, timeInSecond,skyBox, probe);

		}

		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {

			Object* probe = *it;
			UpdateReflectionProbesMap(lightSources, objects, dynamicObjs, timeInSecond, skyBox, probe);

		}

	}
	void IBLProbesRenderer::RenderDiffuseProbeMap(int idx,const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, const std::vector<Object*> diffuseProbes, Object* skyBox)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");

		UpdateDiffuseProbesMap(lightSources, objects, dynamicObjs, timeInSecond, skyBox, diffuseProbes[idx]);

	}

	void IBLProbesRenderer::RenderDiffuseProbeMap(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, const std::vector<Object*> diffuseProbes,  Object* skyBox)
	{
		GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");

		for (auto it = diffuseProbes.begin(); it != diffuseProbes.end(); it++) {

			Object* probe = *it;
			UpdateDiffuseProbesMap(lightSources, objects, dynamicObjs, timeInSecond, skyBox, probe);

		}
	}
	void IBLProbesRenderer::RenderSpecularProbeMap(bool sceneChanged ,const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, const std::vector<Object*> reflectionProbes, Object* skyBox)
	{
		//场景中的物体移动或者light变化时才更新 reflectionProbes!
		if (sceneChanged) {
			for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {

				Object* probe = *it;
				UpdateReflectionProbesMap(lightSources, objects, dynamicObjs, timeInSecond, skyBox, probe);

			}
		}

	}

	void IBLProbesRenderer::RenderSpecularProbeMap(const LightSources* lightSources, const std::vector<Object*> objects, const std::vector<Object*> dynamicObjs, float timeInSecond, const std::vector<Object*> reflectionProbes, Object* skyBox)
	{
		//场景中的物体移动或者light变化时才更新 reflectionProbes!
		//if (sceneChanged) {
		for (auto it = reflectionProbes.begin(); it != reflectionProbes.end(); it++) {

			Object* probe = *it;
			UpdateReflectionProbesMap(lightSources, objects, dynamicObjs, timeInSecond, skyBox, probe);

		}
		//}

	}
	//probeType:0--diffuseProbe ,1--reflectionProbe
	void IBLProbesRenderer::RenderProbes(const std::vector<Object*> probes,int probeType)
	{
		for (auto it = probes.begin(); it != probes.end(); it++)
		{
			Object* probe = *it;

			m_LightProbeShader->Bind();
			/*	m_LightProbeShader->SetUniform1i("u_Image", 0);
				probe->GetSHImage()->Bind(0);*/

		




			//	probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetSpecularPrefilterCubeMap());
				//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(m_SpecularBrdfLUTTexture);

				//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetDiffuseIrradianceCubeMap());
			
			
			if (probeType == 0) //diffuse probe
			{
				//probe->GetObj()->GetComponent<MeshRenderer>()->SetTextures(probe->GetHdrEnvironmentCubeMap());
				m_LightProbeShader->SetUniform1i("u_ProbeType", probeType);
				for (int i = 0; i < 9; i++)
				{
					m_LightProbeShader->SetUniformVec3f("u_SHCoeffs[" + std::to_string(i) + "]", glm::vec3(
						probe->GetComponent<LightProbe>()->GetCoeffis()[i][0], 
						probe->GetComponent<LightProbe>()->GetCoeffis()[i][1],
						probe->GetComponent<LightProbe>()->GetCoeffis()[i][2]
					));
				}

			}
			else {
				probe->GetComponent<MeshRenderer>()->SetTextures(probe->GetComponent<LightProbe>()->GetSpecularPrefilterCubeMap());
				m_LightProbeShader->SetUniform1i("u_ProbeType", probeType);
			}

			probe->GetComponent<MeshRenderer>()->SetShaders(m_LightProbeShader);
			DrawObject(probe, m_LightProbeShader);
			//probe->GetSpecularPrefilterCubeMap()->UnBind();
			//probe->GetSHImage()->UnBind();
		}

	}



	void IBLProbesRenderer::UpdateProbeCamera(Object* probe)
	{
		glm::vec3 objPos = probe->GetComponent<Transform>()->GetPosition();
		glm::vec3 objRot = probe->GetComponent<Transform>()->GetRotation();
		m_ProbeCamera->SetPosition(objPos);
		m_ProbeCamera->SetRotation(objRot);
		m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetZfar(probe->GetComponent<LightProbe>()->GetZfar());

	}

	std::shared_ptr<CubeMapTexture> IBLProbesRenderer::RenderEnvironmerntCubeMaps(const LightSources* lightSources, 
		std::vector<Object*> objects, std::vector<Object*> dynamicObjs, float timeInSecond, Object* probe, Object* skyBox)
	{
		GE_ERROR_JUDGE();
		glm::vec3 center = probe->GetComponent<Transform>()->GetPosition();
		UpdateProbeCamera(probe);
		//probe->UpdateCamera();
		auto camera = m_ProbeCamera;// probe->GetCamera();
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


		//std::shared_ptr<CubeMapTexture> environmentCubeMap = probe->GetHdrEnvironmentCubeMap();
		unsigned int environmentMapResolution = probe->GetComponent<LightProbe>()->GetEnvironmentCubeMapResolution();
		std::shared_ptr<CubeMapTexture> environmentCubeMap(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, environmentMapResolution, environmentMapResolution,GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));

		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::vec2 mipMapSize = { environmentCubeMap->GetWidth(),environmentCubeMap->GetHeight() };
		std::shared_ptr<FrameBuffer> frameBuffer(DBG_NEW FrameBuffer());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GE_ERROR_JUDGE();
		frameBuffer->Bind();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GE_ERROR_JUDGE();
		frameBuffer->AttachCubeMapColorTexture(0, environmentCubeMap);
		GE_ERROR_JUDGE();
		//TODO::
		frameBuffer->AttachRenderBuffer(environmentCubeMap->GetWidth(), environmentCubeMap->GetHeight());
		GE_ERROR_JUDGE();
		for (unsigned int mip = 0; mip < environmentCubeMap->GetMipMapLevel(); mip++)
		{
			frameBuffer->Bind();

			frameBuffer->BindRenderBuffer();
			/*glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipMapSize.x, mipMapSize.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer->GetRenderBufferID());*/
			GE_ERROR_JUDGE();
			glViewport(0, 0, mipMapSize.x, mipMapSize.y);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene to environment cubemap (single pass).

			GE_ERROR_JUDGE();
			for (unsigned int i = 0; i < 6; i++)
			{
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetComponent<Transform>()->GetPosition(),{},cameraComponent->Front(),*lightSources });

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentCubeMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//BasicRenderer::DrawLightSources(lightSources, scene);
			

				for (auto obj : objects) {

					std::vector<uint64_t> excludeObjs = probe->GetComponent<LightProbe>()->GetExcludeObjectsId();
					bool drawEnable = true;
					for (uint64_t id : excludeObjs) {
						if (obj->GetId().id == id) {
							drawEnable = false;
							break;
						}
					}
					if (drawEnable) {
						if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
							m_PbrShader->Bind();
							DrawObject(obj, m_PbrShader, scene, 4);
							GE_ERROR_JUDGE();
						}
						else {
							m_NonPbrShader->Bind();
							DrawObject(obj, m_NonPbrShader, scene, 4);
							GE_ERROR_JUDGE();
						}

					}		

				}
				if (skyBox != nullptr) {
					glDepthFunc(GL_LEQUAL);
					m_SkyboxRenderer->Render(skyBox, timeInSecond, scene);
					//m_SkyboxRenderer->Render(skyBox,scene);

					//					DrawObject(skyBox, scene);
					glDepthFunc(GL_LESS);
				}

				//for (auto obj : dynamicObjs) {

				//	std::vector<uint64_t> excludeObjs = probe->GetComponent<LightProbe>()->GetExcludeObjectsId();
				//	bool drawEnable = true;
				//	for (uint64_t id : excludeObjs) {
				//		if (obj->GetId().id == id) {
				//			drawEnable = false;
				//			break;
				//		}
				//	}
				//	if (drawEnable) {
				//		m_AnimatedModelRenderer->Render(obj, timeInSecond,scene);
				//		//if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				//			//m_PbrShader->Bind();
				//		//	DrawObject(obj, m_PbrShader, scene, 4);
				//			GE_ERROR_JUDGE();
				//		//}
				//		//else {
				//			//m_NonPbrShader->Bind();
				//			//DrawObject(obj, m_NonPbrShader, scene, 4);
				//			//GE_ERROR_JUDGE();
				//		//}

				//	}

				//}
				delete scene;
				scene = nullptr;

			}

			mipMapSize.x /= 2.0f;
			mipMapSize.y /= 2.0f;

		}
		frameBuffer->UnBind();
		GE_ERROR_JUDGE();
		frameBuffer->CleanUp();
		GE_ERROR_JUDGE();
		return environmentCubeMap;
	}

	//void IBLProbesRenderer::RenderDiffuseIrradianceMap(const LightSources* lightSources, std::vector<Object*> objects, LightProbe* probe)
	//{
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, probe->GetHdrEnvironmentCubeMap()->GetRendererID());
	//	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//	glm::vec3 center = probe->GetPosition();
	//	probe->UpdateCamera();
	//	auto camera = probe->GetCamera();
	//	auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

	//	auto projection = cameraComponent->GetProjectionMatrix();

	//	std::vector<glm::mat4> ProbeView = {
	//	   glm::lookAt(center, center - camera->Front(),-camera->Up()),
	//	   glm::lookAt(center, center + camera->Front(),-camera->Up()),
	//	   glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
	//	   glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()
	//	   glm::lookAt(center, center - camera->Right(), -camera->Up()),
	//	   glm::lookAt(center, center + camera->Right(), -camera->Up()),

	//	};
	//	std::vector<glm::mat4> ProbeProjectionViews = {
	//		projection * ProbeView[0],
	//		projection * ProbeView[1],
	//		projection * ProbeView[2],
	//		projection * ProbeView[3],
	//		projection * ProbeView[4],
	//		projection * ProbeView[5]
	//	};

	//	std::shared_ptr<CubeMapTexture> diffuseIrradianceCubeMap = probe->GetDiffuseIrradianceCubeMap();

	//	std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
	//	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	frameBuffer->Bind();
	//	frameBuffer->AttachRenderBuffer(diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetHeight());

	//	glViewport(0, 0, diffuseIrradianceCubeMap->GetWidth(), diffuseIrradianceCubeMap->GetWidth()); // don't forget to configure the viewport to the capture dimensions.
	//	frameBuffer->Bind();

	//	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	//	for (unsigned int i = 0; i < 6; ++i)
	//	{
	//		Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetPosition(),{},cameraComponent->Front(),*lightSources });

	//		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, diffuseIrradianceCubeMap->GetRendererID(), 0);
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		//remember bind shader before drawcall!!
	//		m_IrradianceShader->Bind();
	//		m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 2);
	//		glActiveTexture(GL_TEXTURE2);
	//		probe->GetHdrEnvironmentCubeMap()->Bind();

	//		DrawObject(probe->GetObj(), m_IrradianceShader, scene);
	//		probe->GetHdrEnvironmentCubeMap()->UnBind();

	//		delete scene;
	//		scene = nullptr;

	//	}
	//	frameBuffer->UnBind();
	//	frameBuffer->CleanUp();
	//}


	void IBLProbesRenderer::RenderSpecularPrefilterMap(const LightSources* lightSources, Object* probe, std::shared_ptr<CubeMapTexture> environmentMap)
	{
		glm::vec3 center = probe->GetComponent<Transform>()->GetPosition();
		UpdateProbeCamera(probe);
		//probe->UpdateCamera();
		auto camera = m_ProbeCamera;//probe->GetCamera();
		auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

		auto projection = cameraComponent->GetProjectionMatrix();
		std::vector<glm::mat4> ProbeView = {
			glm::lookAt(center, center - camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Front(),-camera->Up()),
			glm::lookAt(center, center + camera->Up(),-camera->Right()),
			glm::lookAt(center, center - camera->Up(),camera->Right()),
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
		auto specularIrradianceMap = probe->GetComponent<LightProbe>()->GetSpecularPrefilterCubeMap();
		m_SpecularPrefilterShader->Bind();
		GE_ERROR_JUDGE();

		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		GE_ERROR_JUDGE();
		environmentMap->Bind();
		//probe->GetHdrEnvironmentCubeMap()->Bind();
		GE_ERROR_JUDGE();

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		GE_ERROR_JUDGE();

		frameBuffer->Bind();
		frameBuffer->AttachCubeMapColorTexture(0, specularIrradianceMap);
		frameBuffer->AttachRenderBuffer(specularIrradianceMap->GetWidth(), specularIrradianceMap->GetHeight());
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GE_ERROR_JUDGE();

		frameBuffer->Bind();
		GE_ERROR_JUDGE();

		unsigned int maxMipMapLevels = 5;
		for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
		{
			//resize framebuffer according to mipmap-level size;
			unsigned int mipWidth = specularIrradianceMap->GetWidth() * std::pow(0.5, mip);
			unsigned int mipHeight = specularIrradianceMap->GetHeight() * std::pow(0.5, mip);
			frameBuffer->BindRenderBuffer();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);
			GE_ERROR_JUDGE();

			float roughness = (float)mip / (float)(maxMipMapLevels - 1);
			m_SpecularPrefilterShader->Bind();
			m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);
			m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentCubeMapDim", environmentMap->GetWidth());

			GE_ERROR_JUDGE();


			for (unsigned int i = 0; i < 6; i++)
			{

				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetComponent<Transform>()->GetPosition(),{},cameraComponent->Front(),*lightSources });
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, specularIrradianceMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				GE_ERROR_JUDGE();

				probe->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);
				//GE_ERROR_JUDGE();
				m_SpecularPrefilterShader->Bind();
				DrawObject(probe, m_SpecularPrefilterShader, scene);
				delete scene;
				scene = nullptr;

			}
		}
	//	probe->GetHdrEnvironmentCubeMap()->UnBind();
		environmentMap->UnBind();
		frameBuffer->UnBind();
		frameBuffer->CleanUp();

	}
	void IBLProbesRenderer::RenderSpecularBRDFLUTMap()
	{
		m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());

		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution);

	
		frameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);
		frameBuffer->BindRenderBuffer();


		glViewport(0, 0, Configuration::EnvironmantMapResolution, Configuration::EnvironmantMapResolution);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


	}

	void IBLProbesRenderer::RenderSHImage(Object* probe, std::shared_ptr<CubeMapTexture> environmentMap)
	{
		//TimeCounter::Start();
		auto coeffs = SphericalHarmonics::UpdateCoeffs(environmentMap);
		probe->GetComponent<LightProbe>()->SetSHCoeffs(coeffs);
	//	TimeCounter::End("update diffuse probe");



	}


	//std::vector<LightProbe*> IBLProbesRenderer::FindKnearProbes(glm::vec3 objPos, std::vector<LightProbe*> probes)
	//{

	//	glm::vec3 pos = objPos;
	//	std::vector<LightProbe*> kProbes;
	//	std::sort(probes.begin(), probes.end(), [=](LightProbe* pa, LightProbe* pb)
	//	{return glm::length(pa->GetPosition() - pos) < glm::length(pb->GetPosition() - pos); });

	//	GE_ASSERT(m_K <= (int)probes.size(), "m_K larger than probes' number!");

	//	for (int i = 0; i < m_K; i++)
	//	{
	//		kProbes.push_back(probes[i]);
	//	}


	//	return kProbes;
	//}

}