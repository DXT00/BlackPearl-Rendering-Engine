#pragma once
#include "../RefCountPtr.h"
#include "../RHIInputLayout.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class InputBindingLayout :public RefCounter<IInputLayout>
	{
    public:
		virtual uint32_t getNumAttributes() const { return 0; };
		virtual const VertexAttributeDesc* getAttributeDesc(uint32_t index) const { return nullptr; };
	};
}


