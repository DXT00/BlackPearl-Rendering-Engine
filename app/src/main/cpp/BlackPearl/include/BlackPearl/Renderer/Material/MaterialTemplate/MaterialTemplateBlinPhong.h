#pragma once
#include "MaterialTemplate.h"

namespace BlackPearl {
	class MaterialTemplateBlinPhong : public MaterialTemplate
	{
	public:
        virtual void FillMaterialConstants(ICommandList* cmdLIst, Material* material) override;
        virtual size_t GetMaterialConstantSize() override;

		MaterialTemplateBlinPhong() {
            m_Type = MaterialTemplateType::kBlinPhon;
            //TODO::
            m_BindingDesc = {
               { MaterialResource::ConstantBuffer, 7 },
               { MaterialResource::DiffuseTexture, 0 },
               { MaterialResource::SpecularTexture, 1 },
               { MaterialResource::NormalTexture, 2 },
               { MaterialResource::EmissiveTexture, 3 }
            };
		}

	};

}