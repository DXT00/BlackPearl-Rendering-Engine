#include "pch.h"
#include "VoxelConeTracingRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {
	bool VoxelConeTracingRenderer::s_Shadows = true;
	bool VoxelConeTracingRenderer::s_IndirectDiffuseLight = true;
	bool VoxelConeTracingRenderer::s_IndirectSpecularLight = true;
	bool VoxelConeTracingRenderer::s_DirectLight = false;
	float VoxelConeTracingRenderer::s_GICoeffs = 0.8f;
	bool VoxelConeTracingRenderer::s_VoxelizeNow = true;
	bool VoxelConeTracingRenderer::s_HDR = false;
	VoxelConeTracingRenderer::VoxelConeTracingRenderer()
	{

	}
	VoxelConeTracingRenderer::~VoxelConeTracingRenderer()
	{
		if (!m_CubeObj)delete m_CubeObj;
		if (!m_QuadObj)delete m_QuadObj;
		if(!m_VoxelTexture) delete m_VoxelTexture;
	}


	void VoxelConeTracingRenderer::Init(unsigned int viewportWidth, unsigned int viewportHeight, Object * quadObj, Object* brdfLUTQuadObj, Object * cubeObj)
	{
		GE_ASSERT(quadObj, "m_QuadObj is nullptr!");
		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");

		m_QuadObj = quadObj;
		m_CubeObj = cubeObj;

		m_BrdfLUTQuadObj = brdfLUTQuadObj;
		//m_DebugQuadObj = debugQuadObj;
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_MULTISAMPLE);
		m_VoxelConeTracingShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelConeTracing/voxelConeTracingPBR.glsl"));
		InitVoxelization();
		InitVoxelVisualization(viewportWidth, viewportHeight);

		/*debug shader*/
		m_VoxelizationTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/voxelizeTest.glsl"));
		m_FrontBackCubeTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/quadTest.glsl"));

		/*pbr BRDF LUT shader*/
		m_SpecularBRDFLutShader.reset(DBG_NEW Shader("assets/shaders/ibl/brdf.glsl"));
		const std::vector<GLfloat> textureImage2D(4 * 256 * 256 , 0.0f);
		RenderSpecularBRDFLUTMap();


		m_IsInitialize = true;
	}

	void VoxelConeTracingRenderer::InitVoxelization()
	{
		m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelizationPBR.glsl"));
		//m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelization.glsl"));

		const std::vector<GLfloat> texture3D(4 * m_VoxelTextureSize*m_VoxelTextureSize*m_VoxelTextureSize, 0.0f);
		m_VoxelTexture = DBG_NEW Texture3D(texture3D, m_VoxelTextureSize, m_VoxelTextureSize, m_VoxelTextureSize, true);
	}
	void VoxelConeTracingRenderer::InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//Shaders
		m_WorldPositionShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/worldPosition.glsl"));
		m_VoxelVisualizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/voxelVisualization.glsl"));

	}



	void VoxelConeTracingRenderer::Voxilize(const std::vector<Object*>& objs, Object* skybox,bool clearVoxelizationFirst)
	{
		if (clearVoxelizationFirst) {
			float clearColor[4] = { 0.0,0.0,0.0,-1.0 };
			m_VoxelTexture->Clear(clearColor);
		}
		//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	//	m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);


		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// Settings.
		//  disable writing of frame buffer color components
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		
		
		//for (auto obj : objs) {
		//	if (obj->HasComponent<MeshRenderer>())
		//		obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelizationShader);
		//}
		//Render
		//Voxel Texture
	

		if (skybox != nullptr) {

			//glDepthFunc(GL_LEQUAL);
			m_VoxelizationShader->Bind();
			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VoxelizationShader->SetUniform1i("texture3D", 0);
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
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
			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VoxelizationShader->SetUniform1i("texture3D", 0);
			m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
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

	void VoxelConeTracingRenderer::Render(Camera* camera,const std::vector<Object*>& objs, const LightSources * lightSources, unsigned int viewportWidth, unsigned int viewportHeight, Object* skybox,
		RenderingMode reneringMode)
	{
		GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingRenderer::Init() first!");
		//bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		if (s_VoxelizeNow) {
			Voxilize(objs,skybox,true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
		}
		switch (reneringMode) {
		case RenderingMode::VOXELIZATION_VISUALIZATION:
			RenderVoxelVisualization(camera,objs,  viewportWidth, viewportHeight);
			break;
		case RenderingMode::VOXEL_CONE_TRACING:
			//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			//m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			RenderScene(objs, viewportWidth, viewportHeight,skybox);
			break;
		}
	}

	void VoxelConeTracingRenderer::RenderVoxelVisualization(Camera* camera,const std::vector<Object*>& objs,  unsigned int viewportWidth, unsigned int viewportHeight)
	{
		m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);
		m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);

		// -------------------------------------------------------
		// Render 3D texture to screen.
		// -------------------------------------------------------

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, viewportWidth, viewportHeight);
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

	void VoxelConeTracingRenderer::RenderScene(const std::vector<Object*>& objs, unsigned int viewportWidth, unsigned int viewportHeight,Object* skybox)
	{


	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);

		m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);


		

		// GL Settings.
		glViewport(0, 0, viewportWidth, viewportHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	
		
		//TODO::
		float specularReflectivity = 1.0f, diffuseReflectivity = 1.0f, emissivity =0.1f, specularDiffusion = 0.0f;
		float transparency = 0.0f, refractiveIndex = 1.4f;


	//	m_VoxelConeTracingShader->Bind();



		/*for (auto obj : objs) {
			if (obj->HasComponent<MeshRenderer>())
				obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelConeTracingShader);
		}*/
		// Render.
		/*if (skybox != nullptr) {
			glDepthFunc(GL_LEQUAL);
			DrawObject(skybox, m_VoxelConeTracingShader);
			glDepthFunc(GL_LESS);
		}*/
		//m_VoxelConeTracingShader->Bind();
		
		for (auto obj : objs) {
			m_VoxelConeTracingShader->Bind();


			m_VoxelConeTracingShader->SetUniform1i("u_Settings.shadows", s_Shadows);
			m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectDiffuseLight", s_IndirectDiffuseLight);
			m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectSpecularLight", s_IndirectSpecularLight);
			m_VoxelConeTracingShader->SetUniform1i("u_Settings.directLight", s_DirectLight);
			m_VoxelConeTracingShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);
			m_VoxelConeTracingShader->SetUniform1i("u_Settings.hdr", s_HDR);

			m_VoxelConeTracingShader->SetUniform1i("u_State", Configuration::State);

			m_VoxelConeTracingShader->SetUniform1f("u_Material.diffuseReflectivity", diffuseReflectivity);
			m_VoxelConeTracingShader->SetUniform1f("u_Material.specularReflectivity", specularReflectivity);
			m_VoxelConeTracingShader->SetUniform1f("u_Material.emissivity", emissivity);

			m_VoxelConeTracingShader->SetUniform1f("u_Material.specularDiffusion", specularDiffusion);
			m_VoxelConeTracingShader->SetUniform1f("u_Material.transparency", transparency);
			m_VoxelConeTracingShader->SetUniform1f("u_Material.refractiveIndex", refractiveIndex);
			m_VoxelConeTracingShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());


			glActiveTexture(GL_TEXTURE0);
			m_VoxelTexture->Bind();
			m_VoxelConeTracingShader->SetUniform1i("texture3D", 0);

			glActiveTexture(GL_TEXTURE1);
			m_SpecularBrdfLUTTexture->Bind();
			m_VoxelConeTracingShader->SetUniform1i("u_BrdfLUTMap", 1);


			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				m_VoxelConeTracingShader->SetUniform1i("u_IsPBRObjects", 1);
			}
			else {
				m_VoxelConeTracingShader->SetUniform1i("u_IsPBRObjects", 0);

			}
			DrawObject(obj, m_VoxelConeTracingShader);

		}
		//DrawObjects(objs, m_VoxelConeTracingShader);

	}

	void VoxelConeTracingRenderer::RenderSpecularBRDFLUTMap()
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



	void VoxelConeTracingRenderer::VoxelizeTest(const std::vector<Object*>& objs)
	{


		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
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