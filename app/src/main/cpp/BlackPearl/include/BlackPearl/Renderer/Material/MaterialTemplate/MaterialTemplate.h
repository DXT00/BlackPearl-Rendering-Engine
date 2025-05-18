#pragma once

#include "../MaterialResouceBinding.h"

namespace BlackPearl {

	enum MaterialTemplateType {
		kBlinPhon,
		kPBR,
		kDisneyPBR,
		kCustom
	};
	class MaterialTemplate
	{
	public:
		MaterialTemplate() = default;
		virtual void FillMaterialConstants(ICommandList* cmdLIst, Material* material) = 0;
		virtual size_t GetMaterialConstantSize() = 0;

		const std::vector<MaterialResourceBinding>& GetBindingDesc() const;
		uint32_t GetRegisterSpace() const;
		virtual void SetRegisterSpace(uint32_t registerSpace);
	protected:
		std::vector<MaterialResourceBinding> m_BindingDesc;
		uint32_t m_RegisterSpace = 0;
		MaterialTemplateType m_Type;
	};

}