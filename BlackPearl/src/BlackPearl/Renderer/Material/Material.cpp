#include "pch.h"
#include "Material.h"


namespace BlackPearl {
	Material::Material(const std::string shaderPath, const std::shared_ptr<TextureMaps>& textureMaps, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, glm::vec3 emissionColor)
	{
		m_Props = Props();
		m_TextureMaps = textureMaps;
		m_Shader.reset(DBG_NEW Shader(shaderPath));
		m_Shader->Bind();
		if (ambientColor.length() != 0) {
			m_MaterialColors.SetAmbientColor(ambientColor);// .push_back(MaterialColor(MaterialColor::Type::AmbientColor, ambientColor));
			m_Shader->SetUniformVec3f("u_Material.ambientColor", ambientColor);

		}
		if (diffuseColor.length() != 0) {
			m_MaterialColors.SetDiffuseColor(diffuseColor);
			m_Shader->SetUniformVec3f("u_Material.diffuseColor", diffuseColor);

		}
		if (specularColor.length() != 0) {
			m_MaterialColors.SetSpecularColor(specularColor);

			m_Shader->SetUniformVec3f("u_Material.specularColor", specularColor);

		}
		if (emissionColor.length() != 0) {
			m_MaterialColors.SetEmissionColor(emissionColor);
			m_Shader->SetUniformVec3f("u_Material.emissionColor", emissionColor);

		}
	}



	Material::~Material()
	{
	}

	void Material::SetShader(std::string shaderPath)
	{
		m_Shader.reset(DBG_NEW Shader(shaderPath));
		
	}

	void Material::SetMaterialColor(MaterialColor::Color color)
	{
		m_MaterialColors.SetColor(color); 
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
		case Texture::Type::CubeMap:
			m_TextureMaps->cubeTextureMap= texture;
			break;
		default:
			break;
		}
	}

	void Material::SetTexture(const Texture::Type type,const std::string& image)
	{
		std::shared_ptr<Texture>texture(DBG_NEW Texture(type, image));
		SetTexture(texture);
	}

	void Material::SetProps(const Props & props)
	{
		m_Props.isBinnLight = props.isBinnLight;
		m_Props.shininess = props.shininess;
	}

	void Material::SetShininess(float shininess)
	{
		m_Props.shininess = shininess;
	}

	void Material::SetBinnLight(bool isBinnLight)
	{
		m_Props.isBinnLight = isBinnLight;
	}

	void Material::SetTextureSample(int isTextureSample)
	{
		m_Props.isTextureSample = isTextureSample;
	}

	

}
