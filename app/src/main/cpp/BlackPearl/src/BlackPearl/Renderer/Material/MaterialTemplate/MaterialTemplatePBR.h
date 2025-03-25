#pragma once
#include "MaterialTemplate.h"

namespace BlackPearl {
	class MaterialTemplatePBR : public MaterialTemplate
	{
	public:

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
	};

}