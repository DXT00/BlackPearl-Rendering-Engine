#pragma once
#include "MaterialTemplate.h"

namespace BlackPearl {
	class MaterialTemplateCustom : public MaterialTemplate
	{
	public:
		virtual void FillMaterialConstants(ICommandList* cmdLIst, Material* material) override;
		virtual size_t GetMaterialConstantSize() override;
		MaterialTemplateCustom(const std::vector<MaterialResourceBinding>& desc) {

			m_Type = MaterialTemplateType::kCustom;
			m_BindingDesc = desc;
		}
	};

}