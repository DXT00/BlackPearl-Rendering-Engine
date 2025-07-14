#pragma once
#include "MaterialTemplate.h"
#include "Math/Math.h"
using namespace BlackPearl::math;

#include "hlsl/core/material_cb.h"

namespace BlackPearl {
    class Material; 
    class ICommandList;

	class MaterialTemplatePBR : public MaterialTemplate
	{
	public:
        virtual void FillMaterialConstants(ICommandList* cmdLIst, Material* material) override;
        virtual size_t GetMaterialConstantSize() override;
		MaterialTemplatePBR() {

            m_Type = MaterialTemplateType::kPBR;
			m_BindingDesc = {
               { MaterialResource::ConstantBuffer, 7 },
               { MaterialResource::DiffuseTexture, 0 },
               { MaterialResource::SpecularTexture, 1 },
               { MaterialResource::NormalTexture, 2 },
               { MaterialResource::EmissiveTexture, 3 },
               { MaterialResource::OcclusionTexture, 4 },
               { MaterialResource::TransmissionTexture, 5 },
               { MaterialResource::Sampler, 6 },
            };



            material_cb.flags = 0;
            material_cb.materialID = -1;
            material_cb.shadingModelID = ShadingModel_DefaultLit;
            material_cb.domain = MaterialDomain_Opaque;
            material_cb.opacity = 1.0f;
            material_cb.alphaThreshold = 0.0f;

            material_cb.roughness = 0.5f;
            material_cb.metallic = 0.1f;// 0.5f;
            material_cb.ao = 1.0f;
            material_cb.specular = 0.5f;// 0.04f;
            material_cb.albedo = float3(1.0);
            material_cb.emissive = float3(0.0);
            material_cb.transmission = float3(0.0);
            material_cb.ior = 1.0f;//todo


		}
        //set by imgui or Material api
        MaterialConstants material_cb;
	};

}