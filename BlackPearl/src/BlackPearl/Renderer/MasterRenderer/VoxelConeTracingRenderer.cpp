#include "pch.h"
#include "VoxelConeTracingRenderer.h"
#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {
	VoxelConeTracingRenderer::VoxelConeTracingRenderer()
	{

	}
	VoxelConeTracingRenderer::~VoxelConeTracingRenderer()
	{
		if (!m_CubeObj)delete m_CubeObj;
		if (!m_QuadObj)delete m_QuadObj;
		if(!m_VoxelTexture) delete m_VoxelTexture;
	}


	void VoxelConeTracingRenderer::Init(unsigned int viewportWidth, unsigned int viewportHeight, Object * quadObj, Object * cubeObj)
	{
		GE_ASSERT(quadObj, "m_QuadObj is nullptr!");
		GE_ASSERT(cubeObj, "m_CubeObj is nullptr!");

		m_QuadObj = quadObj;
		m_CubeObj = cubeObj;

		//m_DebugQuadObj = debugQuadObj;
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_MULTISAMPLE);
		m_VoxelConeTracingShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelConeTracing/voxelConeTracing.glsl"));
		InitVoxelization();
		InitVoxelVisualization(viewportWidth, viewportHeight);


		/*debug shader*/
		m_VoxelizationTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/voxelizeTest.glsl"));
		m_FrontBackCubeTestShader.reset(DBG_NEW Shader("assets/shaders/voxelization/debug/quadTest.glsl"));
		m_CubeBackPosTexture.reset(DBG_NEW TextureImage2D(viewportWidth, viewportHeight, GL_LINEAR, GL_LINEAR, GL_RGBA32F, GL_RGBA, GL_CLAMP_TO_EDGE, GL_FLOAT, GL_READ_WRITE));

		m_IsInitialize = true;
	}

	void VoxelConeTracingRenderer::InitVoxelization()
	{
		m_VoxelizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelization.glsl"));
		const std::vector<GLfloat> texture3D(4 * m_VoxelTextureSize*m_VoxelTextureSize*m_VoxelTextureSize, 0.0f);
		m_VoxelTexture = DBG_NEW Texture3D(texture3D, m_VoxelTextureSize, m_VoxelTextureSize, m_VoxelTextureSize, true);
	}
	void VoxelConeTracingRenderer::InitVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//Shaders
		m_WorldPositionShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/worldPosition.glsl"));
		m_VoxelVisualizationShader.reset(DBG_NEW Shader("assets/shaders/voxelization/visualization/voxelVisualization.glsl"));

		//FBOs
		m_FrameBuffer1.reset(DBG_NEW FrameBuffer(viewportWidth, viewportHeight, { FrameBuffer::Attachment::ColorTexture,FrameBuffer::Attachment::RenderBuffer }, 0,false, Texture::Type::None));
		m_FrameBuffer2.reset(DBG_NEW FrameBuffer(viewportWidth, viewportHeight, { FrameBuffer::Attachment::ColorTexture,FrameBuffer::Attachment::RenderBuffer }, 0,false, Texture::Type::None));

	}



	void VoxelConeTracingRenderer::Voxilize(const std::vector<Object*>& objs,bool clearVoxelizationFirst)
	{
		if (clearVoxelizationFirst) {
			float clearColor[4] = { 0.0,0.0,0.0,0.0 };
			m_VoxelTexture->Clear(clearColor);
		}
		//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	//	m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);


		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Settings.
		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		//  disable writing of frame buffer color components
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		//Shader
		m_VoxelizationShader->Bind();

		//Voxel Texture
		glActiveTexture(GL_TEXTURE0 + 0);
		m_VoxelTexture->Bind();
		m_VoxelizationShader->SetUniform1i("texture3D", 0);
		m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
		//m_VoxelizationShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());

		glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);


	/*	for (auto obj : objs) {
			if (obj->HasComponent<MeshRenderer>())
				obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelizationShader);
		}*/
		//Render
		DrawObjects(objs, m_VoxelizationShader);

		if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
			glGenerateMipmap(GL_TEXTURE_3D);
			m_RegenerateMipmapQueued = false;
		}
		//enable writing of frame buffer color components
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	}

	void VoxelConeTracingRenderer::Render(const std::vector<Object*>& objs, const LightSources * lightSources, unsigned int viewportWidth, unsigned int viewportHeight,
		RenderingMode reneringMode)
	{
		GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingRenderer::Init() first!");
		bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		if (voxelizeNow) {
			Voxilize(objs,true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
		}
		switch (reneringMode) {
		case RenderingMode::VOXELIZATION_VISUALIZATION:
			RenderVoxelVisualization(objs,  viewportWidth, viewportHeight);
			break;
		case RenderingMode::VOXEL_CONE_TRACING:
			m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			RenderScene(objs, viewportWidth, viewportHeight);
			break;
		}
	}

	void VoxelConeTracingRenderer::RenderVoxelVisualization(const std::vector<Object*>& objs,  unsigned int viewportWidth, unsigned int viewportHeight)
	{
		m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);
		m_CubeObj->GetComponent<Transform>()->SetPosition(Renderer::GetSceneData()->CameraPosition);
		GE_CORE_INFO("Renderer::GetSceneData()->CameraPosition" + std::to_string(Renderer::GetSceneData()->CameraPosition.x) + "," + std::to_string(Renderer::GetSceneData()->CameraPosition.y) + "," + std::to_string(Renderer::GetSceneData()->CameraPosition.z));
		m_CubeObj->GetComponent<Transform>()->SetRotation(Renderer::GetSceneData()->CameraRotation);

		//Seting
		//glClearColor(0.0, 0.0, 0.0, 1.0);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//Backs
		glCullFace(GL_FRONT);
		m_FrameBuffer1->Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, m_FrameBuffer1->GetWidth(), m_FrameBuffer1->GetHeight());

		//m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		m_WorldPositionShader->Bind();
		m_CubeBackPosTexture->Bind(1);
		m_WorldPositionShader->SetUniform1i("u_Image", 1);
		m_WorldPositionShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
		DrawObject(m_CubeObj, m_WorldPositionShader);

		//Front
		glCullFace(GL_BACK);
		/*m_FrameBuffer2->Bind();
		glViewport(0, 0, m_FrameBuffer2->GetWidth(), m_FrameBuffer2->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_WorldPositionShader->Bind();
		m_WorldPositionShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
		DrawObject(m_CubeObj, m_WorldPositionShader);*/


		// -------------------------------------------------------
		// Render 3D texture to screen.
		// -------------------------------------------------------

	//	m_QuadObj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelVisualizationShader);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render.
		glViewport(0, 0, viewportWidth, viewportHeight);

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		m_VoxelVisualizationShader->Bind();

		glActiveTexture(GL_TEXTURE0);
		m_VoxelTexture->Bind();
	//	glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
		m_VoxelVisualizationShader->SetUniform1i("texture3D", 0);


		glActiveTexture(GL_TEXTURE1);
		//m_FrameBuffer1->BindColorTexture(0);
		m_CubeBackPosTexture->Bind(1);
		m_VoxelVisualizationShader->SetUniform1i("u_Image", 1);

		glActiveTexture(GL_TEXTURE2);
		m_FrameBuffer1->BindColorTexture(0);
		m_VoxelVisualizationShader->SetUniform1i("textureBack", 2);

		

		m_VoxelVisualizationShader->SetUniform1i("u_State", Configuration::State);
		m_VoxelVisualizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

		
		//glViewport(0, 0, viewportWidth, viewportHeight);

	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		DrawObject(m_QuadObj, m_VoxelVisualizationShader);

		

		

	}

	void VoxelConeTracingRenderer::RenderScene(const std::vector<Object*>& objs, unsigned int viewportWidth, unsigned int viewportHeight)
	{

		// Fetch references.
		//auto & camera = *renderingScene.renderingCamera;
		//const Material * material = voxelConeTracingMaterial;
		//const GLuint program = material->program;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_VoxelConeTracingShader->Bind();

		

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

		//// Upload uniforms.
		//uploadCamera(camera, program);
		//uploadGlobalConstants(program, viewportWidth, viewportHeight);
		//uploadLighting(renderingScene, program);
		//uploadRenderingSettings(program);
		
		//TODO::
		float specularReflectivity = 1.0f, diffuseReflectivity = 1.0f, emissivity =0.1f, specularDiffusion = 0.0f;
		float transparency = 0.0f, refractiveIndex = 1.4f;



		m_VoxelConeTracingShader->SetUniform1i("u_Settings.shadows", m_Shadows);
		m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectDiffuseLight", m_IndirectDiffuseLight);
		m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectSpecularLight", m_IndirectSpecularLight);
		m_VoxelConeTracingShader->SetUniform1i("u_Settings.directLight", m_DirectLight);
		m_VoxelConeTracingShader->SetUniform1i("u_State", Configuration::State);

		m_VoxelConeTracingShader->SetUniform1f("u_Material.diffuseReflectivity", diffuseReflectivity);
		m_VoxelConeTracingShader->SetUniform1f("u_Material.specularReflectivity", specularReflectivity);
		m_VoxelConeTracingShader->SetUniform1f("u_Material.emissivity", emissivity);

		m_VoxelConeTracingShader->SetUniform1f("u_Material.specularDiffusion", specularDiffusion);
		m_VoxelConeTracingShader->SetUniform1f("u_Material.transparency", transparency);
		m_VoxelConeTracingShader->SetUniform1f("u_Material.refractiveIndex", refractiveIndex);
		m_VoxelConeTracingShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());


		glActiveTexture(GL_TEXTURE0 + 0);
		m_VoxelTexture->Bind();
		m_VoxelConeTracingShader->SetUniform1i("texture3D", 0);


		for (auto obj : objs) {
			if (obj->HasComponent<MeshRenderer>())
				obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelConeTracingShader);
		}
		// Render.
		DrawObjects(objs);

	}




	//void VoxelConeTracingRenderer::Voxilize(Object* obj, glm::vec3 cameraPos,
	//	bool clearVoxelizationFirst)
	//{
	//	if (clearVoxelizationFirst) {
	//		float clearColor[4] = { 0.0,0.0,0.0,0.0 };
	//		m_VoxelTexture->Clear(clearColor);
	//	}
	//	//m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	//	//m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);

	//	glm::vec3 objPos = obj->GetComponent<Transform>()->GetPosition();
	//	m_CubeObj->GetComponent<Transform>()->SetPosition(objPos);

	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//	// Settings.
	//	glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
	//	//  disable writing of frame buffer color components
	//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	//	glDisable(GL_CULL_FACE);
	//	glDisable(GL_DEPTH_TEST);
	//	glDisable(GL_BLEND);

	//	//Shader
	//	m_VoxelizationShader->Bind();

	//	//Voxel Texture
	//	glActiveTexture(GL_TEXTURE0 + 0);
	//	m_VoxelTexture->Bind();
	//	m_VoxelizationShader->SetUniform1i("texture3D", 0);
	//	m_VoxelizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//	m_VoxelizationShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());

	//	glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

	//	//DrawObject(obj, m_VoxelizationShader);
	//
	//	
	//	DrawObject(obj, m_VoxelizationShader);

	//	if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
	//		glGenerateMipmap(GL_TEXTURE_3D);
	//		m_RegenerateMipmapQueued = false;
	//	}
	//	//enable writing of frame buffer color components
	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	//}

	//void VoxelConeTracingRenderer::Render( Object*obj, const LightSources* lightSources, unsigned int viewportWidth, unsigned int viewportHeight, glm::vec3 cameraPos,
	//	RenderingMode reneringMode)
	//{
	//	GE_ASSERT(m_IsInitialize, "Please Call VoxelConeTracingRenderer::Init() first!");
	//	bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize && m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
	//	if (voxelizeNow) {
	//		Voxilize(obj, true);
	//		m_TicksSinceLastVoxelization = 0;
	//		m_VoxelizationQueued = false;
	//	}
	//	switch (reneringMode) {
	//	case RenderingMode::VOXELIZATION_VISUALIZATION:
	//		RenderVoxelVisualization(obj, viewportWidth, viewportHeight);
	//		break;
	//	case RenderingMode::VOXEL_CONE_TRACING:
	//		m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	//		m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
	//		RenderScene(obj, viewportWidth, viewportHeight);
	//		break;
	//	}
	//}

	//void VoxelConeTracingRenderer::RenderVoxelVisualization( Object* obj, unsigned int viewportWidth, unsigned int viewportHeight)
	//{
	//	m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);

	//	m_WorldPositionShader->Bind();
	//	//Seting
	//	glClearColor(0.0, 0.0, 0.0, 1.0);
	//	glEnable(GL_CULL_FACE);
	//	glEnable(GL_DEPTH_TEST);
	//	//Backs
	//	glCullFace(GL_FRONT);
	//	m_FrameBuffer1->Bind();
	//	glViewport(0, 0, m_FrameBuffer1->GetWidth(), m_FrameBuffer1->GetHeight());
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
	//	DrawObject(m_CubeObj);

	//	//Front
	//	glCullFace(GL_BACK);
	//	m_FrameBuffer2->Bind();
	//	glViewport(0, 0, m_FrameBuffer2->GetWidth(), m_FrameBuffer2->GetHeight());
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	DrawObject(m_CubeObj);


	//	// -------------------------------------------------------
	//	// Render 3D texture to screen.
	//	// -------------------------------------------------------

	////	m_QuadObj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelVisualizationShader);

	//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	// Render.
	//	glViewport(0, 0, viewportWidth, viewportHeight);

	//	glDisable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);

	//	m_VoxelVisualizationShader->Bind();

	//	glActiveTexture(GL_TEXTURE0);
	//	m_FrameBuffer1->BindColorTexture(0);
	//	m_VoxelVisualizationShader->SetUniform1i("textureBack", 0);

	//	glActiveTexture(GL_TEXTURE1);
	//	m_FrameBuffer2->BindColorTexture(0);
	//	m_VoxelVisualizationShader->SetUniform1i("textureFront", 1);

	//	glActiveTexture(GL_TEXTURE2);
	//	m_VoxelTexture->Bind();
	//	m_VoxelVisualizationShader->SetUniform1i("texture3D", 2);

	//	m_VoxelVisualizationShader->SetUniform1i("u_State", Configuration::State);
	//	m_VoxelVisualizationShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//	m_VoxelVisualizationShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());


	//	//glViewport(0, 0, viewportWidth, viewportHeight);

	////	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
	//	DrawObject(m_QuadObj, m_VoxelVisualizationShader);





	//}

	//void VoxelConeTracingRenderer::RenderScene( Object* obj, unsigned int viewportWidth, unsigned int viewportHeight)
	//{

	//	// Fetch references.
	//	//auto & camera = *renderingScene.renderingCamera;
	//	//const Material * material = voxelConeTracingMaterial;
	//	//const GLuint program = material->program;
	//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//	m_VoxelConeTracingShader->Bind();



	//	// GL Settings.
	//	glViewport(0, 0, viewportWidth, viewportHeight);
	//	glClearColor(0.0f, 0.0f, 0.0f, 1.0);
	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	glEnable(GL_DEPTH_TEST);
	//	glEnable(GL_CULL_FACE);
	//	glCullFace(GL_BACK);
	//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//	glEnable(GL_BLEND);
	//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//	//// Upload uniforms.
	//	//uploadCamera(camera, program);
	//	//uploadGlobalConstants(program, viewportWidth, viewportHeight);
	//	//uploadLighting(renderingScene, program);
	//	//uploadRenderingSettings(program);

	//	//TODO::
	//	float specularReflectivity = 1.0f, diffuseReflectivity = 1.0f, emissivity = 0.1f, specularDiffusion = 0.0f;
	//	float transparency = 0.0f, refractiveIndex = 1.4f;



	//	m_VoxelConeTracingShader->SetUniform1i("u_Settings.shadows", m_Shadows);
	//	m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectDiffuseLight", m_IndirectDiffuseLight);
	//	m_VoxelConeTracingShader->SetUniform1i("u_Settings.indirectSpecularLight", m_IndirectSpecularLight);
	//	m_VoxelConeTracingShader->SetUniform1i("u_Settings.directLight", m_DirectLight);
	//	m_VoxelConeTracingShader->SetUniform1i("u_State", Configuration::State);

	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.diffuseReflectivity", diffuseReflectivity);
	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.specularReflectivity", specularReflectivity);
	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.emissivity", emissivity);

	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.specularDiffusion", specularDiffusion);
	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.transparency", transparency);
	//	m_VoxelConeTracingShader->SetUniform1f("u_Material.refractiveIndex", refractiveIndex);
	//	m_VoxelConeTracingShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());
	//	m_VoxelConeTracingShader->SetUniformVec3f("u_CubePos", m_CubeObj->GetComponent<Transform>()->GetPosition());


	//	glActiveTexture(GL_TEXTURE0 + 0);
	//	m_VoxelTexture->Bind();
	//	m_VoxelConeTracingShader->SetUniform1i("texture3D", 0);


	//	/*for (auto obj : objs) {
	//		if (obj->HasComponent<MeshRenderer>())
	//			obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelConeTracingShader);
	//	}*/
	//	// Render.
	//	DrawObject(obj, m_VoxelConeTracingShader);

	//}

	void VoxelConeTracingRenderer::VoxelizeTest(const std::vector<Object*>& objs)
	{
		/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/

		glViewport(0, 0, Configuration::WindowWidth, Configuration::WindowHeight);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		DrawObjects(objs);

		/*if (clearVoxelizationFirst) {
			float clearColor[4] = { 0.0,0.0,0.0,0.0 };
			m_VoxelTexture->Clear(clearColor);
		}
		m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
		m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);*/


		//Shader
		m_VoxelizationTestShader->Bind();


		// Settings.
		glViewport(120, 120, m_VoxelTextureSize, m_VoxelTextureSize);
		//  disable writing of frame buffer color components
		//glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		//Voxel Texture
	/*	glActiveTexture(GL_TEXTURE0 + 0);
		m_VoxelTexture->Bind();
		m_VoxelizationShader->SetUniform1i("texture3D", 0);*/
		m_VoxelizationTestShader->SetUniformVec3f("u_CubeSize", m_CubeObj->GetComponent<Transform>()->GetScale());

		//glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);


	/*	for (auto obj : objs) {
			if (obj->HasComponent<MeshRenderer>())
				obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelizationShader);
		}*/
		//Render
		DrawObjects(objs, m_VoxelizationTestShader);

		/*if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
			glGenerateMipmap(GL_TEXTURE_3D);
			m_RegenerateMipmapQueued = false;
		}*/
		//enable writing of frame buffer color components
		//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);




	}

	void VoxelConeTracingRenderer::DrawFrontBackFaceOfCube(Object* debugQuadObj)
	{

		/* debug */
		/*glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
		debugQuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);

		glViewport(0, 0, 240, 135);//draw back face
		m_FrontBackCubeTestShader->Bind();
		glActiveTexture(GL_TEXTURE3);
		m_FrameBuffer1->BindColorTexture(0);
		m_VoxelVisualizationShader->SetUniform1i("u_CubeFace", 3);
		DrawObject(debugQuadObj,m_FrontBackCubeTestShader);

		glViewport(0, 135, 240, 135);//draw front face
		m_FrontBackCubeTestShader->Bind();
		glActiveTexture(GL_TEXTURE4);
		m_FrameBuffer2->BindColorTexture(0);
		m_VoxelVisualizationShader->SetUniform1i("u_CubeFace", 4);
		DrawObject(debugQuadObj, m_FrontBackCubeTestShader);
	}

	

}