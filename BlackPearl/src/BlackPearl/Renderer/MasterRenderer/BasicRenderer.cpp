#include "pch.h"
#include "BasicRenderer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"

#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h"
#include "BlackPearl/Config.h"
namespace BlackPearl {

	BasicRenderer::BasicRenderer()
	{
	}

	BasicRenderer::~BasicRenderer()
	{
	}

	void BasicRenderer::RenderScene(const std::vector<Object*>& objs, const LightSources* lightSources, Renderer::SceneData* scene)
	{
		DrawObjects(objs, scene);
		DrawLightSources(lightSources, scene);

	}

	void BasicRenderer::RenderConfigure(Object* obj)
	{
		//if (obj->HasComponent<MeshRenderer>()) {
		//	auto meshRenderer = obj->GetComponent<MeshRenderer>();
		//	if (meshRenderer->GetEnableCullFace()) {
		//		//glEnable(GL_CULL_FACE);
		//	}
		//	else {
		//		//glDisable(GL_CULL_FACE);

		//	}
		//}
	}

	void BasicRenderer::DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs, Renderer::SceneData* scene)
	{

		for (auto Obj : objs) {
			bool canDraw = true;
			for (auto obj : exceptObjs) {
				if (Obj->GetId().id == obj->GetId().id) {
					canDraw = false;
				}
			}
			if (canDraw)
				DrawObject(Obj, scene);

		}
	}
	void BasicRenderer::DrawObjectsExcept(const std::vector<Object*>& objs, const Object* exceptObj, Renderer::SceneData* scene)
	{

		for (auto obj : objs) {
			if (obj->GetId().id != exceptObj->GetId().id) {
				DrawObject(obj, scene);
			}
		}

	}
	//objs中可以包含Light
	void BasicRenderer::DrawObjects(std::vector<Object*>objs, Renderer::SceneData* scene)
	{
		for (auto obj : objs) {
			DrawObject(obj, scene);
		}

	}
	void BasicRenderer::DrawObjects(std::vector<Object*> objs, std::shared_ptr<Shader> shader, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		for (auto obj : objs) {
			DrawObject(obj, shader, scene);
		}
	}
	//每个Mesh一个shader
	void BasicRenderer::DrawObject(Object* obj, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>() || !obj->GetComponent<MeshRenderer>()->GetEnableRender())
			return;

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		//RenderConfigure(obj);

		for (int i = 0; i < meshes.size(); i++) {
			std::shared_ptr<Shader> shader = meshes[i].GetMaterial()->GetShader();
			shader->Bind();
			GE_ERROR_JUDGE();

			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				shader->SetUniform1i("u_IsPBRObjects", 1);
			}
			else {
				shader->SetUniform1i("u_IsPBRObjects", 0);

			}
			if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>()) {
				PrepareBasicShaderParameters(meshes[i], shader, true, textureBeginIdx);
			}
			else
				PrepareBasicShaderParameters(meshes[i], shader, false, textureBeginIdx);

			Renderer::Submit(meshes[i].GetVertexArray(), shader, transformMatrix, scene);
			if (meshes[i].GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i].GetIndicesSize() / sizeof(unsigned int);
				meshes[i].GetVertexArray()->GetIndexBuffer()->Bind();
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);//0
				meshes[i].GetVertexArray()->GetIndexBuffer()->UnBind();
				meshes[i].GetMaterial()->Unbind();

			}
			else
			{
				meshes[i].GetVertexArray()->UpdateVertexBuffer();
				for (int j = 0; j < meshes[i].GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i].GetVertexArray()->GetVertexBuffers()[j];
					//vertexBuffer->Bind();
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();
					glDrawArrays(GL_TRIANGLES, 0, vertexNum);
					GE_ERROR_JUDGE();

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}

			}
			meshes[i].GetVertexArray()->UnBind();
			meshes[i].GetMaterial()->Unbind();
			shader->Unbind();

		}
	}
	void BasicRenderer::DrawObject(Object* obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>() || !obj->GetComponent<MeshRenderer>()->GetEnableRender())
			return;

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
		GE_ERROR_JUDGE();


		if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
			shader->SetUniform1i("u_IsPBRObjects", 1);
		}
		else {
			shader->SetUniform1i("u_IsPBRObjects", 0);

		}
		GE_ERROR_JUDGE();

		for (int i = 0; i < meshes.size(); i++) {
			if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>())
				PrepareBasicShaderParameters(meshes[i], shader, true, textureBeginIdx);
			else
				PrepareBasicShaderParameters(meshes[i], shader, false, textureBeginIdx);
			GE_ERROR_JUDGE();

			Renderer::Submit(meshes[i].GetVertexArray(), shader, transformMatrix, scene);
			GE_ERROR_JUDGE();

			if (meshes[i].GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i].GetIndicesSize() / sizeof(unsigned int);
				meshes[i].GetVertexArray()->GetIndexBuffer()->Bind();
				GE_ERROR_JUDGE();
				//int count; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &count);
				//GE_CORE_WARN("indicesNum = {0},count = {1}", indicesNum, count / sizeof(GLuint));
				//GE_ERROR_JUDGE();

				//glDrawElements(GL_TRIANGLES, count / sizeof(GLuint), GL_UNSIGNED_INT, 0);
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				//meshes[i].GetVertexArray()->GetIndexBuffer()->UnBind();
			//	meshes[i].GetMaterial()->Unbind();

				GE_ERROR_JUDGE();

			}
			else
			{
				meshes[i].GetVertexArray()->UpdateVertexBuffer();
				for (int j = 0; j < meshes[i].GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i].GetVertexArray()->GetVertexBuffers()[j];
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();
					glDrawArrays(GL_TRIANGLES, 0, vertexNum);
					GE_ERROR_JUDGE();

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
					GE_ERROR_JUDGE();

				}

			}
			meshes[i].GetVertexArray()->UnBind();
			GE_ERROR_JUDGE();
			meshes[i].GetMaterial()->Unbind();
			GE_ERROR_JUDGE();

			shader->Unbind();
			GE_ERROR_JUDGE();

		}
	}
	void BasicRenderer::DrawPointLight(Object* obj, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj->HasComponent<PointLight>(), "obj has no pointlight component!");
		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();


		for (int i = 0; i < meshes.size(); i++) {
			std::shared_ptr<Shader> shader = meshes[i].GetMaterial()->GetShader();
			shader->Bind();
			PrepareBasicShaderParameters(meshes[i], shader, true, textureBeginIdx);
			Renderer::Submit(meshes[i].GetVertexArray(), shader, transformMatrix, scene);
			if (meshes[i].GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i].GetIndicesSize() / sizeof(unsigned int);
				meshes[i].GetVertexArray()->GetIndexBuffer()->Bind();
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				meshes[i].GetVertexArray()->GetIndexBuffer()->UnBind();
				meshes[i].GetMaterial()->Unbind();

			}
			else {
				meshes[i].GetVertexArray()->UpdateVertexBuffer();

				for (int j = 0; j < meshes[i].GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i].GetVertexArray()->GetVertexBuffers()[j];
					glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride());

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}

			}
			meshes[i].GetVertexArray()->UnBind();

			meshes[i].GetMaterial()->Unbind();
			GE_ERROR_JUDGE();

		}

	}
	void BasicRenderer::DrawLightSources(const LightSources* lightSources, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		for (auto lightObj : lightSources->Get()) {
			if (lightObj->HasComponent<PointLight>())
				DrawPointLight(lightObj, scene, textureBeginIdx);
		}
	}

	void BasicRenderer::PrepareBasicShaderParameters(Mesh mesh, std::shared_ptr<Shader> shader, bool isLight, unsigned int textureBeginIdx)
	{
		shader->Bind();

		std::shared_ptr<Material> material = mesh.GetMaterial();
		std::shared_ptr<Material::TextureMaps> textures = material->GetTextureMaps();
		MaterialColor::Color materialColor = material->GetMaterialColor().Get();
		GE_ERROR_JUDGE();


		/************************************* Map settings *****************************************************/

		//k从2开始，0，1号texture用于自定义 texture
		bool diffuseMap = false, specularMap = false, normalMap = false,
			mentallicMap = false, aoMap = false, roughnessMap = false,
			emissionMap = false, cubeMap = false, depthMap = false,
			heightMap = false;
		unsigned int k = textureBeginIdx;
		if (textures != nullptr) {
			if (textures->diffuseTextureMap != nullptr) {
				diffuseMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				GE_ERROR_JUDGE();

				shader->SetUniform1i(ShaderConfig::DIFFUSE_TEXTURE2D, k);
				GE_ERROR_JUDGE();

				textures->diffuseTextureMap->Bind();
				GE_ERROR_JUDGE();

				k++;
				GE_ERROR_JUDGE();
			}
			if (textures->specularTextureMap != nullptr) {
				specularMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::SPECULAR_TEXTURE2D, k);
				textures->specularTextureMap->Bind();
				k++;
				GE_ERROR_JUDGE();
			}
			if (textures->emissionTextureMap != nullptr) {
				emissionMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::EMISSION_TEXTURE2D, k);
				textures->emissionTextureMap->Bind();
				k++;
			}
			if (textures->heightTextureMap != nullptr) {
				heightMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::HEIGHT_TEXTURE2D, k);
				textures->heightTextureMap->Bind();
				float width = textures->heightTextureMap->GetWidth();
				float height = textures->heightTextureMap->GetHeight();

				shader->SetUniformVec2f("u_HeightMapSize", glm::vec2(width, height));

				k++;
			}
			if (textures->normalTextureMap != nullptr) {
				normalMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::NORMAL_TEXTURE2D, k);
				textures->normalTextureMap->Bind();
				k++;
			}
			if (textures->cubeTextureMap != nullptr) {
				cubeMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::CUBE_TEXTURECUBE, k);
				textures->cubeTextureMap->Bind();
				k++;
			}
			if (textures->depthTextureMap != nullptr) {
				depthMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::DEPTH_TEXTURE2D, k);
				textures->depthTextureMap->Bind();
				k++;
			}
			if (textures->aoMap != nullptr) {
				aoMap = true;//TODO::
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::AO_TEXTURE2D, k);
				textures->aoMap->Bind();
				k++;
			}
			if (textures->roughnessMap != nullptr) {
				roughnessMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::ROUGHNESS_TEXTURE2D, k);
				textures->roughnessMap->Bind();
				k++;
			}
			if (textures->mentallicMap != nullptr) {
				mentallicMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::METALLIC_TEXTURE2D, k);
				textures->mentallicMap->Bind();
				k++;
			}
		}
		GE_ERROR_JUDGE();

		/************************************* Color settings *****************************************************/
		{
			shader->SetUniformVec3f(ShaderConfig::DIFFUSE_COLOR, materialColor.diffuseColor);

			shader->SetUniformVec3f(ShaderConfig::SPECULAR_COLOR, materialColor.specularColor);

			shader->SetUniformVec3f(ShaderConfig::AMBIENT_COLOR, materialColor.ambientColor);

			shader->SetUniformVec3f(ShaderConfig::EMISSION_COLOR, materialColor.emissionColor);

			shader->SetUniformVec3f(ShaderConfig::EMISSION_COLOR, materialColor.emissionColor);

			shader->SetUniform1f(ShaderConfig::ROUGHNESS_VALUE, 1.0f);

			shader->SetUniform1f(ShaderConfig::METALLIC_VALUE, 1.0f);

			shader->SetUniform1f(ShaderConfig::AO_VALUE, 1.0f);

			GE_ERROR_JUDGE();


		}
		/************************************* Textures Sample settings *****************************************************/
		{	
		shader->SetUniform1i(ShaderConfig::IS_AMBIENT_TEXTURE_SAMPLE, 0);//TODO::has not ambient map yet

		shader->SetUniform1i(ShaderConfig::IS_DIFFUSE_TEXTURE_SAMPLE, ((diffuseMap || cubeMap) && material->GetProps().isDiffuseTextureSample == 1) ? 1 : 0);

		shader->SetUniform1i(ShaderConfig::IS_SPECULAR_TEXTURE_SAMPLE, (specularMap && material->GetProps().isSpecularTextureSample == 1) ? 1 : 0);

		shader->SetUniform1i(ShaderConfig::IS_PBR_TEXTURE_SAMPLE, (aoMap && roughnessMap && mentallicMap && normalMap && material->GetProps().isPBRTextureSample == 1) ? 1 : 0);

		shader->SetUniform1i(ShaderConfig::IS_EMISSION_TEXTURE_SAMPLE, (emissionMap == true && material->GetProps().isEmissionTextureSample == 1) ? 1 : 0);

		shader->SetUniform1i(ShaderConfig::IS_HEIGHT_TEXTURE_SAMPLE, (heightMap == true && material->GetProps().isHeightTextureSample == 1) ? 1 : 0);

		GE_ERROR_JUDGE();

		}
		/************************************* Others settings *****************************************************/

		{
			shader->SetUniform1f(ShaderConfig::SHININESS, material->GetProps().shininess);
			shader->SetUniform1i(ShaderConfig::IS_BLINNLIGHT, material->GetProps().isBinnLight);
			GE_ERROR_JUDGE();
		}

		/************************************* Lights settings *****************************************************/

		if (!isLight) {
			shader->SetUniform1i("u_Settings.shadows", 1);
			shader->SetLightUniform(Renderer::GetSceneData()->LightSources);
		}
		else {
			shader->SetUniform1i("u_Settings.shadows", 0);

		}
		GE_ERROR_JUDGE();

		

	}


}