#include "pch.h"
#include "Material.h"


namespace BlackPearl {
	Material::Material(const std::string shaderPath, const std::shared_ptr<TextureMaps>& textureMaps, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor)
	{
		m_TextureMaps = textureMaps;
		m_Shader.reset(DBG_NEW Shader(shaderPath));
		m_Shader->Bind();
		if (ambientColor.length() != 0) {
			m_MaterialColors.SetAmbientColor(ambientColor);// .push_back(MaterialColor(MaterialColor::Type::AmbientColor, ambientColor));
			m_Shader->SetUniformVec3f("u_Material.ambientColor", ambientColor);

		}
		if (diffuseColor.length() != 0) {
			m_MaterialColors.SetDiffuseColor(diffuseColor);
			//m_MaterialColors.push_back(MaterialColor(MaterialColor::Type::DiffuseColor, diffuseColor));
			m_Shader->SetUniformVec3f("u_Material.diffuseColor", diffuseColor);

		}
		if (specularColor.length() != 0) {
			m_MaterialColors.SetSpecularColor(specularColor);

			//m_MaterialColors.push_back(MaterialColor(MaterialColor::Type::SpecularColor, specularColor));
			m_Shader->SetUniformVec3f("u_Material.specularColor", specularColor);

		}
		if (emissionColor.length() != 0) {
			m_MaterialColors.SetEmissionColor(emissionColor);

			//m_MaterialColors.push_back(MaterialColor(MaterialColor::Type::EmissionColor, emissionColor));
			m_Shader->SetUniformVec3f("u_Material.emissionColor", emissionColor);

		}
	}



	Material::~Material()
	{
	}

	void Material::SetTexture(const std::shared_ptr<Texture> texture)
	{
		switch (texture->GetType())
		{
		case Texture::Type::DiffuseMap:
			m_TextureMaps->diffuseTextureMap = texture;
			break;
		case Texture::Type::SpecularMap:
			m_TextureMaps->specularTextureMap = texture;
			break;
		case Texture::Type::EmissionMap:
			m_TextureMaps->emissionTextureMap = texture;
			break;
		case Texture::Type::HeightMap:
			m_TextureMaps->heightTextureMap = texture;
			break;
		case Texture::Type::NormalMap:
			m_TextureMaps->normalTextureMap = texture;
			break;

		default:
			break;
		}
	}

	

}
