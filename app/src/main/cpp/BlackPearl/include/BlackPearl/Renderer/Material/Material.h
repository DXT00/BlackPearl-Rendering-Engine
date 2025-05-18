#pragma once
//#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/RHI/RHIShader.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "MaterialColor.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Renderer/Shader/MaterialShader.h"
#include "hlsl/core/material_cb.h"
#include "MaterialTemplate/MaterialTemplate.h"
#include "MaterialResouceBinding.h"

using namespace BlackPearl::math;


namespace BlackPearl {
	class Material
	{
	public:
		enum RTXType
		{
			RTX_AMBIENT,
			RTX_METALLIC,
			RTX_DIELECTRIC,
			RTX_DIFFUSE,
			RTX_SPECULAR,
			RTX_EMISSION,
			RTX_ROUGHNESS,
			RTX_AO
		};
		
		struct TextureMaps {
			TextureHandle diffuseTextureMap;
			TextureHandle specularTextureMap;
			TextureHandle emissionTextureMap;
			TextureHandle normalTextureMap;
			TextureHandle heightTextureMap;
			TextureHandle cubeTextureMap;
			TextureHandle depthTextureMap;
			TextureHandle aoMap;
			TextureHandle roughnessMap;
			TextureHandle mentallicMap;
			TextureHandle opacityMap;
			TextureHandle transmissionTexture;


			TextureMaps() {
				diffuseTextureMap
				= specularTextureMap
				= emissionTextureMap
				= normalTextureMap
				= heightTextureMap
				= cubeTextureMap
				= depthTextureMap
				= aoMap
				= roughnessMap
				= mentallicMap
				= opacityMap
				= transmissionTexture
				= nullptr;
			}
		};

		//case: for virtual node ,such as RTXTransformNode
		Material()
			: m_Props(Props()) {

				m_Props.shininess = 64.0f;
				m_Props.refractIndex = 1.5;
				m_Props.isBinnLight = false;
				m_Props.isPBRTextureSample = 0;
				m_Props.isDiffuseTextureSample = 0;
				m_Props.isSpecularTextureSample = 0;
				m_Props.isEmissionTextureSample = 0;
				m_Props.isHeightTextureSample = 0;
				m_Props.isRefractMaterial = 0;
		}

		Material(
			MaterialShader* shader,
			const std::shared_ptr<TextureMaps>& textureMaps,
			const MaterialColor& materialColors,
			MaterialTemplateType templateType = MaterialTemplateType::kPBR,
			const std::vector<MaterialResourceBinding>& customBindings
			= std::vector<MaterialResourceBinding>()
		);
			
		Material(
			const std::string shaderPath, 
			const std::shared_ptr<TextureMaps>& textureMaps,
			math::float3 ambientColor, math::float3 diffuseColor, math::float3 specularColor, math::float3 emissiveColor,
			MaterialTemplateType templateType = MaterialTemplateType::kPBR,
			const std::vector<MaterialResourceBinding>& customBindings
			= std::vector<MaterialResourceBinding>()
		);

		~Material();

		void UploadConstantsBuffer(ICommandList* commandList);


		MaterialShader*				 GetShader()const { return m_MaterialShader; }
		std::shared_ptr<TextureMaps> GetTextureMaps()const { return m_TextureMaps; }
		MaterialColor                GetMaterialColor()const { return m_MaterialColors; }
		Props                        GetProps() const { return m_Props; }
		RTXType						 GetRTXType() const { return m_RTXType; }
		SamplerHandle				 GetSampler() const { return m_Sampler; }

		void SetShader(const std::string& shaderPath);
		void SetShader(MaterialShader* shader);
		void SetTexture(ITexture* texture);
		void SetSampler(ISampler* sampler);
		void SetId(uint32_t _matId);
		uint32_t GetId() const;

		void SetProps(const Props& props);
		void SetShininess(float shininess);
		void SetBinnLight(bool isBinnLight);
		void SetPBRTextureSample(int isPBRTextureSample);
		void SetTextureSampleDiffuse(int isTextureSampleDiffuse);
		void SetTextureSampleSpecular(int isTextureSampleSpecular);
		void SetTextureSampleHeight(int isTextureSampleHeight);
		void SetTextureSampleEmission(int isTextureSampleMetallic);
		void SetRefractMaterial(int isRefractMaterial);
		void SetRefractIdx(float idx);
		void SetRTXType(Material::RTXType materialType);

		void Unbind() {
			if (m_TextureMaps->diffuseTextureMap != nullptr)   m_TextureMaps->diffuseTextureMap->UnBind();
			if (m_TextureMaps->specularTextureMap != nullptr)	  m_TextureMaps->specularTextureMap->UnBind();
			if (m_TextureMaps->emissionTextureMap != nullptr)	  m_TextureMaps->emissionTextureMap->UnBind();
			if (m_TextureMaps->normalTextureMap != nullptr)	  m_TextureMaps->normalTextureMap->UnBind();
			if (m_TextureMaps->heightTextureMap != nullptr)	  m_TextureMaps->heightTextureMap->UnBind();
			if (m_TextureMaps->cubeTextureMap != nullptr)  m_TextureMaps->cubeTextureMap->UnBind();
			if (m_TextureMaps->depthTextureMap != nullptr)  m_TextureMaps->depthTextureMap->UnBind();
			if (m_TextureMaps->aoMap != nullptr)  m_TextureMaps->aoMap->UnBind();
			if (m_TextureMaps->roughnessMap != nullptr)	  m_TextureMaps->roughnessMap->UnBind();
			if (m_TextureMaps->mentallicMap != nullptr)	  m_TextureMaps->mentallicMap->UnBind();
			if (m_TextureMaps->opacityMap != nullptr)	  m_TextureMaps->opacityMap->UnBind();
			if (m_TextureMaps->transmissionTexture != nullptr)	  m_TextureMaps->transmissionTexture->UnBind();


		}
	public:
		bool isDirty = true;
		MaterialDomain domain = MaterialDomain::Opaque;
		BufferHandle materialConstants;
		MaterialTemplate* materialTemplate;
		MaterialTemplateType materialTemplateType = MaterialTemplateType::kPBR;
		std::vector<MaterialResourceBinding> customBindingDesc;
		std::string name = "Default_Material";

	private:
		
		MaterialShader*				 m_MaterialShader = nullptr;
		std::shared_ptr<TextureMaps> m_TextureMaps = nullptr;
		MaterialColor				 m_MaterialColors;
		SamplerHandle				 m_Sampler = nullptr;
		Props                        m_Props;
		RTXType						 m_RTXType;
		uint32_t					 m_MatId = 0;

	private:
		void _CreateMaterialConstantBuffer();
		MaterialTemplate* _CreateMaterialTemplate();
	};

}