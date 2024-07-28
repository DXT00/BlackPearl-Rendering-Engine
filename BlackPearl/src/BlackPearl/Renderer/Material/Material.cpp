#include "pch.h"
#include "Material.h"
#include "MaterialManager.h"
namespace BlackPearl {
	//extern  DynamicRHI::Type g_RHIType;
	Material::Material(const std::shared_ptr<Shader>& shader, const std::shared_ptr<TextureMaps>& textureMaps, const MaterialColor& materialColors)
		:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors), m_Props(Props())
	{
		
	}
	Material::Material(const std::string shaderPath, const std::shared_ptr<TextureMaps>& textureMaps, 
		math::float3 ambientColor, 
		math::float3 diffuseColor, 
		math::float3 specularColor, 
		math::float3 emissionColor)
	{
		m_Props = Props();
		m_TextureMaps = textureMaps;
		if (shaderPath != "") {

			//if (ambientColor.length() != 0)
				m_MaterialColors.SetAmbientColor(ambientColor);
			//if (diffuseColor.length() != 0)
				m_MaterialColors.SetDiffuseColor(diffuseColor);
			//if (specularColor.length() != 0)
				m_MaterialColors.SetSpecularColor(specularColor);
			//if (emissionColor.length() != 0)
				m_MaterialColors.SetEmissionColor(emissionColor);

			//TODO:: 重构Shader，区分OpenGL和DirectX
			if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
				m_Shader.reset(DBG_NEW Shader(shaderPath));
				m_Shader->Bind();
				//if (ambientColor.length() != 0) {
					// .push_back(MaterialColor(MaterialColor::Type::AmbientColor, ambientColor));
					m_Shader->SetUniformVec3f("u_Material.ambientColor", ambientColor);
				//}
				//if (diffuseColor.length() != 0) {					
					m_Shader->SetUniformVec3f("u_Material.diffuseColor", diffuseColor);
				//}
				//if (specularColor.length() != 0) {
					m_Shader->SetUniformVec3f("u_Material.specularColor", specularColor);
				//}
				//if (emissionColor.length() != 0) {
					m_Shader->SetUniformVec3f("u_Material.emissionColor", emissionColor);

				//}
			}
		}


	}



	Material::~Material()
	{
	}

	void Material::SetShader(const std::string& shaderPath)
	{
		m_Shader.reset(DBG_NEW Shader(shaderPath));

	}
	void Material::SetShader(const std::shared_ptr<Shader> &shader)
	{
		m_Shader = shader;

	}
	void Material::SetMaterialColor(MaterialColor::Color color)
	{
		m_MaterialColors.SetColor(color);
	}

	void Material::SetMaterialColorDiffuseColor(const math::float3& color)
	{
		m_MaterialColors.SetDiffuseColor(color);
		//m_Type = Type::DIFFUSE;

	}
	void Material::SetMaterialColorSpecularColor(const math::float3& color)
	{
		m_MaterialColors.SetSpecularColor(color);
		//m_Type = Type::SPECULAR;
	}
	void Material::SetMaterialColorEmissionColor(const math::float3& color)
	{
		m_MaterialColors.SetEmissionColor(color);
		//m_Type = Type::EMISSION;


	}
	void Material::SetId(uint32_t matId)
	{
		m_MatId = matId;
	}
	uint32_t Material::GetId() const
	{
		return m_MatId;
	}
	void Material::SetTexture(ITexture* texture)
	{
		switch (texture->GetType())
		{
		case TextureType::DiffuseMap:
			m_TextureMaps->diffuseTextureMap = texture;
			break;
		case TextureType::SpecularMap:
			m_TextureMaps->specularTextureMap = texture;
			break;
		case TextureType::EmissionMap:
			m_TextureMaps->emissionTextureMap = texture;
			break;
		case TextureType::HeightMap:
			m_TextureMaps->heightTextureMap = texture;
			break;
		case TextureType::NormalMap:
			m_TextureMaps->normalTextureMap = texture;
			break;
		case TextureType::CubeMap:
			m_TextureMaps->cubeTextureMap = texture;
			break;
		case TextureType::DepthMap:
			m_TextureMaps->depthTextureMap = texture;
			break;

		case TextureType::AoMap:
			m_TextureMaps->aoMap = texture;
			break;
		case TextureType::RoughnessMap:
			m_TextureMaps->roughnessMap = texture;
			break;
		case TextureType::MentallicMap:
			m_TextureMaps->mentallicMap = texture;
			break;
		default:
			break;
		}
	}

	/*void Material::SetTexture(const TextureType type, const std::string& image)
	{
		std::shared_ptr<Texture>texture(DBG_NEW Texture(type, image, GL_LINEAR, GL_LINEAR, GL_RGBA, GL_CLAMP_TO_EDGE, GL_UNSIGNED_BYTE));
		SetTexture(texture);
	}*/

	void Material::SetProps(const Props & props)
	{
		m_Props.isBinnLight = props.isBinnLight;
		m_Props.shininess = props.shininess;
		m_Props.refractIndex = props.refractIndex;

	}

	void Material::SetShininess(float shininess)
	{
		m_Props.shininess = shininess;
	}

	void Material::SetRefractIdx(float idx) 
	{
		GE_ASSERT(m_Props.isRefractMaterial, "props.isRefractMaterial = 0");
		m_Props.refractIndex = idx;
	}

	void Material::SetBinnLight(bool isBinnLight)
	{
		m_Props.isBinnLight = isBinnLight;
	}

	void Material::SetPBRTextureSample(int isPBRTextureSample)
	{
		m_Props.isPBRTextureSample = isPBRTextureSample;
	}

	void Material::SetTextureSampleDiffuse(int isTextureSampleDiffuse)
	{
		m_Props.isDiffuseTextureSample = isTextureSampleDiffuse;
	}

	void Material::SetTextureSampleSpecular(int isTextureSampleSpecular)
	{
		m_Props.isSpecularTextureSample = isTextureSampleSpecular;
	}

	void Material::SetTextureSampleHeight(int isTextureSampleHeight)
	{
		m_Props.isHeightTextureSample = isTextureSampleHeight;
	}

	void Material::SetTextureSampleEmission(int isTextureSampleMetallic)
	{
		m_Props.isEmissionTextureSample = isTextureSampleMetallic;

	}

	void Material::SetRefractMaterial(int isRefractMaterial)
	{
		m_Props.isRefractMaterial = isRefractMaterial;
	}

	void Material::SetRTXType(Material::RTXType materialType)
	{
		m_RTXType = materialType;
	}



}
