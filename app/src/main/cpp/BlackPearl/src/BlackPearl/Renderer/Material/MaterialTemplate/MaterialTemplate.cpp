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
}
