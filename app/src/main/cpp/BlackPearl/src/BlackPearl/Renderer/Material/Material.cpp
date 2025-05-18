#include "pch.h"
#include "Renderer/Material/Material.h"
#include "Renderer/Material/MaterialManager.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplatePBR.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplateBlinPhong.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplateCustom.h"
namespace BlackPearl {

    extern ShaderFactory* g_shaderFactory;
	extern DeviceManager* g_deviceManager;

	Material::Material(MaterialShader* shader,
		 const std::shared_ptr<TextureMaps>& textureMaps, const MaterialColor& materialColors, 
		MaterialTemplateType templateType,
		const std::vector<MaterialResourceBinding>& customBindings)
		:m_MaterialShader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors), m_Props(Props()), customBindingDesc(customBindings)
		
	{
//#if APP_VERSION == APP_VERSION_1_0
		_CreateMaterialConstantBuffer();
//#endif
		materialTemplateType = templateType;
		materialTemplate = _CreateMaterialTemplate();

	}
	Material::Material(const std::string shaderPath,const std::shared_ptr<TextureMaps>& textureMaps,
		math::float3 ambientColor, 
		math::float3 diffuseColor, 
		math::float3 specularColor, 
		math::float3 emissiveColor,
		MaterialTemplateType templateType,
		const std::vector<MaterialResourceBinding>& customBindings)
	{
		m_Props = Props();
		m_TextureMaps = textureMaps;
		m_MaterialColors.ambientColor = ambientColor;
		m_MaterialColors.diffuseColor = diffuseColor;
		m_MaterialColors.specularColor = specularColor;
		m_MaterialColors.emissiveColor = emissiveColor;
		if (!shaderPath.empty()) {
			m_MaterialShader = new MaterialShader(shaderPath);
		}
		customBindingDesc = customBindings;
//#if APP_VERSION == APP_VERSION_1_0
		_CreateMaterialConstantBuffer();
//#endif
		materialTemplateType = templateType;
		materialTemplate = _CreateMaterialTemplate();
	}



	Material::~Material()
	{
	}

	void Material::UploadConstantsBuffer(ICommandList* commandList)
	{
		if (materialTemplate) {
			materialTemplate->FillMaterialConstants(commandList, this);
		}
		
	}

	void Material::SetShader(const std::string& shaderPath)
	{
		m_MaterialShader = new MaterialShader(shaderPath);
	}
	void Material::SetShader(MaterialShader* shader)
	{
		m_MaterialShader = shader;

	}

	void Material::SetId(uint32_t _matId)
	{
		m_MatId = _matId;
		name = name + "_" + std::to_string(m_MatId);
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

	void Material::SetSampler(ISampler* sampler)
	{
		m_Sampler = sampler;
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

	
	void Material::_CreateMaterialConstantBuffer()
	{
		BufferDesc bufferDesc;
		bufferDesc.byteSize = sizeof(MaterialConstants);
		bufferDesc.debugName = name;
		bufferDesc.isConstantBuffer = true;
		bufferDesc.initialState = ResourceStates::Common;
		bufferDesc.keepInitialState = true;

		materialConstants =  g_deviceManager->GetDevice()->createBuffer(bufferDesc);
	}

	MaterialTemplate* Material::_CreateMaterialTemplate()
	{
		if (materialTemplateType == MaterialTemplateType::kPBR) {
			return DBG_NEW MaterialTemplatePBR();
		}
		else if (materialTemplateType == MaterialTemplateType::kBlinPhon) {
			return DBG_NEW MaterialTemplateBlinPhong();
		}
		else if (materialTemplateType == MaterialTemplateType::kCustom) {
			GE_CORE_WARN("custom Template need to set material binding desc outside");
			return DBG_NEW MaterialTemplateCustom(customBindingDesc);
		}
		else {
			GE_ASSERT(0, "Unknown materialTemplateType");
		}
	}



}
