#include "pch.h"
#include "Material.h"
#include "MaterialManager.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
namespace BlackPearl {

    extern ShaderFactory* g_shaderFactory;
	extern DeviceManager* g_deviceManager;

	Material::Material(IShader* shader, const std::shared_ptr<TextureMaps>& textureMaps, const MaterialColor& materialColors)
		:m_Shader(shader), m_TextureMaps(textureMaps), m_MaterialColors(materialColors), m_Props(Props())
	{
#if APP_VERSION == APP_VERSION_1_0
		_CreateMaterialConstantBuffer();
#endif
	}
	Material::Material(const std::string shaderPath, const std::shared_ptr<TextureMaps>& textureMaps, 
		math::float3 ambientColor, 
		math::float3 diffuseColor, 
		math::float3 specularColor, 
		math::float3 emissiveColor)
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
				m_MaterialColors.SetEmissionColor(emissiveColor);

			//TODO:: �ع�Shader������OpenGL��DirectX
			if (DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL) {
                std::vector<ShaderMacro> Macros;        
				m_Shader = g_shaderFactory->CreateShader(shaderPath.c_str(), "main", &Macros, ShaderType::AllGraphics);

                //TODO:: shader state set
//                m_Shader->Bind();
//
//				m_Shader->SetUniformVec3f("u_Material.ambientColor", ambientColor);
//
//				m_Shader->SetUniformVec3f("u_Material.diffuseColor", diffuseColor);
//
//				m_Shader->SetUniformVec3f("u_Material.specularColor", specularColor);
//
//				m_Shader->SetUniformVec3f("u_Material.emissiveColor", emissiveColor);


			}
		}
#if APP_VERSION == APP_VERSION_1_0
		_CreateMaterialConstantBuffer();
#endif

	}



	Material::~Material()
	{
	}

	void Material::SetShader(const std::string& shaderPath)
	{
		//m_Shader.reset(DBG_NEW Shader(shaderPath));
        std::vector<ShaderMacro> Macros;
        m_Shader= g_shaderFactory->CreateShader(shaderPath.c_str(), "main", &Macros, ShaderType::AllGraphics);

	}
	void Material::SetShader(IShader* shader)
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

	MaterialConstants Material::FillMaterialConstants()
	{
		MaterialConstants material_cb;
        //MaterialColor::Color color =  m_MaterialColors.Get();
		material_cb.materialID = m_MatId;
		material_cb.diffuseColor =  m_MaterialColors.Get().diffuseColor;
		material_cb.specularColor = m_MaterialColors.Get().specularColor;
		material_cb.ambientColor = m_MaterialColors.Get().ambientColor;
		material_cb.emissiveColor = m_MaterialColors.Get().emissiveColor;
		material_cb.roughnessValue = 0.5f;
		material_cb.metalnessValue = 0.5f;
		material_cb.aoValue = 1.0f;
		material_cb.shininess = 64.0f;

		material_cb.props.isBinnLight = m_Props.isBinnLight;
		material_cb.props.isPBRTextureSample = m_Props.isPBRTextureSample;
		material_cb.props.isDiffuseTextureSample = m_Props.isDiffuseTextureSample;
		material_cb.props.isSpecularTextureSample = m_Props.isSpecularTextureSample;
		material_cb.props.isHeightTextureSample = m_Props.isHeightTextureSample;
		material_cb.props.isEmissionTextureSample = m_Props.isEmissionTextureSample;
		material_cb.props.isRefractMaterial = m_Props.isRefractMaterial;
		material_cb.props.isDoubleSided = m_Props.isDoubleSided;

		if(m_TextureMaps->diffuseTextureMap && material_cb.props.isDiffuseTextureSample)
			material_cb.flags |= MaterialFlags_UseBaseOrDiffuseTexture;
		if (m_TextureMaps->specularTextureMap && material_cb.props.isSpecularTextureSample)
			material_cb.flags |= MaterialFlags_UseSpecularTexture;
		if (m_TextureMaps->emissionTextureMap && material_cb.props.isEmissionTextureSample)
			material_cb.flags |= MaterialFlags_UseEmissiveTexture;

		if (m_TextureMaps->normalTextureMap)
			material_cb.flags |= MaterialFlags_UseNormalTexture;
		if (m_TextureMaps->heightTextureMap && material_cb.props.isHeightTextureSample)
			material_cb.flags |= MaterialFlags_UseHeightMapTexture;
		if (m_TextureMaps->cubeTextureMap)
			material_cb.flags |= MaterialFlags_UseCubeMapTexture;
		if (m_TextureMaps->depthTextureMap)
			material_cb.flags |= MaterialFlags_UseDepthTexture;
		if (m_TextureMaps->aoMap)
			material_cb.flags |= MaterialFlags_UseOcclusionTexture;
		if (m_TextureMaps->roughnessMap)
			material_cb.flags |= MaterialFlags_UseRoughnessTexture;
		if (m_TextureMaps->mentallicMap)
			material_cb.flags |= MaterialFlags_UseMetalTexture;
		if (m_TextureMaps->opacityMap)
			material_cb.flags |= MaterialFlags_UseOpacityTexture;
		if (m_TextureMaps->transmissionTexture)
			material_cb.flags |= MaterialFlags_UseTransmissionTexture;

		return material_cb;
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



}
