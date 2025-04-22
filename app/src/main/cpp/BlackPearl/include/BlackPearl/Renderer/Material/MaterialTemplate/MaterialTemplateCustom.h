#pragma once
#include "MaterialTemplate.h"

namespace BlackPearl {
	class MaterialTemplateCustom : public MaterialTemplate
	{
	public:

		MaterialTemplateCustom(const std::vector<MaterialResourceBinding>& desc) {

			m_Type = MaterialTemplateType::kCustom;
			m_BindingDesc = desc;
		}
	};

}