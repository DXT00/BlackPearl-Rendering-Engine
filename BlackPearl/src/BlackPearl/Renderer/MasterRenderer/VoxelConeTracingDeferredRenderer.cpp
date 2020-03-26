#include "pch.h"
#include "VoxelConeTracingDeferredRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Renderer/MasterRenderer/GBufferRenderer.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {
	bool VoxelConeTracingDeferredRenderer::s_Shadows = true;
	bool VoxelConeTracingDeferredRenderer::s_IndirectDiffuseLight = true;
	bool VoxelConeTracingDeferredRenderer::s_IndirectSpecularLight = true;
	bool VoxelConeTracingDeferredRenderer::s_DirectLight = false;
	float VoxelConeTracingDeferredRenderer::s_GICoeffs = 0.8f;
	bool VoxelConeTracingDeferredRenderer::s_VoxelizeNow = true;
	bool VoxelConeTracingDeferredRenderer::s_HDR = false;
	bool VoxelConeTracingDeferredRenderer::s_GuassianHorizontal = false;
	bool VoxelConeTracingDeferredRenderer::s_GuassianVertical = false;
	bool VoxelConeTracingDeferredRenderer::s_ShowBlurArea = false;
	float VoxelConeTracingDeferredRenderer::s_SpecularBlurThreshold = 0.1f;

	VoxelConeTracingDeferredRenderer::VoxelConeTracingDeferredRenderer()
	{

	}
	VoxelConeTracingDeferredRenderer::~VoxelConeTracingDeferredRenderer()
	{
		if (!m_CubeObj)delete m_CubeObj;
		if (!m_QuadObj)delete m_QuadObj;
		if (!m_VoxelTexture) delete m_VoxelTexture;
		m_FrameBuffer->CleanUp();
	}


	void VoxelConeTracingDeferredRenderer::Init(unsigned int viewportWidth, unsigned int viewportHeight,
		Object* quadObj, Object* quadGbufferObj, Object* brdfLUTQuadObj, Object* quadFinalScreenObj , Object* surroundSphere,Object* cubeObj)
	{
		GE_ASSERT(quadObj, "m_QuadObj is nullptr!");
		GE_ASSERT(quadGbufferObj, "m_QuadGbufferObj is nullptr!");
		GE_ASSERT(brdfLUTQuadObj, "m_BrdfLUTQuadObj is nullptr!");
		GE_ASSERT(surroundSphere, "m_SurroundSphere is nullptr!");

		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");

		m_QuadObj = quadObj;
		m_CubeObj = cubeObj;
		m_QuadGbufferObj = quadGbufferObj;
		m_BrdfLUTQuadObj = brdfLUTQuadObj;
		m_QuadFinalScreenObj = quadFinalScreenObj;
		m_SurroundSphere = surroundSphere;
		glEnable(GL_MULTISAMPLE);
		m_VCTAmbientGIShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/gBuffer_ambientFilter_voxelConeTracingPBR.glsl"));
		m_VCTPointLightShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/gBuffer_pointLight_voxelConeTracingPBR.glsl"));
		InitVoxelization();
		InitVoxelVisualization(viewportWidth, viewportHeight);

		/*debug shader*/
		m_VoxelizationTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/voxelizeTest.glsl"));
		m_FrontBackCubeTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/quadTest.glsl"));

		/*pbr BRDF LUT shader*/
		m_SpecularBRDFLutShader.reset(DBG_NEW Shader("assets/shaders/ibl/brdf.glsl"));
		//const std::vector<GLfloat> textureImage2D(4 * 256 * 256, 0.0f);
		RenderSpecularBRDFLUTMap();

		/* GBuffer */
		m_GBuffer.reset(DBG_NEW GBuffer(m_ScreenWidth,m_ScreenHeight));
		m_GBufferShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/gBufferVoxel.glsl"));

		m_GuassianFilterHorizontalShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/GuassianFilter_horizontal.glsl"));
		m_GuassianFilterVerticalShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/GuassianFilter_vertical.glsl"));
		m_FinalScreenShader.reset(DBG_NEW Shader("assets/shaders/gBufferVoxel/FinalScreenQuad.glsl"));


		/* FrameBuffer */
		m_PostProcessTexture.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth,m_ScreenHeight, false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_FrameBuffer.reset(DBG_NEW FrameBuffer());
		m_FrameBuffer->Bind();
		m_FrameBuffer->AttachRenderBuffer(m_ScreenWidth, m_ScreenHeight);
		m_FrameBuffer->AttachColorTexture(m_PostProcessTexture, 0);
		m_FrameBuffer->BindRenderBuffer();
		
		m_BlurHTexture.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth ,m_ScreenHeight , false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_BlurHoriFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_BlurHoriFrameBuffer->Bind();
		m_BlurHoriFrameBuffer->AttachRenderBuffer(m_ScreenWidth , m_ScreenHeight );
		m_BlurHoriFrameBuffer->AttachColorTexture(m_BlurHTexture, 0);
		m_BlurHoriFrameBuffer->BindRenderBuffer();

		m_BlurVTexture.reset(DBG_NEW Texture(Texture::Type::None, m_ScreenWidth ,m_ScreenHeight , false, GL_LINEAR, GL_LINEAR, GL_RGBA16F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT));
		m_BlurVertFrameBuffer.reset(DBG_NEW FrameBuffer());
		m_BlurVertFrameBuffer->Bind();
		m_BlurVertFrameBuffer->AttachRenderBuffer(m_ScreenWidth , m_ScreenHeight );
		m_BlurVertFrameBuffer->AttachColorTexture(m_BlurVTexture, 0);
		m_BlurVertFrameBuffer->BindRenderBuffer();

		m_IsInitialize = true;
	}

	void VoxelConeTracingDeferredRenderer::InitVoxelization()
	{
		m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelizationPBR.glsl"));
		//m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelization.glsl"));

		const std::vector<GLfloat> texture3D(4 * m_VoxelTextureSize * m_VoxelTextureSize * m_VoxelTextureSize, 0.0f);
		m_VoxelTexture = DBG_NEW Texture3D(texture3D, m_VoxelTextureSize, m_VoxelTextureSize, m_VoxelTextureSize, true);
	}
	void VoxelConeTracingDeferredRenderer::InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//Shaders
		m_WorldPositionShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/worldPosition.glsl"));
		m_VoxelVisualizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/voxelVisualization.glsl"));

	}

	void VoxelConeTracingDeferredRenderer::RenderGBuffer(const std::vector<Object*>& objs, Object* skybox)
	{
		
		glViewport(0, 0, m_ScreenWidth,m_ScreenHeight);
		m_GBuffer->Bind();
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (Object* obj : objs) {

			m_GBufferShader->Bind();

	
			m_GBufferShader->SetUniform1i("u_IsSkybox", false);
			GE_ASSERT(obj->GetId().index() < 256, "GBuffer store max object number is:256");
			m_GBufferShader->SetUniform1i("u_ObjectId", obj->GetId().index());

			DrawObject(obj, m_GBufferShader);



		}
		
		m_GBuffer->UnBind();


	}

	void VoxelConeTracingDeferredRenderer::DrawGBuffer(Object* gBufferDebugQuad)
	{

		glViewport(0, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetPositionTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(0, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetDiffuseRoughnessTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(320, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetSpecularMentallicTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 0, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetAmbientGIAOTexture());
		DrawObject(gBufferDebugQuad);

		glViewport(640, 270, 320, 270);
		gBufferDebugQuad->GetComponent<MeshRenderer>()->SetTextures(m_GBuffer->GetNormalMapTexture());
		DrawObject(gBufferDebugQuad);

	}

	void VoxelConeTracingDeferredRenderer::SetgBufferTextureUniforms()
	{
		
		glActiveTexture(GL_TEXTURE0);
		m_VoxelTexture->Bind();
		m_VCTAmbientGIShader->SetUniform1i("texture3D", 0);
		glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
		m_VCTAmbientGIShader->SetUniform1i("gPosition", 1);
		m_VCTAmbientGIShader->SetUniform1i("gNormal", 2);
		m_VCTAmbientGIShader->SetUniform1i("gDiffuse_Roughness", 3);
		m_VCTAmbientGIShader->SetUniform1i("gSpecular_Mentallic", 4);
		m_VCTAmbientGIShader->SetUniform1i("gAmbientGI_AO", 5);
		m_VCTAmbientGIShader->SetUniform1i("gNormalMap", 6);

	}


	void VoxelConeTracingDeferredRenderer::Voxilize(const std::vector<Object*>& objs, Object* skybox, bool clearVoxelizationFirst)
	{
		if (clearVoxelizationFirst) {
			float clearColor[4] = { 0.0,0.0,0.0,-1.0 };
			m_VoxelTexture->Clear(clearColor);
		}

		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		//glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Settings.
		//  disable writing of frame buffer color components
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		if (skybox != nullptr) {

			//glDepthFunc(GL_LEQUAL);
			m_VoxelizationShader->Bind();
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VoxelizationShader->SetUniform1i("texture3D", 0);
			glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);
			skybox->GetComponent<Transform>()->SetScale(m_CubeObj->GetComponent<Transform>()->GetScale() - glm::vec3(3.0f));
			skybox->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);

			m_VoxelizationShader->SetUniform1i("u_IsSkybox", true);
			m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);
			DrawObject(skybox, m_VoxelizationShader);
			//glDepthFunc(GL_LESS);
		}

		for (auto obj : objs) {
			//m_VoxelizationShader->Bind();
			//glActiveTexture(GL_TEXTURE0);
			//m_VoxelTexture->Bind();
			//m_VoxelizationShader->SetUniform1i("texture3D", 0);
			//m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
			////m_VoxelizationShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());
			//glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA16);
			m_VoxelizationShader->Bind();
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VoxelizationShader->SetUniform1i("texture3D", 0);
			glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);


			m_VoxelizationShader->SetUniform1i("u_IsSkybox", false);
			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 1);
			}
			else {
				m_VoxelizationShader->SetUniform1i("u_IsPBRObjects", 0);

			}
			DrawObject(obj, m_VoxelizationShader);

		}
		if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
			glGenerateMipmap(GL_TEXTURE_3D);
			m_RegenerateMipmapQueued = false;
		}

		//enable writing of frame buffer color components
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	}

	void VoxelConeTracingDeferredRenderer::Render(Camera* camera, const std::vector<Object*>& objs, const LightSources* lightSources,
		unsigned int viewportWidth, unsigned int viewportHeight,  Object* skybox,RenderingMode reneringMode)
	{
		GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingDeferredRenderer::Init() first!");
		//bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		//RenderGBuffer(objs, skybox);
		if (s_VoxelizeNow) {
			Voxilize(objs, skybox, true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
		}
		switch (reneringMode) {
		case RenderingMode::VOXELIZATION_VISUALIZATION:
			RenderVoxelVisualization(camera, objs, viewportWidth, viewportHeight);
			break;
		case RenderingMode::VOXEL_CONE_TRACING:
			//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			//m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			RenderScene(objs, lightSources, viewportWidth, viewportHeight, skybox);
			break;
		}
	}

	void VoxelConeTracingDeferredRenderer::RenderVoxelVisualization(Camera* camera, const std::vector<Object*>& objs, unsigned int viewportWidth, unsigned int viewportHeight)
	{
		m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);
		m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);

		// -------------------------------------------------------
		// Render 3D texture to screen.
		// -------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0, viewportWidth, viewportHeight);
		glViewport(0, 0, viewportHeight, viewportHeight);

		/*	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

		// Render.



		m_VoxelVisualizationShader->Bind();

		glActiveTexture(GL_TEXTURE0);
		m_VoxelTexture->Bind();
		m_VoxelVisualizationShader->SetUniform1i("texture3D", 0);




		m_VoxelVisualizationShader->SetUniformVec3f("u_CameraFront", camera->Front());
		m_VoxelVisualizationShader->SetUniformVec3f("u_CameraUp", camera->Up());
		m_VoxelVisualizationShader->SetUniformVec3f("u_CameraRight", camera->Right());

		m_VoxelVisualizationShader->SetUniform1i("u_State", Configuration::State);
		m_VoxelVisualizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());


		//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		DrawObject(m_QuadObj, m_VoxelVisualizationShader);





	}

	void VoxelConeTracingDeferredRenderer::RenderScene(const std::vector<Object*>& objs, const LightSources* lightSources ,
		unsigned int viewportWidth, unsigned int viewportHeight,Object* skybox)
	{


		//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);




	



		//TODO::
		float specularReflectivity = 1.0f, diffuseReflectivity = 1.0f, emissivity = 0.1f, specularDiffusion = 0.0f;
		float transparency = 0.0f, refractiveIndex = 1.4f;


		
		RenderGBuffer(objs, skybox);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

		
		//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);








		/********************************* Point light pass ************************************************/

		m_FrameBuffer->Bind();

		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);// blend the computed fragment color values with the values in the color buffers
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);

		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (Object* pointLight : lightSources->GetPointLights()) {


			float radius = GBufferRenderer::CalculateSphereRadius(pointLight);
			m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
			m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());


			m_VCTPointLightShader->Bind();
			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VCTPointLightShader->SetUniform1i("texture3D", 0);
			m_VCTPointLightShader->SetUniform1i("gPosition", 1);
			m_VCTPointLightShader->SetUniform1i("gNormal", 2);
			m_VCTPointLightShader->SetUniform1i("gDiffuse_Roughness", 3);
			m_VCTPointLightShader->SetUniform1i("gSpecular_Mentallic", 4);
			m_VCTPointLightShader->SetUniform1i("gAmbientGI_AO", 5);
			m_VCTPointLightShader->SetUniform1i("gNormalMap", 6);

			m_VCTPointLightShader->SetUniformVec2f("u_ScreenSize", glm::vec2(m_ScreenWidth, m_ScreenHeight));
			m_VCTPointLightShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

			glActiveTexture(GL_TEXTURE1);
			m_GBuffer->GetPositionTexture()->Bind();
			glActiveTexture(GL_TEXTURE2);
			m_GBuffer->GetNormalTexture()->Bind();
			glActiveTexture(GL_TEXTURE3);
			m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
			glActiveTexture(GL_TEXTURE4);
			m_GBuffer->GetSpecularMentallicTexture()->Bind();
			glActiveTexture(GL_TEXTURE5);
			m_GBuffer->GetAmbientGIAOTexture()->Bind();
			glActiveTexture(GL_TEXTURE6);
			m_GBuffer->GetNormalMapTexture()->Bind();

			m_VCTPointLightShader->SetUniformVec3f("u_PointLight.ambient", pointLight->GetComponent<PointLight>()->GetLightProps().ambient);
			m_VCTPointLightShader->SetUniformVec3f("u_PointLight.diffuse", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
			m_VCTPointLightShader->SetUniformVec3f("u_PointLight.specular", pointLight->GetComponent<PointLight>()->GetLightProps().specular);
			m_VCTPointLightShader->SetUniformVec3f("u_PointLight.position", pointLight->GetComponent<Transform>()->GetPosition());

			m_VCTPointLightShader->SetUniform1f("u_PointLight.constant", pointLight->GetComponent<PointLight>()->GetAttenuation().constant);
			m_VCTPointLightShader->SetUniform1f("u_PointLight.linear", pointLight->GetComponent<PointLight>()->GetAttenuation().linear);
			m_VCTPointLightShader->SetUniform1f("u_PointLight.quadratic", pointLight->GetComponent<PointLight>()->GetAttenuation().quadratic);
			m_VCTPointLightShader->SetUniform1f("u_PointLight.intensity", pointLight->GetComponent<PointLight>()->GetLightProps().intensity);

			m_VCTPointLightShader->SetUniform1i("u_Settings.directLight", VoxelConeTracingDeferredRenderer::s_DirectLight);
			m_VCTPointLightShader->SetUniform1i("u_Settings.shadows", VoxelConeTracingDeferredRenderer::s_Shadows);

		

			m_SurroundSphere->GetComponent<MeshRenderer>()->SetShaders(m_VCTPointLightShader);
			DrawObject(m_SurroundSphere, m_VCTPointLightShader);





		}





		/***************************** GI pass and Guassian fileter ***************************************/

		m_VCTAmbientGIShader->Bind();

		glActiveTexture(GL_TEXTURE0);
		m_VoxelTexture->Bind();
		m_VCTAmbientGIShader->SetUniform1i("texture3D", 0);

		m_VCTAmbientGIShader->SetUniform1i("gPosition", 1);
		m_VCTAmbientGIShader->SetUniform1i("gNormal", 2);
		m_VCTAmbientGIShader->SetUniform1i("gDiffuse_Roughness", 3);
		m_VCTAmbientGIShader->SetUniform1i("gSpecular_Mentallic", 4);
		m_VCTAmbientGIShader->SetUniform1i("gAmbientGI_AO", 5);
		m_VCTAmbientGIShader->SetUniform1i("gNormalMap", 6);
		glActiveTexture(GL_TEXTURE1);
		m_GBuffer->GetPositionTexture()->Bind();
		glActiveTexture(GL_TEXTURE2);
		m_GBuffer->GetNormalTexture()->Bind();
		glActiveTexture(GL_TEXTURE3);
		m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
		glActiveTexture(GL_TEXTURE4);
		m_GBuffer->GetSpecularMentallicTexture()->Bind();
		glActiveTexture(GL_TEXTURE5);
		m_GBuffer->GetAmbientGIAOTexture()->Bind();
		glActiveTexture(GL_TEXTURE6);
		m_GBuffer->GetNormalMapTexture()->Bind();

		glActiveTexture(GL_TEXTURE7);
		m_SpecularBrdfLUTTexture->Bind();
		m_VCTAmbientGIShader->SetUniform1i("u_BrdfLUTMap", 7);
		
		m_VCTAmbientGIShader->SetUniform1i("u_Settings.shadows", s_Shadows);
		m_VCTAmbientGIShader->SetUniform1i("u_Settings.indirectDiffuseLight", VoxelConeTracingDeferredRenderer::s_IndirectDiffuseLight);
		m_VCTAmbientGIShader->SetUniform1i("u_Settings.indirectSpecularLight", VoxelConeTracingDeferredRenderer::s_IndirectSpecularLight);
		m_VCTAmbientGIShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);
		m_VCTAmbientGIShader->SetUniform1f("u_SpecularBlurThreshold", s_SpecularBlurThreshold);

		m_VCTAmbientGIShader->SetUniform1f("u_Material.specularDiffusion", specularDiffusion);

		m_VCTAmbientGIShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
		m_VCTAmbientGIShader->SetUniformVec2f("gScreenSize", glm::vec2(m_ScreenWidth,m_ScreenHeight));
		DrawObject(m_QuadGbufferObj, m_VCTAmbientGIShader);


	


		m_FrameBuffer->UnBind();
		/********************************* Guassian filter pass ************************************************/
		m_BlurHoriFrameBuffer->Bind();
		
		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight );
		glClear(GL_COLOR_BUFFER_BIT);

		m_GuassianFilterHorizontalShader->Bind();
		m_GuassianFilterHorizontalShader->SetUniform1i("u_FinalScreenTexture", 1);
		glActiveTexture(GL_TEXTURE1);
		m_PostProcessTexture->Bind();


		m_GuassianFilterHorizontalShader->SetUniform1i("gPosition", 2); //blur filter get object id from gPosition texture
		glActiveTexture(GL_TEXTURE2);
		m_GBuffer->GetPositionTexture()->Bind();

		m_GuassianFilterHorizontalShader->SetUniform1i("u_Settings.hdr", s_HDR);
		m_GuassianFilterHorizontalShader->SetUniform1i("u_Settings.showBlurArea", s_ShowBlurArea);

		m_GuassianFilterHorizontalShader->SetUniform1i("u_Settings.guassian_horiziotal", s_GuassianHorizontal);
		m_GuassianFilterHorizontalShader->SetUniform1f("u_ScreenWidth", m_ScreenWidth );
		DrawObject(m_QuadFinalScreenObj, m_GuassianFilterHorizontalShader);

		m_BlurHoriFrameBuffer->UnBind();


		m_BlurVertFrameBuffer->Bind();
		glViewport(0, 0, m_ScreenWidth , m_ScreenHeight );
		glClear(GL_COLOR_BUFFER_BIT);

		m_GuassianFilterVerticalShader->Bind();
		m_GuassianFilterVerticalShader->SetUniform1i("u_FinalScreenTexture", 1);
		glActiveTexture(GL_TEXTURE1);
		m_PostProcessTexture->Bind();

		m_GuassianFilterVerticalShader->SetUniform1i("u_BlurHTexture", 2);
		glActiveTexture(GL_TEXTURE2);
		m_BlurHTexture->Bind();

		m_GuassianFilterVerticalShader->SetUniform1i("gPosition", 3); //blur filter get object id from gPosition texture
		glActiveTexture(GL_TEXTURE3);
		m_GBuffer->GetPositionTexture()->Bind();



		m_GuassianFilterVerticalShader->SetUniform1i("u_Settings.hdr", s_HDR);
		m_GuassianFilterVerticalShader->SetUniform1i("u_Settings.showBlurArea", s_ShowBlurArea);

		m_GuassianFilterVerticalShader->SetUniform1i("u_Settings.guassian_vertical", s_GuassianVertical);
		m_GuassianFilterVerticalShader->SetUniform1f("u_ScreenWidth", m_ScreenWidth );

		DrawObject(m_QuadFinalScreenObj, m_GuassianFilterVerticalShader);
		m_BlurVertFrameBuffer->UnBind();

		

		/********************************* postProcess pass ************************************************/

		glViewport(0, 0, m_ScreenWidth, m_ScreenHeight);

		glClear(GL_COLOR_BUFFER_BIT);
		m_FinalScreenShader->Bind();
		m_FinalScreenShader->SetUniform1i("u_FinalScreenTexture",1);
		m_FinalScreenShader->SetUniform1f("u_Settings.hdr", s_HDR);
		glActiveTexture(GL_TEXTURE1);
		m_BlurVTexture->Bind();
		DrawObject(m_QuadFinalScreenObj, m_FinalScreenShader);


		/*********************************ÕýÏòäÖÈ¾ light objects ********************************************************/

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		m_GBuffer->Bind();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // Write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, m_ScreenWidth, m_ScreenHeight, 0, 0, m_ScreenWidth, m_ScreenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		glDisable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);

		DrawObject(skybox);
		glDepthFunc(GL_LESS);
		//DrawObjects(backGroundObjs);

		DrawLightSources(lightSources);

	}

	void VoxelConeTracingDeferredRenderer::RenderSpecularBRDFLUTMap()
	{
		m_SpecularBrdfLUTTexture.reset(DBG_NEW Texture(Texture::DiffuseMap, m_VoxelTextureSize, m_VoxelTextureSize, false, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		//std::shared_ptr<Texture> brdfLUTTexture(new Texture(Texture::None, 512, 512, GL_LINEAR, GL_LINEAR, GL_RG16F, GL_RG, GL_CLAMP_TO_EDGE, GL_FLOAT));
		std::shared_ptr<FrameBuffer> frameBuffer(new FrameBuffer());
		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		frameBuffer->Bind();
		frameBuffer->AttachRenderBuffer(m_VoxelTextureSize, m_VoxelTextureSize);

		//m_FrameBuffer->Bind();
		//m_FrameBuffer->BindRenderBuffer();
		frameBuffer->AttachColorTexture(m_SpecularBrdfLUTTexture, 0);
		frameBuffer->BindRenderBuffer();
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);


		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		m_SpecularBRDFLutShader->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_BrdfLUTQuadObj, m_SpecularBRDFLutShader);
		frameBuffer->UnBind();
		frameBuffer->CleanUp();


	}



	void VoxelConeTracingDeferredRenderer::VoxelizeTest(const std::vector<Object*>& objs)
	{


		glViewport(0, 0, m_ScreenWidth,m_ScreenHeight);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		DrawObjects(objs);


		//Shader
		m_VoxelizationTestShader->Bind();


		// Settings.
		glViewport(120, 120, m_VoxelTextureSize, m_VoxelTextureSize);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);


		m_VoxelizationTestShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());


		DrawObjects(objs, m_VoxelizationTestShader);






	}

	



}