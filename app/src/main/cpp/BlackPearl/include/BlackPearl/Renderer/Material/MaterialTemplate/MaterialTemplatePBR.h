#pragma once
#include "MaterialTemplate.h"
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

		}
        MaterialConstants material_cb;
	};

}