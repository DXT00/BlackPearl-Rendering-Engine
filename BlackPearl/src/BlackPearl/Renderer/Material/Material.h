#pragma once
#include "BlackPearl/Renderer/Shader/Shader.h"
#include "BlackPearl/Renderer/SceneType.h"
#include "MaterialColor.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/RHI/RHITexture.h"

namespace BlackPearl {
	class Material
	{
	public:
		//用于Raytracing材质判断
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
		struct Props {
			//Enable texture
			float shininess;
			float refractIndex; //电解质系数
			bool  isBinnLight;
			int  isPBRTextureSample;//是否使用纹理-->包括 ao,normal,metalllic,roughness
			int  isDiffuseTextureSample;//是否使用纹理
			int  isSpecularTextureSample;//是否使用纹理
			int  isHeightTextureSample;//是否使用纹理
			int  isEmissionTextureSample;//是否使用纹理
			int isRefractMaterial;
			int isDoubleSided;

			Props() :shininess(64.0f),
				refractIndex(1.5),
				isBinnLight(false), 
				isPBRTextureSample(0),
				isDiffuseTextureSample(0),
				isSpecularTextureSample(0),
				isEmissionTextureSample(0),
				isHeightTextureSample(0),
				isRefractMaterial(0){}

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
		}

		Material(
			const std::shared_ptr<Shader>& shader,
			const std::shared_ptr<TextureMaps>& textureMaps,
			const MaterialColor& materialColors
		);
			

		Material(
			const std::string shaderPath,
			const std::shared_ptr<TextureMaps>& textureMaps,
			math::float3 ambientColor, math::float3 diffuseColor, math::float3 specularColor, math::float3 emissionColor
		);

		~Material();
		//TODO:: 区分opengl和directX shader
		std::shared_ptr<Shader>      GetShader()const { GE_ASSERT(DynamicRHI::g_RHIType == DynamicRHI::Type::OpenGL,"Shader class only support opengl now"); return m_Shader; }
		std::shared_ptr<TextureMaps> GetTextureMaps()const { return m_TextureMaps; }
		MaterialColor                GetMaterialColor()const { return m_MaterialColors; }
		Props                        GetProps() const { return m_Props; }
		RTXType						 GetRTXType() const { return m_RTXType; }

		void SetShader(const std::string& shaderPath);
		void SetShader(const std::shared_ptr<Shader>& shader);
		void SetTexture(ITexture* texture);
		//void SetTexture(const TextureType type, const std::string& image);
		void SetMaterialColor(MaterialColor::Color color);
		void SetMaterialColorDiffuseColor(const math::float3& color);
		void SetMaterialColorSpecularColor(const math::float3& color);
		void SetMaterialColorEmissionColor(const math::float3& color);
		void SetId(uint32_t matId);
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

		MaterialDomain domain = MaterialDomain::Opaque;
		std::shared_ptr<LoadedTexture> baseOrDiffuseTexture; // metal-rough: base color; spec-gloss: diffuse color; .a = opacity (both modes)
		std::shared_ptr<LoadedTexture> metalRoughOrSpecularTexture; // metal-rough: ORM map; spec-gloss: specular color, .a = glossiness
		std::shared_ptr<LoadedTexture> normalTexture;
		std::shared_ptr<LoadedTexture> emissiveTexture;
		std::shared_ptr<LoadedTexture> occlusionTexture;
		std::shared_ptr<LoadedTexture> transmissionTexture; // see KHR_materials_transmission; undefined on specular-gloss materials
		// std::shared_ptr<LoadedTexture> thicknessTexture; // see KHR_materials_volume (not implemented yet)
		BufferHandle materialConstants;

	public:
		std::string name;
		MaterialDomain domain;// = MaterialDomain::Opaque;
	private:
		std::shared_ptr<Shader>		 m_Shader = nullptr;
		std::shared_ptr<TextureMaps> m_TextureMaps = nullptr;
		MaterialColor				 m_MaterialColors;
		Props                        m_Props;
		RTXType						 m_RTXType;
		uint32_t					 m_MatId;
	};

}