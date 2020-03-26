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
	void BasicRenderer::DrawObjects(std::vector<Object*> objs, std::shared_ptr<Shader> shader, Renderer::SceneData* scene,unsigned int textureBeginIdx)
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
		std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		//RenderConfigure(obj);

		for (int i = 0; i < meshes.size(); i++) {
			std::shared_ptr<Shader> shader = meshes[i].GetMaterial()->GetShader();
			shader->Bind();
			if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
				shader->SetUniform1i("u_IsPBRObjects",1);
			}
			else {
				shader->SetUniform1i("u_IsPBRObjects", 0);

			}
			if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>()) {
				PrepareBasicShaderParameters(meshes[i], shader, true,textureBeginIdx);
			}
			else
				PrepareBasicShaderParameters(meshes[i], shader,false, textureBeginIdx);

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
					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}

			}
			meshes[i].GetVertexArray()->UnBind();
			meshes[i].GetMaterial()->Unbind();


		}
	}
	void BasicRenderer::DrawObject(Object* obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>() || !obj->GetComponent<MeshRenderer>()->GetEnableRender())
			return;

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		//RenderConfigure(obj);

		if (obj->GetComponent<MeshRenderer>()->GetIsPBRObject()) {
			shader->SetUniform1i("u_IsPBRObjects", 1);
		}
		else {
			shader->SetUniform1i("u_IsPBRObjects", 0);

		}
		for (int i = 0; i < meshes.size(); i++) {
			if (obj->HasComponent<PointLight>() || obj->HasComponent<ParallelLight>() || obj->HasComponent<SpotLight>())
				PrepareBasicShaderParameters(meshes[i], shader, true,textureBeginIdx);
			else
				PrepareBasicShaderParameters(meshes[i], shader,false,textureBeginIdx);

			Renderer::Submit(meshes[i].GetVertexArray(), shader, transformMatrix, scene);
			if (meshes[i].GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i].GetIndicesSize() / sizeof(unsigned int);
				meshes[i].GetVertexArray()->GetIndexBuffer()->Bind();
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				meshes[i].GetVertexArray()->GetIndexBuffer()->UnBind();
				//meshes[i].GetMaterial()->Unbind();


			}
			else
			{
				meshes[i].GetVertexArray()->UpdateVertexBuffer();
				for (int j = 0; j < meshes[i].GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i].GetVertexArray()->GetVertexBuffers()[j];
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();
					glDrawArrays(GL_TRIANGLES, 0, vertexNum);

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}
				//glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

			}
			meshes[i].GetVertexArray()->UnBind();
			meshes[i].GetMaterial()->Unbind();

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
			PrepareBasicShaderParameters(meshes[i], shader, true,textureBeginIdx);
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
				//glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

			}
			meshes[i].GetVertexArray()->UnBind();

			meshes[i].GetMaterial()->Unbind();

		}

	}
	void BasicRenderer::DrawLightSources(const LightSources* lightSources, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		for (auto lightObj : lightSources->Get()) {
			if (lightObj->HasComponent<PointLight>())
				DrawPointLight(lightObj, scene, textureBeginIdx);
		}
	}
	//void BasicRenderer::PrepareShaderParameters(Mesh mesh, std::shared_ptr<Shader> shader, bool isLight)
	//{
	//	PrepareBasicShaderParameters(mesh, shader, isLight);
	//}
	void BasicRenderer::PrepareBasicShaderParameters(Mesh mesh, std::shared_ptr<Shader> shader, bool isLight, unsigned int textureBeginIdx)
	{	//TODO::shader在渲染前Bind()!不在这！
		//还是需要shader->Bind()的！表示使用这个shader!
		//shader->Bind();

		std::shared_ptr<Material> material = mesh.GetMaterial();
		std::shared_ptr<Material::TextureMaps> textures = material->GetTextureMaps();
		MaterialColor::Color materialColor = material->GetMaterialColor().Get();

		//k从4开始，0，1，2，3号texture用于自定义texture
		bool diffuseMap = false, specularMap = false, normalMap = false, mentallicMap = false, aoMap = false;
		unsigned int k = textureBeginIdx;
		if (textures != nullptr) {
			if (textures->diffuseTextureMap != nullptr) {
				diffuseMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::DIFFUSE_TEXTURE2D, k);
				shader->SetUniform1i(ShaderConfig::IS_DIFFUSE_TEXTURE_SAMPLE, 1);
				textures->diffuseTextureMap->Bind();
				k++;
			}
			if (textures->specularTextureMap != nullptr) {
				specularMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::SPECULAR_TEXTURE2D, k);
				shader->SetUniform1i(ShaderConfig::IS_SPECULAR_TEXTURE_SAMPLE, 1);
				textures->specularTextureMap->Bind();
				k++;
			}
			if (textures->emissionTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::EMISSION_TEXTURE2D, k);
				textures->emissionTextureMap->Bind();
				k++;
			}
			if (textures->heightTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);

				shader->SetUniform1i(ShaderConfig::HEIGHT_TEXTURE2D, k);
				textures->heightTextureMap->Bind();
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
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::CUBE_TEXTURECUBE, k);
				textures->cubeTextureMap->Bind();
				k++;
			}
			if (textures->depthTextureMap != nullptr) {
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
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::ROUGHNESS_TEXTURE2D, k);

				textures->roughnessMap->Bind();
				k++;
			}
			if (textures->mentallicMap != nullptr) {
				mentallicMap = true;
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i(ShaderConfig::METALLIC_TEXTURE2D, k);
				shader->SetUniform1i(ShaderConfig::IS_METALLIC_TEXTURE_SAMPLE, 1);


				textures->mentallicMap->Bind();
				k++;
			}
		}
		if (diffuseMap == false) {
			shader->SetUniformVec3f(ShaderConfig::DIFFUSE_COLOR, materialColor.diffuseColor);
			shader->SetUniform1i(ShaderConfig::IS_DIFFUSE_TEXTURE_SAMPLE, 0);
		}
		if (specularMap == false) {
			shader->SetUniformVec3f(ShaderConfig::SPECULAR_COLOR, materialColor.specularColor);
			shader->SetUniform1i(ShaderConfig::IS_SPECULAR_TEXTURE_SAMPLE, 0);
		}
		if (mentallicMap == false) {
			shader->SetUniformVec3f(ShaderConfig::METALLIC_VALUE, materialColor.specularColor);
			shader->SetUniform1i(ShaderConfig::IS_METALLIC_TEXTURE_SAMPLE, 0);
		}
		shader->SetUniformVec3f(ShaderConfig::AMBIENT_COLOR, materialColor.ambientColor);
		shader->SetUniformVec3f(ShaderConfig::EMISSION_COLOR, materialColor.emissionColor);

		shader->SetUniform1f(ShaderConfig::SHININESS, material->GetProps().shininess);
		shader->SetUniform1i(ShaderConfig::IS_BLINNLIGHT, material->GetProps().isBinnLight);
		shader->SetUniform1i(ShaderConfig::IS_TEXTURE_SAMPLE, material->GetProps().isTextureSample);

		if (!material->GetProps().isTextureSample) {

			shader->SetUniform1f("u_Material.roughnessValue", 1.0f);
			shader->SetUniform1f("u_Material.mentallicValue", 0.0f);
			shader->SetUniform1f("u_Material.aoValue", 1.0f);


		}


		shader->SetExtraUniform();
		if (!isLight) {
			shader->SetUniform1i("u_Settings.shadows", 1);

			shader->SetLightUniform(Renderer::GetSceneData()->LightSources);
		}
		else {
			shader->SetUniform1i("u_Settings.shadows", 0);

		}
	}

}