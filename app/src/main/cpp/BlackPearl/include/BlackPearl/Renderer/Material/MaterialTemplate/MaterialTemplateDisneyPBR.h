#pragma once
#include "MaterialTemplate.h"
#include "hlsl/core/material_cb.h"
class ICommandList;

namespace BlackPearl {
    class Material;
	class MaterialTemplateDisneyPBR : public MaterialTemplate
	{
	public:
        virtual void FillMaterialConstants(ICommandList* cmdLIst, Material* material) override;
        virtual size_t GetMaterialConstantSize() override;
        MaterialTemplateDisneyPBR() {

            m_Type = MaterialTemplateType::kDisneyPBR;
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

		}
    public:
        DisneyMaterialConstant material_cb;
	};

}