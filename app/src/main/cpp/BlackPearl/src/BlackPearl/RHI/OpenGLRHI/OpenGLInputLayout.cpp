#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLInputLayout.h"
namespace BlackPearl {


    void InputLayout::_fillInputDesc(int slot, const BufferElement& element) {
        if (slot != element.Location) {
            GE_CORE_ERROR("mismatch of location");
            return;
        }
        inputDesc[slot].isValid = true;
        inputDesc[slot].bNormalized = element.Normalized;
        inputDesc[slot].location = element.Location;
        inputDesc[slot].elementStride = layout.GetStride();
        inputDesc[slot].offset = element.Offset;
        inputDesc[slot].name = element.Name;
        inputDesc[slot].elementType = element.Type;
        inputDesc[slot].elementCnt = element.GetElementCount();
        inputDesc[slot].elementSizeByte = element.ElementSize;
    }
	InputLayout::InputLayout(const VertexBufferLayout& _layout)
	{
		layout = _layout;
		//inputDesc.resize(layout.ElementSize());

        for (size_t i = 0; i < layout.ElementSize(); i++)
        {
            if (layout.GetElement(i).Name == "aPos") {
                _fillInputDesc(Slot_aPos, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aPrePos") {
                _fillInputDesc(Slot_aPrePos, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aTexCoords") {
                _fillInputDesc(Slot_aTexCoords, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aNormal") {
                _fillInputDesc(Slot_aNormal, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aTangent") {
                _fillInputDesc(Slot_aTangent, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aJointIndices") {
                _fillInputDesc(Slot_aJointIndices, layout.GetElement(i));
            }
            if (layout.GetElement(i).Name == "aJointWeights") {
                _fillInputDesc(Slot_aJointWeights, layout.GetElement(i));
            }
        }


		//for (size_t i = 0; i < inputDesc.size(); i++)
		//{
		//	inputDesc[i].bNormalized = layout.GetElement(i).Normalized;
		//	inputDesc[i].location = layout.GetElement(i).Location;
		//	inputDesc[i].elementStride = layout.GetStride();
		//	inputDesc[i].offset = layout.GetElement(i).Offset;
		//	inputDesc[i].name = layout.GetElement(i).Name;
		//	inputDesc[i].elementType = layout.GetElement(i).Type;
		//	inputDesc[i].elementCnt = layout.GetElement(i).GetElementCount();
		//	inputDesc[i].elementSizeByte = layout.GetElement(i).ElementSize;

		//}


	}

	uint32_t InputLayout::getNumAttributes() const
	{
		return layout.ElementSize();
	}
	const VertexAttributeDesc* InputLayout::getAttributeDesc(uint32_t index) const
	{
		if (index < uint32_t(Slot_Num))
			return &inputDesc[index];
		else
			return nullptr;
	}

}