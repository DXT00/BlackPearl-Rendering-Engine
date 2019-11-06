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
		std::vector<std::shared_ptr<Texture>> textures = m_Material->GetTextureMaps();
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			switch (m_Material->GetTextureMaps()[i]->GetType()) {
			case Texture::Type::DiffuseMap:
				shader->SetUniform1i("u_Material.diffuse", i);
				break;
			case Texture::Type::SpecularMap:
				shader->SetUniform1i("u_Material.specular", i);
				break;

			case Texture::Type::EmissionMap:
				shader->SetUniform1i("u_Material.emission", i);
				break;
			case Texture::Type::NormalMap:
				shader->SetUniform1i("u_Material.normal", i);
				break;
			case Texture::Type::HeightMap:
				shader->SetUniform1i("u_Material.height", i);
				break;
			default:
				GE_CORE_ERROR(" Mesh::Draw failed! Unknown Texture type!!")
					break;

			}
			textures[i]->Bind();

		}
		std::vector<std::shared_ptr<MaterialColor>> materialColor = m_Material->GetMaterialColor();
		for (unsigned int i = 0; i < materialColor.size(); i++) {

			switch (materialColor[i]->GetType()) {
			case MaterialColor::Type::DiffuseColor:
				shader->SetUniformVec3f("u_Material.diffuseColor", materialColor[i]->Get());
				break;
			case MaterialColor::Type::SpecularColor:
				shader->SetUniformVec3f("u_Material.specularColor", materialColor[i]->Get());
				break;

			case MaterialColor::Type::AmbientColor:
				shader->SetUniformVec3f("u_Material.ambientColor", materialColor[i]->Get());
				break;
			default:
				GE_CORE_ERROR(" Mesh::Draw failed! Unknown MaterialColor type!!")
					break;

			}


		}
		shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		shader->SetUniform1f("u_Material.shininess", 64.0f);
		shader->SetLightUniform(lightSources);
		Renderer::Submit(m_VertexArray, shader, model);

		glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	//Draw Light
	void Mesh::Draw(const glm::mat4 & model)
	{
		std::shared_ptr<Shader> shader = m_Material->GetShader();
		shader->Bind();
		std::vector<std::shared_ptr<Texture>> textures = m_Material->GetTextureMaps();
		for (unsigned int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			switch (m_Material->GetTextureMaps()[i]->GetType()) {
			case Texture::Type::DiffuseMap:
				shader->SetUniform1i("u_Material.diffuse", i);
				break;
			case Texture::Type::SpecularMap:
				shader->SetUniform1i("u_Material.specular", i);
				break;

			case Texture::Type::EmissionMap:
				shader->SetUniform1i("u_Material.emission", i);
				break;
			case Texture::Type::NormalMap:
				shader->SetUniform1i("u_Material.normal", i);
				break;
			case Texture::Type::HeightMap:
				shader->SetUniform1i("u_Material.height", i);
				break;
			default:
				GE_CORE_ERROR(" Mesh::Draw failed! Unknown Texture type!!")
					break;

			}
			textures[i]->Bind();

		}
		std::vector<std::shared_ptr<MaterialColor>> materialColor = m_Material->GetMaterialColor();
		for (unsigned int i = 0; i < materialColor.size(); i++) {

			switch (materialColor[i]->GetType()) {
			case MaterialColor::Type::DiffuseColor:
				shader->SetUniformVec3f("u_Material.diffuseColor", materialColor[i]->Get());
				break;
			case MaterialColor::Type::SpecularColor:
				shader->SetUniformVec3f("u_Material.specularColor", materialColor[i]->Get());
				break;

			case MaterialColor::Type::AmbientColor:
				shader->SetUniformVec3f("u_Material.ambientColor", materialColor[i]->Get());
				break;
			default:
				GE_CORE_ERROR(" Mesh::Draw failed! Unknown MaterialColor type!!")
					break;

			}


		}
		//shader->SetUniformMat4f("u_TranInverseModel", glm::transpose(glm::inverse(model)));
		//shader->SetUniform1f("u_Material.shininess", 64.0f);
		//shader->SetLightUniform(lightSources);


		Renderer::Submit(m_VertexArray, shader, model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		//glDrawElements(GL_TRIANGLES, m_IndicesSize / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		for (GLuint i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
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