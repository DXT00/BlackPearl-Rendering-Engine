#pragma once
#if GE_API_VULKAN

#include "../RefCountPtr.h"
#include "../RHIInputLayout.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class InputLayout :public RefCounter<IInputLayout>
	{
    public:
        std::vector<VertexAttributeDesc> inputDesc;

        std::vector<VkVertexInputBindingDescription> bindingDesc;
        std::vector<VkVertexInputAttributeDescription> attributeDesc;

        uint32_t getNumAttributes() const override;
        const VertexAttributeDesc* getAttributeDesc(uint32_t index) const override;
	};
}


#endif