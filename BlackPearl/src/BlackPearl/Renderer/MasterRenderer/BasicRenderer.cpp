#include "pch.h"
#include "BasicRenderer.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"
#include "BlackPearl/Component/LightComponent/ParallelLight.h"
#include "BlackPearl/Component/LightComponent/PointLight.h"
#include "BlackPearl/Component/LightComponent/SpotLight.h"

#include "BlackPearl/Component/MeshRendererComponent/MeshRenderer.h"
#include "BlackPearl/Component/TransformComponent/Transform.h"
#include "BlackPearl/Component/TerrainComponent/TerrainComponent.h"
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/Renderer.h"
#include "BlackPearl/Renderer/MasterRenderer/ShadowMapPointLightRenderer.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Debugger/D3D12Debugger/HLSLPixDebugger.h"
namespace BlackPearl {
	uint32_t BasicRenderer::s_DrawCallCnt = 0;

	BasicRenderer::BasicRenderer()
	{
		if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
		{
			LoadLibrary(HLSLPixDebugger::GetLatestWinPixGpuCapturerPath_Cpp17().c_str());
		}
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
		std::vector<std::shared_ptr<Mesh>> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		//RenderConfigure(obj);

		for (int i = 0; i < meshes.size(); i++) {
			std::shared_ptr<Shader> shader = meshes[i]->GetMaterial()->GetShader();
			if (!shader) {
				GE_CORE_ERROR(" no shader found!");
				break;
			}
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

			Renderer::Submit(meshes[i]->GetVertexArray(), shader, transformMatrix, scene);
			if (meshes[i]->GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i]->GetIndicesSize() / sizeof(unsigned int);
				meshes[i]->GetVertexArray()->GetIndexBuffer()->Bind();
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				s_DrawCallCnt++;
				meshes[i]->GetVertexArray()->GetIndexBuffer()->UnBind();
				meshes[i]->GetMaterial()->Unbind();

			}
			else
			{
				meshes[i]->GetVertexArray()->UpdateVertexBuffers();
				for (int j = 0; j < meshes[i]->GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i]->GetVertexArray()->GetVertexBuffers()[j];
					//vertexBuffer->Bind();
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();
					glDrawArrays(GL_TRIANGLES, 0, vertexNum);
					s_DrawCallCnt++;
					GE_ERROR_JUDGE();

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}

			}
			meshes[i]->GetVertexArray()->UnBind();
			meshes[i]->GetMaterial()->Unbind();
			shader->Unbind();

		}
	}
	void BasicRenderer::DrawObject(Object* obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>() || !obj->GetComponent<MeshRenderer>()->GetEnableRender())
			return;

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<std::shared_ptr<Mesh>> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
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

			Renderer::Submit(meshes[i]->GetVertexArray(), shader, transformMatrix, scene);
			GE_ERROR_JUDGE();

			if (meshes[i]->GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i]->GetIndicesSize() / sizeof(unsigned int);
				meshes[i]->GetVertexArray()->GetIndexBuffer()->Bind();
				GE_ERROR_JUDGE();
				/*for (size_t v = 0; v < indicesNum; v++)
				{
					GE_CORE_INFO("indexBuffer{0} = {1}\n", v, meshes[i]->GetVertexArray()->GetIndexBuffer()->GetIndicies()[v]);

				}*/

				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				s_DrawCallCnt++;
				GE_ERROR_JUDGE();
				/*meshes[i]->GetVertexArray()->GetIndexBuffer()->UnBind();
				GE_ERROR_JUDGE();*/

			}
			else
			{
				meshes[i]->GetVertexArray()->UpdateVertexBuffers();
				GE_ERROR_JUDGE();
				for (int j = 0; j < meshes[i]->GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i]->GetVertexArray()->GetVertexBuffers()[j];
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();
			


					glDrawArrays(GL_TRIANGLES, 0, vertexNum);
					s_DrawCallCnt++;
					GE_ERROR_JUDGE();

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
					GE_ERROR_JUDGE();

				}

			}
			meshes[i]->GetVertexArray()->UnBind();
			GE_ERROR_JUDGE();
			meshes[i]->GetMaterial()->Unbind();
			GE_ERROR_JUDGE();

			shader->Unbind();
			GE_ERROR_JUDGE();

		}
	}

	void BasicRenderer::DrawObjectVertex(Object* obj, std::shared_ptr<Shader> shader, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj, "obj is empty!");
		if (!obj->HasComponent<MeshRenderer>() || !obj->GetComponent<MeshRenderer>()->GetEnableRender())
			return;

		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<std::shared_ptr<Mesh>> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();
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

			Renderer::Submit(meshes[i]->GetVertexArray(), shader, transformMatrix, scene);
			GE_ERROR_JUDGE();

			/*if (meshes[i]->GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i]->GetIndicesSize() / sizeof(unsigned int);
				meshes[i]->GetVertexArray()->GetIndexBuffer()->Bind();
				GE_ERROR_JUDGE();
				for (size_t v = 0; v < indicesNum; v++)
				{
					GE_CORE_INFO("indexBuffer{0} = {1}\n", v, meshes[i]->GetVertexArray()->GetIndexBuffer()->GetIndicies()[v]);

				}

				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				s_DrawCallCnt++;
				GE_ERROR_JUDGE();
				meshes[i]->GetVertexArray()->GetIndexBuffer()->UnBind();
				GE_ERROR_JUDGE();

			}
			else*/
			{
				meshes[i]->GetVertexArray()->UpdateVertexBuffers();
				GE_ERROR_JUDGE();
				for (int j = 0; j < meshes[i]->GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i]->GetVertexArray()->GetVertexBuffers()[j];
					unsigned int vertexNum = vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride();



					glDrawArrays(GL_TRIANGLES, 0, vertexNum);
					s_DrawCallCnt++;
					GE_ERROR_JUDGE();

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
					GE_ERROR_JUDGE();

				}

			}
			meshes[i]->GetVertexArray()->UnBind();
			GE_ERROR_JUDGE();
			meshes[i]->GetMaterial()->Unbind();
			GE_ERROR_JUDGE();

			shader->Unbind();
			GE_ERROR_JUDGE();

		}
	}

	void BasicRenderer::DiscpatchCompute(uint32_t x, uint32_t y, uint32_t z)
	{
		glDispatchCompute(x, y, z);
		s_DrawCallCnt++;

	}

	void BasicRenderer::DrawBatchNode(BatchNode* node, std::shared_ptr<Shader> shader)
	{
		
		//node->SetRenderState();
		uint32_t batchIndexCnt = node->GetIndexCount();
		shader->Bind();
		/*for (size_t i = 0; i < node->GetObjCnt(); i++)
		{
			shader->SetUniformMat4f("u_Model[" + std::to_string(i) + "]", node->GetObjs()[i]->GetComponent<Transform>()->GetTransformMatrix());
		}*/
		//shader->SetUniformMat3x4f("u_Model", node->GetModelMatrix(), node->GetObjCnt());
		Renderer::Submit(node->GetVertexArray(), shader, node->GetModelMatrix(), node->GetObjCnt());

		//glDrawElements
		node->GetVertexArray()->Bind();
		glDrawElements(GL_TRIANGLES, batchIndexCnt, GL_UNSIGNED_INT, 0);
		s_DrawCallCnt++;
		node->GetVertexArray()->UnBind();


	}

	void BasicRenderer::DrawInstanceNode(InstanceNode* node, std::shared_ptr<Shader> shader)
	{
	}

	void BasicRenderer::DrawSingleNode(SingleNode* node,const std::shared_ptr<Shader>& shader)
	{
	}

	void BasicRenderer::DrawMultiIndirect(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Shader> shader, uint32_t cmdsCnt)
	{
		shader->Bind();
		GE_ERROR_JUDGE();

		Renderer::Submit(vertexArray, shader, nullptr, 0);
		GE_ERROR_JUDGE();

		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (GLvoid*)0, cmdsCnt, 0);
		s_DrawCallCnt++;
	}

	void BasicRenderer::DrawTerrain(Object* obj, std::shared_ptr<Shader> shader, bool drawPolygon)
	{
		GE_ASSERT(obj->HasComponent<TerrainComponent>(), "obj has no terrain component");
		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<std::shared_ptr<Mesh>> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		shader->Bind();
		auto comp = obj->GetComponent<TerrainComponent>();
		if (comp->GetDynamicTess()) {
			shader->SetUniform1i("u_DynamicTessLevel", 1);
		}
		else {
			shader->SetUniform1i("u_DynamicTessLevel", 0);

		}
		shader->SetUniform1i("u_TessLevel", comp->GetStaticTessLevel());

		for (int i = 0; i < meshes.size(); i++) {
			PrepareBasicShaderParameters(meshes[i], shader, false/* is light*/);
			Renderer::Submit(meshes[i]->GetVertexArray(), shader, transformMatrix);
			uint32_t vertexPerChunk = comp->GetVertexPerChunk();
			uint32_t chunkCnt = comp->GetChunkCnt();
			glDrawArrays(GL_PATCHES, 0, vertexPerChunk * chunkCnt);
			s_DrawCallCnt++;

			if (drawPolygon)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}


	void BasicRenderer::DrawPointLight(Object* obj, Renderer::SceneData* scene, unsigned int textureBeginIdx)
	{
		GE_ASSERT(obj->HasComponent<PointLight>(), "obj has no pointlight component!");
		glm::mat4 transformMatrix = obj->GetComponent<Transform>()->GetTransformMatrix();
		std::vector<std::shared_ptr<Mesh>> meshes = obj->GetComponent<MeshRenderer>()->GetMeshes();

		for (int i = 0; i < meshes.size(); i++) {
			std::shared_ptr<Shader> shader = meshes[i]->GetMaterial()->GetShader();
			shader->Bind();
			PrepareBasicShaderParameters(meshes[i], shader, true, textureBeginIdx);
			Renderer::Submit(meshes[i]->GetVertexArray(), shader, transformMatrix, scene);
			if (meshes[i]->GetIndicesSize() > 0) {
				unsigned int indicesNum = meshes[i]->GetIndicesSize() / sizeof(unsigned int);
				meshes[i]->GetVertexArray()->GetIndexBuffer()->Bind();
				glDrawElements(GL_TRIANGLES, indicesNum, GL_UNSIGNED_INT, 0);
				s_DrawCallCnt++;

				meshes[i]->GetVertexArray()->GetIndexBuffer()->UnBind();
				meshes[i]->GetMaterial()->Unbind();

			}
			else {
				meshes[i]->GetVertexArray()->UpdateVertexBuffers();

				for (int j = 0; j < meshes[i]->GetVertexArray()->GetVertexBuffers().size(); j++)
				{
					auto vertexBuffer = meshes[i]->GetVertexArray()->GetVertexBuffers()[j];
					glDrawArrays(GL_TRIANGLES, 0, vertexBuffer->GetVertexSize() / vertexBuffer->GetBufferLayout().GetStride());
					s_DrawCallCnt++;

					for (int index = 0; index < vertexBuffer->GetBufferLayout().GetElements().size(); index++)
					{
						glDisableVertexAttribArray(vertexBuffer->GetBufferLayout().GetElements()[index].Location);
					}
					vertexBuffer->UnBind();
				}

			}
			meshes[i]->GetVertexArray()->UnBind();

			meshes[i]->GetMaterial()->Unbind();
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

	void BasicRenderer::PrepareBasicShaderParameters(std::shared_ptr<Mesh> mesh, std::shared_ptr<Shader> shader, bool isLight, unsigned int textureBeginIdx)
	{
		shader->Bind();

		std::shared_ptr<Material> material = mesh->GetMaterial();
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