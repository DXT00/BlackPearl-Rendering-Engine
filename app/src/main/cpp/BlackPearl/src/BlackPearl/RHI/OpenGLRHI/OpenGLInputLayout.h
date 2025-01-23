#pragma once
#include "../RHIInputLayout.h"

namespace BlackPearl {
	class InputLayout :public RefCounter<IInputLayout>
	{
    public:
        std::vector<VertexAttributeDesc> inputDesc;

      /*  std::vector<VkVertexInputBindingDescription> bindingDesc;
        std::vector<VkVertexInputAttributeDescription> attributeDesc;*/

        uint32_t getNumAttributes() const override;
        const VertexAttributeDesc* getAttributeDesc(uint32_t index) const override;
	};
}

