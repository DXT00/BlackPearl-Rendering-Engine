#include "pch.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <BlackPearl/Renderer/Shader/Shader.h>
#include "BlackPearl/Renderer/Renderer.h"
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext/matrix_transform.hpp"

namespace BlackPearl {


	Mesh::~Mesh()
	{
	}

	//void Mesh::Draw(const glm::mat4 & model, const LightSources& lightSources)
	//{
	//	std::shared_ptr<Shader> shader = m_Material->GetShader();
	//	shader->Bind();
	//	std::shared_ptr<Material::TextureMaps> textures = m_Material->GetTextureMaps();
	//	unsigned int k = 0;
	//	if (textures != nullptr) {
	//		if (textures->diffuseTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.diffuse", k);
	//			textures->diffuseTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->specularTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.specular", k);
	//			textures->specularTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->emissionTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.emission", k);
	//			textures->emissionTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->heightTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.height", k);
	//			textures->heightTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->normalTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.normal", k);
	//			textures->normalTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->cubeTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.cube", k);
	//			textures->cubeTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->depthTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.depth", k);
	//			textures->depthTextureMap->Bind();
	//			k++;
	//		}
	//	}
	//
	//	MaterialColor::Color materialColor = m_Material->GetMaterialColor().Get();


	//	shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
	//	shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
	//	shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
	//	shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

	//	shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
	//	shader->SetUniform1f("u_Material.shininess", m_Material->GetProps().shininess);
	//	shader->SetUniform1i("u_Material.isBlinnLight", m_Material->GetProps().isBinnLight);
	//	shader->SetUniform1i("u_Material.isTextureSample", m_Material->GetProps().isTextureSample);

	//	glm::mat4 lightProjection, lightView; //TODO;
	//	glm::mat4 lightProjectionViewMatrix;
	//	GLfloat nearPlane = 1.0f, farPlane =7.5f;
	//	glm::vec3 LightPos = { 0.0f, 2.0f, 2.0f };
	//	lightProjection = glm::ortho(-48.0f, 48.0f, -27.0f, 27.0f, nearPlane, farPlane);
	//	lightView = glm::lookAt(LightPos, glm::vec3(0.0f), glm::vec3( 0.0,1.0,0.0));
	//	lightProjectionViewMatrix = lightProjection * lightView;
	//	//m_Shader->Bind();
	//	/*shader->SetUniformMat4f("u_LightProjectionViewMatrix", lightProjectionViewMatrix);
	//	shader->SetUniformVec3f("u_LightPos", LightPos);*/
	//	shader->SetExtraUniform();

	//	shader->SetLightUniform(lightSources);
	//	Renderer::Submit(m_VertexArray, shader, model);
	//	//TODO :: Çø·ÖModelºÍcube
	//	if (m_IndicesSize > 0)
	//		glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
	//	else
	//		glDrawArrays(GL_TRIANGLES, 0, m_VerticesSize / m_VertexBufferLayout.GetStride());

	//	//for (GLuint i = 0; i < k; i++)
	//	//{
	//	//	glActiveTexture(GL_TEXTURE0 + i);
	//	//	//glBindTexture(GL_TEXTURE_2D, 0);
	//	//}
	//}





	////Draw Light
	//void Mesh::Draw(const glm::mat4 & model)
	//{
	//	std::shared_ptr<Shader> shader = m_Material->GetShader();
	//	shader->Bind();
	//	std::shared_ptr<Material::TextureMaps> textures = m_Material->GetTextureMaps();
	//	unsigned int k = 0;
	//	if (textures != nullptr) {
	//		if (textures->diffuseTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);
	//			shader->SetUniform1i("u_Material.diffuse", k);
	//			textures->diffuseTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->specularTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);
	//			shader->SetUniform1i("u_Material.specular", k);
	//			textures->specularTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->emissionTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.emission", k);
	//			textures->emissionTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->heightTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.height", k);
	//			textures->heightTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->normalTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);

	//			shader->SetUniform1i("u_Material.normal", k);
	//			textures->normalTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->cubeTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);
	//			shader->SetUniform1i("u_Material.cube", k);
	//			textures->cubeTextureMap->Bind();
	//			k++;
	//		}
	//		if (textures->depthTextureMap != nullptr) {
	//			glActiveTexture(GL_TEXTURE0 + k);
	//			shader->SetUniform1i("u_Material.depth", k);

	//			textures->depthTextureMap->Bind();
	//			k++;
	//		}
	//	}



	//	MaterialColor::Color materialColor = m_Material->GetMaterialColor().Get();


	//	shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
	//	shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
	//	shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
	//	shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

	//	//shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
	//	//shader->SetUniform1f("u_Material.shininess", 64.0f);
	//	//shader->SetLightUniform(lightSources);


	//	Renderer::Submit(m_VertexArray, shader, model);
	//	glDrawArrays(GL_TRIANGLES, 0, m_VerticesSize / m_VertexBufferLayout.GetStride());
	//	//glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

	//	/*for (GLuint i = 0; i < textures.size(); i++)//TODO::É¾³ýTexture
	//	{
	//		glActiveTexture(GL_TEXTURE0 + i);
	//		glBindTexture(GL_TEXTURE_2D, 0);
	//	}*/
	//}
	void Mesh::Init()
	{

		m_VertexArray.reset(DBG_NEW VertexArray());

		std::shared_ptr<VertexBuffer> vertexBuffer;
		//vertexBuffer.reset(new VertexBuffer(m_Vertices));
		vertexBuffer.reset(DBG_NEW VertexBuffer(m_Vertices, m_VerticesSize));

		if (m_IndicesSize != 0) {
			std::shared_ptr<IndexBuffer> indexBuffer;
			//indexBuffer.reset(new IndexBuffer(m_Indices));
			indexBuffer.reset(DBG_NEW IndexBuffer(m_Indices, m_IndicesSize));
			m_VertexArray->SetIndexBuffer(indexBuffer);
		}


		vertexBuffer->SetBufferLayout(m_VertexBufferLayout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

	}
}