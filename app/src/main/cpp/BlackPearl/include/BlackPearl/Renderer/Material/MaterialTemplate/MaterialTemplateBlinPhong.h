#pragma once
#include "MaterialTemplate.h"

namespace BlackPearl {
	class MaterialTemplateBlinPhong : public MaterialTemplate
	{
	public:
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