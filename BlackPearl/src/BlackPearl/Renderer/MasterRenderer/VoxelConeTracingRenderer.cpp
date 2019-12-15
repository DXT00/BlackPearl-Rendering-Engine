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
	}


	void VoxelConeTracingRenderer::Init(unsigned int viewportWidth, unsigned int viewportHeight)
	{
		//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glEnable(GL_MULTISAMPLE);
		m_VoxelConeTracingShader.reset(DBG_NEW Shader("assets/shaders/voxelization/voxelConeTracing/voxelConeTracing.glsl"));
		InitVoxelization();
		InitVoxelVisualization(viewportWidth, viewportHeight);
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
		m_FrameBuffer1.reset(DBG_NEW FrameBuffer(viewportWidth, viewportHeight, { FrameBuffer::Attachment::ColorTexture,FrameBuffer::Attachment::RenderBuffer }, false, Texture::Type::None));
		m_FrameBuffer2.reset(DBG_NEW FrameBuffer(viewportWidth, viewportHeight, { FrameBuffer::Attachment::ColorTexture,FrameBuffer::Attachment::RenderBuffer }, false, Texture::Type::None));

	}



	void VoxelConeTracingRenderer::Voxilize(const std::vector<Object*>& objs, Object * m_QuadObj, Object * m_CubeObj, bool clearVoxelizationFirst)
	{
		if (clearVoxelizationFirst) {
			float clearColor[4] = { 0.0,0.0,0.0,0.0 };
			m_VoxelTexture->Clear(clearColor);
		}
		m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
		m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);


		//Shader
		m_VoxelizationShader->Bind();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Settings.
		glViewport(0, 0, m_VoxelTextureSize, m_VoxelTextureSize);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		//Voxel Texture
		glActiveTexture(GL_TEXTURE0 + 0);
		m_VoxelTexture->Bind();
		m_VoxelizationShader->SetUniform1i("texture3D", 0);
		glBindImageTexture(0, m_VoxelTexture->GetRendererID(), 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);


		for (auto obj : objs) {
			if (obj->HasComponent<MeshRenderer>())
				obj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelizationShader);
		}
		//Render
		DrawObjects(objs);



		if (m_AutomaticallyRegenerateMipmap || m_RegenerateMipmapQueued) {
			glGenerateMipmap(GL_TEXTURE_3D);
			m_RegenerateMipmapQueued = false;
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	}

	void VoxelConeTracingRenderer::Render(const std::vector<Object*>& objs, Object * m_QuadObj, Object * m_CubeObj, const LightSources * lightSources, unsigned int viewportWidth, unsigned int viewportHeight, RenderingMode reneringMode)
	{
		bool voxelizeNow = m_VoxelizationQueued || (m_AutomaticallyVoxelize &&m_VoxelizationSparsity > 0 && ++m_TicksSinceLastVoxelization >= m_VoxelizationSparsity);
		if (voxelizeNow) {
			Voxilize(objs, m_QuadObj, m_CubeObj, true);
			m_TicksSinceLastVoxelization = 0;
			m_VoxelizationQueued = false;
		}
		switch (reneringMode) {
		case RenderingMode::VOXELIZATION_VISUALIZATION:
			RenderVoxelVisualization(objs, m_QuadObj, m_CubeObj, viewportWidth, viewportHeight);
			break;
		case RenderingMode::VOXEL_CONE_TRACING:
			m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(false);
			RenderScene(objs, viewportWidth, viewportHeight);
			break;
		}
	}

	void VoxelConeTracingRenderer::RenderVoxelVisualization(const std::vector<Object*>& objs, Object * m_QuadObj, Object * m_CubeObj, unsigned int viewportWidth, unsigned int viewportHeight)
	{
		m_CubeObj->GetComponent<MeshRenderer>()->SetShaders(m_WorldPositionShader);

		m_WorldPositionShader->Bind();
		//Seting
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		//Backs
		glCullFace(GL_FRONT);
		m_FrameBuffer1->Bind();
		glViewport(0, 0, m_FrameBuffer1->GetWidth(), m_FrameBuffer1->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_CubeObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		DrawObject(m_CubeObj);

		//Front
		glCullFace(GL_BACK);
		m_FrameBuffer2->Bind();
		glViewport(0, 0, m_FrameBuffer2->GetWidth(), m_FrameBuffer2->GetHeight());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawObject(m_CubeObj);


		// -------------------------------------------------------
		// Render 3D texture to screen.
		// -------------------------------------------------------

		m_QuadObj->GetComponent<MeshRenderer>()->SetShaders(m_VoxelVisualizationShader);

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_VoxelVisualizationShader->Bind();


		glDisable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glActiveTexture(GL_TEXTURE0 + 0);
		m_FrameBuffer1->BindColorTexture();
		m_VoxelVisualizationShader->SetUniform1i("textureBack", 0);

		glActiveTexture(GL_TEXTURE0 + 1);
		m_FrameBuffer2->BindColorTexture();
		m_VoxelVisualizationShader->SetUniform1i("textureFront", 1);

		glActiveTexture(GL_TEXTURE0 + 2);
		m_VoxelTexture->Bind();
		m_VoxelVisualizationShader->SetUniform1i("texture3D", 2);

		m_VoxelVisualizationShader->SetUniform1i("u_State", Configuration::State);

		// Render.
		glViewport(0, 0, viewportWidth, viewportHeight);
		//glViewport(0, 0, viewportWidth, viewportHeight);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_QuadObj->GetComponent<MeshRenderer>()->SetEnableRender(true);
		DrawObject(m_QuadObj);
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
		float specularReflectivity = 0.2f, diffuseReflectivity = 1.0f, emissivity =0.1f, specularDiffusion = 2.0f;
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

	//void VoxelConeTracingRenderer::DrawCube(Object * obj, std::shared_ptr<Shader> shader)
	//{
	//	glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
	//	std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();



	//	for (int i = 0; i < meshes.size(); i++) {

	//		//PrepareShaderParameters(meshes[i], transformMatrix);

	//		std::shared_ptr<Material> material = meshes[i].GetMaterial();

	//		MaterialColor::Color materialColor = meshes[i].GetMaterial()->GetMaterialColor().Get();

	//		shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
	//		shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
	//		shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
	//		shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);
	//		
	//		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(transformMatrix)));
	//		shader->SetUniform1f("u_Material.shininess", material->GetProps().shininess);
	//		shader->SetUniform1i("u_Material.isBlinnLight", material->GetProps().isBinnLight);
	//		shader->SetUniform1i("u_Material.isTextureSample", material->GetProps().isTextureSample);

	//		if (!obj->HasComponent<PointLight>())
	//			shader->SetLightUniform(Renderer::GetSceneData()->LightSources);

	//		Renderer::Submit(meshes[i].GetVertexArray(), shader, transformMatrix); //meshes[i].GetMaterial()->GetShader()
	//		if (meshes[i].GetIndicesSize() > 0)
	//			glDrawElements(GL_TRIANGLES, meshes[i].GetIndicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	//		else
	//			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

	//	}
	//}

	//void VoxelConeTracingRenderer::DrawQuad(Object * obj, std::shared_ptr<Shader> shader)
	//{
	//	glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
	//	std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();



	//	for (int i = 0; i < meshes.size(); i++) {

	//		//PrepareShaderParameters(meshes[i], transformMatrix);

	//		std::shared_ptr<Material> material = meshes[i].GetMaterial();

	//		MaterialColor::Color materialColor = meshes[i].GetMaterial()->GetMaterialColor().Get();

	//		shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
	//		shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
	//		shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
	//		shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

	//		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(transformMatrix)));
	//		shader->SetUniform1f("u_Material.shininess", material->GetProps().shininess);
	//		shader->SetUniform1i("u_Material.isBlinnLight", material->GetProps().isBinnLight);
	//		shader->SetUniform1i("u_Material.isTextureSample", material->GetProps().isTextureSample);

	//		if (!obj->HasComponent<PointLight>())
	//			shader->SetLightUniform(Renderer::GetSceneData()->LightSources);

	//		Renderer::Submit(meshes[i].GetVertexArray(), meshes[i].GetMaterial()->GetShader(), transformMatrix);
	//		if (meshes[i].GetIndicesSize() > 0)
	//			glDrawElements(GL_TRIANGLES, meshes[i].GetIndicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	//		else
	//			glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

	//	}
	//}
	//

}