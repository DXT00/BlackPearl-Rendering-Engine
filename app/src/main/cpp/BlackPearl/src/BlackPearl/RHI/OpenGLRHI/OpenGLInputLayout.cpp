#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLInputLayout.h"
namespace BlackPearl {

	InputLayout::InputLayout(const VertexBufferLayout& _layout)
	{
		layout = _layout;
		inputDesc.resize(layout.ElementSize());
		for (size_t i = 0; i < inputDesc.size(); i++)
		{
			inputDesc[i].bNormalized = layout.GetElement(i).Normalized;
			inputDesc[i].location = layout.GetElement(i).Location;
			inputDesc[i].elementStride = layout.GetStride();
			inputDesc[i].offset = layout.GetElement(i).Offset;
			inputDesc[i].name = layout.GetElement(i).Name;
			inputDesc[i].elementType = layout.GetElement(i).Type;
			inputDesc[i].elementCnt = layout.GetElement(i).GetElementCount();
			inputDesc[i].elementSizeByte = layout.GetElement(i).ElementSize;

		}


	}

	uint32_t InputLayout::getNumAttributes() const
	{
		return layout.ElementSize();
	}
	const VertexAttributeDesc* InputLayout::getAttributeDesc(uint32_t index) const
	{
		if (index < uint32_t(inputDesc.size()))
			return &inputDesc[index];
		else
			return nullptr;
	}

}