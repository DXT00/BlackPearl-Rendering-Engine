#include "pch.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplatePBR.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "RHI/RHICommandList.h"
namespace BlackPearl {





	void MaterialTemplatePBR::FillMaterialConstants(ICommandList* cmdLIst, Material* material)
	{

		material_cb.materialID = material->GetId();
		material_cb.diffuseColor = material->GetMaterialColor().diffuseColor;
		material_cb.specularColor = material->GetMaterialColor().specularColor;
		material_cb.ambientColor = material->GetMaterialColor().ambientColor;
		material_cb.emissiveColor = material->GetMaterialColor().emissiveColor;
		material_cb.roughness = 0.5f;
		material_cb.metalness = 0.5f;
		material_cb.ao = 1.0f;
		material_cb.shininess = 64.0f;

		//material_cb.props.isBinnLight = m_Props.isBinnLight;
		//material_cb.props.isPBRTextureSample = m_Props.isPBRTextureSample;
		//material_cb.props.isDiffuseTextureSample = m_Props.isDiffuseTextureSample;
		//material_cb.props.isSpecularTextureSample = m_Props.isSpecularTextureSample;
		//material_cb.props.isHeightTextureSample = m_Props.isHeightTextureSample;
		//material_cb.props.isEmissionTextureSample = m_Props.isEmissionTextureSample;
		//material_cb.props.isRefractMaterial = m_Props.isRefractMaterial;
		//material_cb.props.isDoubleSided = m_Props.isDoubleSided;
		auto textureMaps = material->GetTextureMaps();
		if (textureMaps->diffuseTextureMap)
			material_cb.flags |= MaterialFlags_UseBaseOrDiffuseTexture;
		if (textureMaps->specularTextureMap)
			material_cb.flags |= MaterialFlags_UseSpecularTexture;
		if (textureMaps->emissionTextureMap)
			material_cb.flags |= MaterialFlags_UseEmissiveTexture;

		if (textureMaps->normalTextureMap)
			material_cb.flags |= MaterialFlags_UseNormalTexture;
		if (textureMaps->heightTextureMap)
			material_cb.flags |= MaterialFlags_UseHeightMapTexture;
		if (textureMaps->cubeTextureMap)
			material_cb.flags |= MaterialFlags_UseCubeMapTexture;
		if (textureMaps->depthTextureMap)
			material_cb.flags |= MaterialFlags_UseDepthTexture;
		if (textureMaps->aoMap)
			material_cb.flags |= MaterialFlags_UseOcclusionTexture;
		if (textureMaps->roughnessMap)
			material_cb.flags |= MaterialFlags_UseRoughnessTexture;
		if (textureMaps->mentallicMap)
			material_cb.flags |= MaterialFlags_UseMetalTexture;
		if (textureMaps->opacityMap)
			material_cb.flags |= MaterialFlags_UseOpacityTexture;
		if (textureMaps->transmissionTexture)
			material_cb.flags |= MaterialFlags_UseTransmissionTexture;


		cmdLIst->writeBuffer(material->materialConstants,
			&material_cb,
			GetMaterialConstantSize());
	}

	size_t MaterialTemplatePBR::GetMaterialConstantSize()
	{
		return sizeof(MaterialConstants);
	}

}
