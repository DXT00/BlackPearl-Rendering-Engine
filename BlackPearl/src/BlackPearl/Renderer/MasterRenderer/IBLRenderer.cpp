#include "pch.h"
#include "IBLRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <BlackPearl/Component/LightComponent/PointLight.h>
#include <BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h>
namespace BlackPearl {

	float IBLRenderer::s_GICoeffs = 0.5f;
	bool IBLRenderer::s_HDR = true;
	IBLRenderer::IBLRenderer()
	{
		
		//std::vector<std::string> textureFaces =
		//{ "assets/skybox/skybox/right.jpg",
		// "assets/skybox/skybox/left.jpg",
		// "assets/skybox/skybox/top.jpg",
		// "assets/skybox/skybox/bottom.jpg",
		// "assets/skybox/skybox/front.jpg",
		// "assets/skybox/skybox/back.jpg",
		//};
		/*Load Hdr environment map*/
		m_HdrMapToCubeShader.reset(DBG_NEW Shader("assets/shaders/ibl/hdrMapToCube.glsl"));
		//m_IBLShader.reset(DBG_NEW Shader("assets/shaders/ibl/ibl.glsl"));

		/*diffuse shader*/
		////m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/backlot/Diffuse10.hdr")); 
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/OfficeEden/Eden_REF.hdr")); 																														  //m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/sIBL-Collection/sIBL_Collection/apartment/Apartment_Reflection.hdr")); //TODO::
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/Chiricahua_NarrowPath/NarrowPath_8k.jpg"));
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/Playa_Sunrise/Playa_Sunrise_8k.jpg"));

		m_IrradianceShader.reset(DBG_NEW Shader("assets/shaders/ibl/irradianceConvolution.glsl"));
		m_IBLShader.reset(DBG_NEW Shader("assets/shaders/ibl/iblTexture.glsl"));
		m_SpecularPrefilterShader.reset(DBG_NEW Shader("assets/shaders/ibl/prefilterMap.glsl"));
		m_SpecularBRDFLutShader.reset(DBG_NEW Shader("assets/shaders/ibl/brdf.glsl"));

		m_HdrCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentMapDim, m_EnvironmentMapDim, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_FLOAT));
		m_IrradianceCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_IrradianceDiffuseMapDim, m_IrradianceDiffuseMapDim, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_FLOAT));
		m_BRDFLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, m_BRDFLutDim, m_BRDFLutDim, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_PrefilterCubeMap.reset(DBG_NEW CubeMapTexture(Texture::CubeMap, m_SpecularPrefilterMapDim, m_SpecularPrefilterMapDim, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_FLOAT, true));
		m_SkyBoxCubeMap.reset(DBG_NEW CubeMapTexture(Texture::Type::CubeMap, m_EnvironmentMapDim, m_EnvironmentMapDim, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB8, GL_RGB, GL_FLOAT, true));
		//m_HdrTexture.reset(DBG_NEW HDRTexture("assets/texture/hdrEnvironmentMap/Desert_Highway/Road_to_MonumentValley_Ref.hdr"));

	}
	void IBLRenderer::Init(Object* hdrCubeObj, Object* brdfLUTQuad,Object* skybox,std::vector<Object*>objs, const LightSources* lightSources)
	{
		GE_ASSERT(hdrCubeObj, "hdrCubeObj is nullptr!");
		GE_ASSERT(brdfLUTQuad, "brdfLUTQuad is nullptr!");

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		float aspect = 1.0;
		m_CaptureProjection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 10.0f);
		m_CaptureViews = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};
		m_CaptureProjectionViews =
		{
			m_CaptureProjection * m_CaptureViews[0],
			m_CaptureProjection * m_CaptureViews[1],
			m_CaptureProjection * m_CaptureViews[2],
			m_CaptureProjection * m_CaptureViews[3],
			m_CaptureProjection * m_CaptureViews[4],
			m_CaptureProjection * m_CaptureViews[5]
		};
		m_CubeObj = hdrCubeObj;
		m_LUTQuad = brdfLUTQuad;
		RenderEnvironmerntCubeMaps(lightSources, objs,skybox);
		//RenderHdrMapToEnvironmentCubeMap(); //for environment skybox
		RenderDiffuseIrradianceMap(); //for diffuse GI
		RenderSpecularPrefilterMap(); //for specular GI's irradiance part
		RenderSpecularBRDFLUTMap(); //for specular GI's brdf part
		m_IsInitialize = true;
	}
	IBLRenderer::~IBLRenderer()
	{
	}
	void IBLRenderer::RenderHdrMapToEnvironmentCubeMap() {


		float aspect = (float)m_HdrCubeMap->GetWidth() / (float)m_HdrCubeMap->GetHeight();
		m_CaptureProjection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 10.0f);
		m_CaptureViews = {
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};
		m_CaptureProjectionViews =
		{
			m_CaptureProjection * m_CaptureViews[0],
			m_CaptureProjection * m_CaptureViews[1],
			m_CaptureProjection * m_CaptureViews[2],
			m_CaptureProjection * m_CaptureViews[3],
			m_CaptureProjection * m_CaptureViews[4],
			m_CaptureProjection * m_CaptureViews[5]
		};
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		GE_ERROR_JUDGE();
		frameBuffer->Bind();
		GE_ERROR_JUDGE();
		frameBuffer->AttachCubeMapColorTexture(0, m_HdrCubeMap);
		GE_ERROR_JUDGE();
		frameBuffer->AttachRenderBuffer(m_HdrCubeMap->GetWidth(), m_HdrCubeMap->GetHeight());
		GE_ERROR_JUDGE();

		m_HdrMapToCubeShader->Bind();
		glActiveTexture(GL_TEXTURE0 + 1);
		m_HdrTexture->Bind();
		m_HdrMapToCubeShader->SetUniform1i("hdrTexture", 1);

		//Draw CubeMap from hdrMap
		glViewport(0, 0, m_HdrCubeMap->GetWidth(), m_HdrCubeMap->GetHeight());
		frameBuffer->Bind();
		frameBuffer->BindRenderBuffer();

		/*
		面向立方体六个面设置六个不同的视图矩阵，
		给定投影矩阵的 fov 为 90 度以捕捉整个面，
		并渲染立方体六次，将结果存储在浮点帧缓冲中
		*/
		for (unsigned int i = 0; i < 6; i++)
		{	
			//TODO:: camera->Front有问题，不过Front只用于spotLight，这里暂且不管
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_CaptureProjectionViews[i] ,m_CaptureViews[i],m_CaptureProjection,Renderer::GetSceneData()->CameraPosition,Renderer::GetSceneData()->CameraRotation, Renderer::GetSceneData()->CameraFront,Renderer::GetSceneData()->LightSources };

			//m_HdrMapToCubeShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);
			//将CubeMap的六个面附加到FrameBuffer的GL_COLOR_ATTACHMENT0中
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_HdrCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_HdrMapToCubeShader->Bind();
			DrawObject(m_CubeObj, m_HdrMapToCubeShader, scene);
			delete scene;
			scene = nullptr;
		}
		
		frameBuffer->UnBind();
		GE_ERROR_JUDGE();
		frameBuffer->CleanUp();
		GE_ERROR_JUDGE();
	}

	void IBLRenderer::RenderEnvironmerntCubeMaps(const LightSources* lightSources, std::vector<Object*> objects, Object* skyBox)
	{
		std::shared_ptr<Shader> m_PbrShader(DBG_NEW BlackPearl::Shader("assets/shaders/pbr/PbrTexture.glsl"));
		std::shared_ptr<Shader>  m_NonPbrShader(DBG_NEW BlackPearl::Shader("assets/shaders/IronMan.glsl"));
		GE_ERROR_JUDGE();
		glm::vec3 center{ 0,0,0 };
	
		float aspect = (float)m_SkyBoxCubeMap->GetWidth() / (float)m_SkyBoxCubeMap->GetHeight();
		auto projection = glm::perspective(glm::radians(90.0f), aspect, 0.1f, 10.0f);
		//	cameraComponent->SetPosition(probe->GetPosition());

		std::vector<glm::mat4> ProbeView = {
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))

		};
		std::vector<glm::mat4> ProbeProjectionViews = {
			projection * ProbeView[0],
			projection * ProbeView[1],
			projection * ProbeView[2],
			projection * ProbeView[3],
			projection * ProbeView[4],
			projection * ProbeView[5]
		};


		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glm::vec2 mipMapSize = { m_SkyBoxCubeMap->GetWidth(),m_SkyBoxCubeMap->GetHeight() };
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GE_ERROR_JUDGE();
		frameBuffer->Bind();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GE_ERROR_JUDGE();
		frameBuffer->AttachCubeMapColorTexture(0, m_SkyBoxCubeMap);
		GE_ERROR_JUDGE();
		//TODO::
		frameBuffer->AttachRenderBuffer(m_SkyBoxCubeMap->GetWidth(), m_SkyBoxCubeMap->GetHeight());
		GE_ERROR_JUDGE();
		for (unsigned int mip = 0; mip < m_SkyBoxCubeMap->GetMipMapLevel(); mip++)
		{

			
			/*glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipMapSize.x, mipMapSize.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, frameBuffer->GetRenderBufferID());*/
			GE_ERROR_JUDGE();
			glViewport(0, 0, mipMapSize.x, mipMapSize.y);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Render the scene to environment cubemap (single pass).

			GE_ERROR_JUDGE();
			for (unsigned int i = 0; i < 6; i++)
			{
				frameBuffer->Bind();
				frameBuffer->BindRenderBuffer();
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ ProbeProjectionViews[i] ,ProbeView[i],projection,Renderer::GetSceneData()->CameraPosition,Renderer::GetSceneData()->CameraRotation, Renderer::GetSceneData()->CameraFront,Renderer::GetSceneData()->LightSources };
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_SkyBoxCubeMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				//BasicRenderer::DrawLightSources(lightSources, scene);
				glDepthFunc(GL_LEQUAL);
				DrawObject(skyBox, scene,8);
				glDepthFunc(GL_LESS);
			/*	for (auto obj : objects) {
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

				}*/
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

	}
	void IBLRenderer::RenderDiffuseIrradianceMap()
	{
		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		//m_HdrCubeMap->Bind();
		//m_SkyBoxCubeMap->Bind();
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_HdrCubeMapID);
		//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		/*std::shared_ptr<CubeMapTexture> diffuseIrradianceCubeMap(DBG_NEW CubeMapTexture(
			Texture::CubeMap, 32, 32, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));

		m_IrradianceCubeMapID = diffuseIrradianceCubeMap->GetRendererID();*/

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(m_IrradianceCubeMap->GetWidth(), m_IrradianceCubeMap->GetHeight());

		

		/*glBindRenderbuffer(GL_RENDERBUFFER, m_FrameBuffer->GetRenderBufferID());
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

		m_IrradianceShader->Bind();
		m_IrradianceShader->SetUniform1i("u_EnvironmentMap", 0);
		m_IrradianceShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());

		glActiveTexture(GL_TEXTURE0);
		//m_HdrCubeMap->Bind();
		m_SkyBoxCubeMap->Bind();
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_HdrCubeMapID);
		// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.

		glViewport(0, 0, m_IrradianceCubeMap->GetWidth(), m_IrradianceCubeMap->GetHeight()); // don't forget to configure the viewport to the capture dimensions.
		frameBuffer->Bind();

		for (unsigned int i = 0; i < 6; ++i)
		{
			Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_CaptureProjectionViews[i] ,m_CaptureViews[i],m_CaptureProjection,Renderer::GetSceneData()->CameraPosition,Renderer::GetSceneData()->CameraRotation,Renderer::GetSceneData()->CameraFront,Renderer::GetSceneData()->LightSources };

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_IrradianceCubeMap->GetRendererID(), 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			DrawObject(m_CubeObj, m_IrradianceShader, scene);
			delete scene;
			scene = nullptr;
		}
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
		m_SkyBoxCubeMap->UnBind();

	}

	void IBLRenderer::RenderSpecularPrefilterMap()
	{
		/*std::shared_ptr<CubeMapTexture> specularPrefilterCubeMap(new CubeMapTexture(Texture::CubeMap, 128, 128, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_RGB16F, GL_RGB, GL_FLOAT, true));
		m_PrefilterCubeMapID = specularPrefilterCubeMap->GetRendererID();*/

		// pbr: run a quasi monte-carlo simulation on the environment lighting to create a prefilter (cube)map.
		m_SpecularPrefilterShader->Bind();
		m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);
		glActiveTexture(GL_TEXTURE0);
		//m_HdrCubeMap->Bind();

		m_SkyBoxCubeMap->Bind();
		//glBindTexture(GL_TEXTURE_CUBE_MAP, m_HdrCubeMapID);

		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		frameBuffer->Bind();
		frameBuffer->AttachCubeMapColorTexture(0, m_PrefilterCubeMap);
		frameBuffer->AttachRenderBuffer(m_SpecularPrefilterMapDim, m_SpecularPrefilterMapDim);

		frameBuffer->Bind();
		unsigned int maxMipMapLevels = 5;
		
		for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
		{
			//resize framebuffer according to mipmap-level size;
			unsigned int mipWidth = m_SpecularPrefilterMapDim * std::pow(0.5, mip);
			unsigned int mipHeight = m_SpecularPrefilterMapDim * std::pow(0.5, mip);
			frameBuffer->BindRenderBuffer();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipMapLevels - 1);
			m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);
			m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentCubeMapDim", m_EnvironmentMapDim);

			for (unsigned int i = 0; i < 6; i++)
			{
				Renderer::SceneData* scene = DBG_NEW Renderer::SceneData{ m_CaptureProjectionViews[i] ,m_CaptureViews[i],m_CaptureProjection,Renderer::GetSceneData()->CameraPosition,Renderer::GetSceneData()->CameraRotation,Renderer::GetSceneData()->CameraFront,Renderer::GetSceneData()->LightSources };

				//m_SpecularPrefilterShader->SetUniformMat4f("u_CubeMapProjectionView", m_CaptureProjectionViews[i]);

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PrefilterCubeMap->GetRendererID(), mip);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);
				//m_SpecularPrefilterShader->Bind();
				DrawObject(m_CubeObj, m_SpecularPrefilterShader,scene,4);
				delete scene;
				scene = nullptr;
			}
			
		}

		m_SkyBoxCubeMap->UnBind();
		frameBuffer->UnBind();
		frameBuffer->CleanUp();
	}

	void IBLRenderer::RenderSpecularBRDFLUTMap()
	{
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(m_BRDFLutDim, m_BRDFLutDim);

		//m_FrameBuffer->Bind();
		//m_FrameBuffer->BindRenderBuffer();
		frameBuffer->AttachColorTexture(m_BRDFLUTTexture, 0);
		frameBuffer->BindRenderBuffer();
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		glViewport(0, 0, m_BRDFLutDim, m_BRDFLutDim);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_LUTQuad, m_SpecularBRDFLutShader);
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


		//std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//m_FrameBuffer->Bind();
		//m_FrameBuffer->BindRenderBuffer();
		//m_FrameBuffer->AttachColorTexture(m_BRDFLUTTexture, 0);//必须attach 0 
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);

		//

		//glViewport(0, 0, 512, 512);
		//m_SpecularBRDFLutShader->Bind();
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//DrawObject(m_LUTQuad, m_SpecularBRDFLutShader);
		//m_FrameBuffer->UnBind();

	}

	void IBLRenderer::RenderSpheres(Object* sphere)
	{
		GE_ASSERT(m_IsInitialize, "please initial IBLRenderer first! IBLRenderer::init()");


		int rows = 7;
		int cols = 7;
		int spacing = 2;
		m_IBLShader->Bind();
		m_IBLShader->SetUniform1i("u_IrradianceMap", 0);
		m_IBLShader->SetUniform1i("u_PrefilterMap", 1);
		m_IBLShader->SetUniform1i("u_BrdfLUTMap", 2);

		glActiveTexture(GL_TEXTURE0);
		m_IrradianceCubeMap->Bind();
		glActiveTexture(GL_TEXTURE1);
		m_PrefilterCubeMap->Bind();
		glActiveTexture(GL_TEXTURE2);
		m_BRDFLUTTexture->Bind();

		m_IBLShader->SetUniform1f("u_ao", 1.0f);
		for (int row = 0; row < rows; row++) {
			m_IBLShader->SetUniform1f("u_metallic", (float)row / (float)rows);

			for (int col = 0; col < cols; col++) {

				sphere->GetComponent<Transform>()->SetPosition({
					(col - (cols / 2)) * spacing,
					(row - (rows / 2)) * spacing,
					0.0f });
				m_IBLShader->SetUniform1f("u_roughness", glm::clamp((float)col / (float)cols, 0.05f, 1.0f));
				sphere->GetComponent<MeshRenderer>()->SetShaders(m_IBLShader);
				DrawObject(sphere, m_IBLShader);

			}
		}



	}

	void IBLRenderer::RenderTextureSphere(std::vector<Object*> objs,LightSources* lightSources)
	{
	

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GE_ERROR_JUDGE();
		for (Object* obj:objs)
		{
			//m_IBLShader->SetUniform1i("u_IsPBRObjects",obj->GetComponent<MeshRenderer>()->GetIsPBRObject());
			m_IBLShader->Bind();
			m_IBLShader->SetUniform1i("u_IrradianceMap", 0);
			m_IBLShader->SetUniform1i("u_PrefilterMap", 1);
			m_IBLShader->SetUniform1i("u_BrdfLUTMap", 2);

			m_IBLShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);
			m_IBLShader->SetUniform1i("u_Settings.hdr", s_HDR);

			


			GE_ERROR_JUDGE();
			glActiveTexture(GL_TEXTURE0);
			m_IrradianceCubeMap->Bind();
			glActiveTexture(GL_TEXTURE1);
			m_PrefilterCubeMap->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_BRDFLUTTexture->Bind();

			int k = 0;
			m_IBLShader->SetUniform1f("u_FarPlane", ShadowMapPointLightRenderer::s_FarPlane);

			for (Object* pointLight : lightSources->GetPointLights()) {
				m_IBLShader->SetUniform1i("u_ShadowMap[" + std::to_string(k) + "]", k + 3);
				glActiveTexture(GL_TEXTURE0+ (k + 3));
				pointLight->GetComponent<PointLight>()->GetShadowMap()->Bind();
				k++;
			}


			DrawObject(obj, m_IBLShader, Renderer::GetSceneData(), 5);

			m_IrradianceCubeMap->UnBind();
			m_PrefilterCubeMap->UnBind();
			m_BRDFLUTTexture->UnBind();

		}
		

	}


	void IBLRenderer::DrawBRDFLUTMap()
	{
		glViewport(0, 0, 240, 240);
		DrawObject(m_LUTQuad, m_SpecularBRDFLutShader);
		glViewport(0, 0, BlackPearl::Configuration::WindowWidth, BlackPearl::Configuration::WindowHeight);
	}

	void IBLRenderer::Render(std::vector<Object*> objs)
	{
	}
}