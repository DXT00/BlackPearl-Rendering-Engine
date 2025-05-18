#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplateDisneyPBR.h"
namespace BlackPearl {


    void MaterialTemplateDisneyPBR::FillMaterialConstants(ICommandList* cmdLIst, Material* material)
    {
		material_cb.materialID = material->GetId();
		
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

		//todo:: get from IMGUI
		material_cb.baseColor = material->GetMaterialColor().diffuseColor;
		material_cb.subsurface = 0.0;
		material_cb.subsurfaceRadius = 0.0;
		material_cb.subsurfaceColor = material->GetMaterialColor().subsurfaceColor;
		material_cb.metallic = 0.5;
		material_cb.specular = 0.5;
		material_cb.specularTint = 0.0;
		material_cb.roughness = 0.5;
		material_cb.anisotropic = 0.0;
		material_cb.anisotropicRotation = 0.0;
		material_cb.sheen = 0.0;
		material_cb.sheenTint = 0.0;
		material_cb.clearcoat = 0.0;
		material_cb.clearcoatRoughness = 0.0;
		material_cb.ior = 0.0;
		material_cb.transmission = 0.0;
		material_cb.transmissionRoughness = 0.0;
		material_cb.emission = 0.0;
		material_cb.alpha = 1.0;

		cmdLIst->writeBuffer(material->materialConstants,
			&material_cb,
			GetMaterialConstantSize());
    }

    size_t MaterialTemplateDisneyPBR::GetMaterialConstantSize()
    {
        return sizeof(DisneyMaterialConstant);
    }

}
