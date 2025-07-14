#include "pch.h"
#include "Renderer/Material/MaterialTemplate/MaterialTemplate.h"

namespace BlackPearl {
	const std::vector<MaterialResourceBinding>& MaterialTemplate::GetBindingDesc() const
	{
		return m_BindingDesc;
	}
	uint32_t MaterialTemplate::GetRegisterSpace() const
	{
		return m_RegisterSpace;
	}
	void MaterialTemplate::SetRegisterSpace(uint32_t registerSpace)
	{
		m_RegisterSpace = registerSpace;
	}


    std::string MaterialTemplate::ToString() {
        switch (m_Type)
        {
        case BlackPearl::kBlinPhon:
            return "kBlinPhon";
        case BlackPearl::kPBR:
            return "kPBR";

        case BlackPearl::kDisneyPBR:
            return "kDisneyPBR";

        case BlackPearl::kCustom:
            return "kCustom";

        default:
            break;
        }
        return "UnknownTemplate";

    }
}
