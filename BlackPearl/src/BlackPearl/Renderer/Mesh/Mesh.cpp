#include "pch.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include <glad/glad.h>
#include <BlackPearl/Renderer/Shader.h>
#include "BlackPearl/Renderer/Renderer.h"

namespace BlackPearl {


	Mesh::~Mesh()
	{
	}

	void Mesh::Draw(const glm::mat4 & model, const LightSources& lightSources)
	{
		std::shared_ptr<Shader> shader = m_Material->GetShader();
		shader->Bind();
		std::shared_ptr<Material::TextureMaps> textures = m_Material->GetTextureMaps();
		unsigned int k = 0;
		if (textures != nullptr) {
			if (textures->diffuseTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.diffuse", k);
				textures->diffuseTextureMap->Bind();
				k++;
			}
			if (textures->specularTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.specular", k);
				textures->specularTextureMap->Bind();
				k++;
			}
			if (textures->emissionTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.emission", k);
				textures->emissionTextureMap->Bind();
				k++;
			}
			if (textures->heightTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.height", k);
				textures->heightTextureMap->Bind();
				k++;
			}
			if (textures->normalTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.normal", k);
				textures->normalTextureMap->Bind();
				k++;
			}
		}
		//std::vector<std::shared_ptr<Texture>> textures = m_Material->GetTextureMaps();
		//for (unsigned int i = 0; i < textures.size(); i++) {
		//	glActiveTexture(GL_TEXTURE0 + i);

		//	switch (m_Material->GetTextureMaps()[i]->GetType()) {
		//	case Texture::Type::DiffuseMap:
		//		shader->SetUniform1i("u_Material.diffuse", i);
		//		break;
		//	case Texture::Type::SpecularMap:
		//		shader->SetUniform1i("u_Material.specular", i);
		//		break;

		//	case Texture::Type::EmissionMap:
		//		shader->SetUniform1i("u_Material.emission", i);
		//		break;
		//	case Texture::Type::NormalMap:
		//		shader->SetUniform1i("u_Material.normal", i);
		//		break;
		//	case Texture::Type::HeightMap:
		//		shader->SetUniform1i("u_Material.height", i);
		//		break;
		//	default:
		//		GE_CORE_ERROR(" Mesh::Draw failed! Unknown Texture type!!")
		//			break;

		//	}
		//	textures[i]->Bind();

		//}
		MaterialColor::Color materialColor = m_Material->GetMaterialColor().Get();


		shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
		shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
		shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
		shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		shader->SetUniform1f("u_Material.shininess", 64.0f);
		shader->SetLightUniform(lightSources);
		Renderer::Submit(m_VertexArray, shader, model);
		//TODO :: Çø·ÖModelºÍcube
		if (m_IndicesSize > 0)
			glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, m_VerticesSize / m_VertexBufferLayout.GetStride());

		/*for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}*/
	}

	void Mesh::SetTexture(const std::shared_ptr<Texture> texture)
	{
		m_Material->SetTexture(texture);
	}



	//Draw Light
	void Mesh::Draw(const glm::mat4 & model)
	{
		std::shared_ptr<Shader> shader = m_Material->GetShader();
		shader->Bind();
		std::shared_ptr<Material::TextureMaps> textures = m_Material->GetTextureMaps();
		unsigned int k = 0;
		if (textures != nullptr) {
			if (textures->diffuseTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.diffuse", k);
				textures->diffuseTextureMap->Bind();
				k++;
			}
			if (textures->specularTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.specular", k);
				textures->specularTextureMap->Bind();
				k++;
			}
			if (textures->emissionTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.emission", k);
				textures->emissionTextureMap->Bind();
				k++;
			}
			if (textures->heightTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.height", k);
				textures->heightTextureMap->Bind();
				k++;
			}
			if (textures->normalTextureMap != nullptr) {
				shader->SetUniform1i("u_Material.normal", k);
				textures->normalTextureMap->Bind();
				k++;
			}
		}


		//for (unsigned int i = 0; i < textures.size(); i++) {
		//	glActiveTexture(GL_TEXTURE0 + i);

		//	switch (m_Material->GetTextureMaps()[i]->GetType()) {
		//	case Texture::Type::DiffuseMap:
		//		shader->SetUniform1i("u_Material.diffuse", i);
		//		break;
		//	case Texture::Type::SpecularMap:
		//		shader->SetUniform1i("u_Material.specular", i);
		//		break;

		//	case Texture::Type::EmissionMap:
		//		shader->SetUniform1i("u_Material.emission", i);
		//		break;
		//	case Texture::Type::NormalMap:
		//		shader->SetUniform1i("u_Material.normal", i);
		//		break;
		//	case Texture::Type::HeightMap:
		//		shader->SetUniform1i("u_Material.height", i);
		//		break;
		//	default:
		//		GE_CORE_ERROR(" Mesh::Draw failed! Unknown Texture type!!")
		//			break;

		//	}
			//textures[i]->Bind();

	//	}

		MaterialColor::Color materialColor = m_Material->GetMaterialColor().Get();


		shader->SetUniformVec3f("u_Material.diffuseColor", materialColor.diffuseColor);
		shader->SetUniformVec3f("u_Material.specularColor", materialColor.specularColor);
		shader->SetUniformVec3f("u_Material.ambientColor", materialColor.ambientColor);
		shader->SetUniformVec3f("u_Material.emissionColor", materialColor.emissionColor);

		//shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		//shader->SetUniform1f("u_Material.shininess", 64.0f);
		//shader->SetLightUniform(lightSources);


		Renderer::Submit(m_VertexArray, shader, model);
		glDrawArrays(GL_TRIANGLES, 0, m_VerticesSize / m_VertexBufferLayout.GetStride());
		//glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

		/*for (GLuint i = 0; i < textures.size(); i++)//TODO::É¾³ýTexture
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}*/
	}
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