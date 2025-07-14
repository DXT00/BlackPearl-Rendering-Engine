#include "pch.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplatePBR.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/Renderer/Material/Material.h"
#include "RHI/RHICommandList.h"
namespace BlackPearl {



    //TODO:: 去掉旧接口 material->GetMaterialColor()，  material->GetProps().roughness;

	void MaterialTemplatePBR::FillMaterialConstants(ICommandList* cmdLIst, Material* material)
	{
        material_cb.flags = 0;
		material_cb.materialID = material->GetId();
        material_cb.shadingModelID = ShadingModel_DefaultLit;
        material_cb.domain = MaterialDomain_Opaque;
     //   material_cb.opacity = 1.0f;
    //    material_cb.alphaThreshold = 0.0f;

	//	  material_cb.roughness = material->GetProps().roughness;
    //    material_cb.metallic = 0.1f;// 0.5f;
	//	  material_cb.ao = 1.0f;
     //   material_cb.specular = 0.5f;// 0.04f;
     //   material_cb.albedo = material->GetMaterialColor().diffuseColor;
     //   material_cb.emissive = material->GetMaterialColor().emissiveColor;
     //   material_cb.transmission = material->GetMaterialColor().subsurfaceColor;
     //   material_cb.ior = 1.0f;//todo


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
