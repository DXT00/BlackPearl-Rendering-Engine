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
namespace BlackPearl {

	BasicRenderer::BasicRenderer()
	{
	}


	BasicRenderer::~BasicRenderer()
	{
	}
	void BasicRenderer::RenderScene(const std::vector<Object*> &objs, const LightSources* lightSources)
	{

		DrawLightSources(lightSources);
		DrawObjects(objs);

	}

	void BasicRenderer::DrawObjectsExcept(const std::vector<Object*>& objs, const std::vector<Object*>& exceptObjs)
	{

		for (auto Obj : objs) {
			bool canDraw = true;
			for (auto obj : exceptObjs) {
				if (Obj->GetId().id == obj->GetId().id) {
					canDraw = false;
				}
			}
			if (canDraw)
				DrawObject(Obj);

		}
	}
	void BasicRenderer::DrawObjectsExcept(const std::vector<Object*>& objs, const Object * exceptObj)
	{

		for (auto obj : objs) {
			if (obj->GetId().id != exceptObj->GetId().id) {
				DrawObject(obj);
			}
		}

	}
	void BasicRenderer::DrawObjects(std::vector<Object *>objs)
	{
		for (auto obj : objs) {
			DrawObject(obj);
		}

	}
	void BasicRenderer::DrawObject(Object * obj)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>()) {
			GE_CORE_WARN(obj->ToString() + "obj has no meshes! nothing to draw!");
			return;
		}

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		//			std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		for (int i = 0; i < meshes.size(); i++) {

			PrepareShaderParameters(meshes[i], transformMatrix);

			Renderer::Submit(meshes[i].GetVertexArray(), meshes[i].GetMaterial()->GetShader(), transformMatrix);
			if (meshes[i].GetIndicesSize() > 0)
				glDrawElements(GL_TRIANGLES, meshes[i].GetIndicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

		}
	}
	void BasicRenderer::DrawPointLight(Object * obj)
	{
		GE_ASSERT(obj->HasComponent<PointLight>(), "obj has no pointlight component!");
		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<Mesh>& meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
	

		for (int i = 0; i < meshes.size(); i++) {
			PrepareShaderParameters(meshes[i], transformMatrix, true);
			Renderer::Submit(meshes[i].GetVertexArray(), meshes[i].GetMaterial()->GetShader(), transformMatrix);
			if (meshes[i].GetIndicesSize() > 0)
				glDrawElements(GL_TRIANGLES, meshes[i].GetIndicesSize() / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
			else
				glDrawArrays(GL_TRIANGLES, 0, meshes[i].GetVerticesSize() / meshes[i].GetVertexBufferLayout().GetStride());

		}

	}
	void BasicRenderer::DrawLightSources(const LightSources * lightSources)
	{
		for (auto lightObj : lightSources->Get()) {
			if (lightObj->HasComponent<PointLight>())
				DrawPointLight(lightObj);
		}
	}
	void BasicRenderer::PrepareShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight)
	{
		PrepareBasicShaderParameters(mesh, transformMatrix, isLight);
	}
	void BasicRenderer::PrepareBasicShaderParameters(Mesh &mesh, glm::mat4 transformMatrix, bool isLight)
	{
		std::shared_ptr<Shader> shader = mesh.GetMaterial()->GetShader();
		shader->Bind();
		std::shared_ptr<Material> material = mesh.GetMaterial();
		std::shared_ptr<Material::TextureMaps> textures = material->GetTextureMaps();

		unsigned int k = 0;
		if (textures != nullptr) {
			if (textures->diffuseTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i("u_Material.diffuse", k);
				textures->diffuseTextureMap->Bind();
				k++;
			}
			if (textures->specularTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i("u_Material.specular", k);
				textures->specularTextureMap->Bind();
				k++;
			}
			if (textures->emissionTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);

				shader->SetUniform1i("u_Material.emission", k);
				textures->emissionTextureMap->Bind();
				k++;
			}
			if (textures->heightTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);

				shader->SetUniform1i("u_Material.height", k);
				textures->heightTextureMap->Bind();
				k++;
			}
			if (textures->normalTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);

				shader->SetUniform1i("u_Material.normal", k);
				textures->normalTextureMap->Bind();
				k++;
			}
			if (textures->cubeTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i("u_Material.cube", k);
				textures->cubeTextureMap->Bind();
				k++;
			}
			if (textures->depthTextureMap != nullptr) {
				glActiveTexture(GL_TEXTURE0 + k);
				shader->SetUniform1i("u_Material.depth", k);

				textures->depthTextureMap->Bind();
				k++;
			}
		}
		MaterialColor::Color materialColor = mesh.GetMaterial()->GetMaterialColor().Get();

		shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
		shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
		shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
		shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(transformMatrix)));
		shader->SetUniform1f("u_Material.shininess", material->GetProps().shininess);
		shader->SetUniform1i("u_Material.isBlinnLight", material->GetProps().isBinnLight);
		shader->SetUniform1i("u_Material.isTextureSample", material->GetProps().isTextureSample);

		shader->SetExtraUniform();
		if (!isLight)
			shader->SetLightUniform(Renderer::GetSceneData()->LightSources);
	}

}