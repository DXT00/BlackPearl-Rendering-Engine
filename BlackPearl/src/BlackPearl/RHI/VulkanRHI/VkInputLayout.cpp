#include "pch.h"
#include "VkBindingLayout.h"
#include "VkInputLayout.h"

namespace BlackPearl {
	uint32_t InputLayout::getNumAttributes() const
	{
		return uint32_t(inputDesc.size());
	}
	const VertexAttributeDesc* InputLayout::getAttributeDesc(uint32_t index) const
	{
		if (index < uint32_t(inputDesc.size()))
			return &inputDesc[index];
		else
			return nullptr;
	}
}
