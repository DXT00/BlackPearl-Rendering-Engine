#pragma once
#include "../RefCountPtr.h"
#include "../RHIBindingSet.h"
#include "../RHIBindingLayoutDesc.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class BindingSet :public RefCounter<IBindingSet>
	{
    public:
        virtual const BindingSetDesc* getDesc() const { return nullptr; };  // returns nullptr for descriptor tables
        virtual IBindingLayout* getLayout() const { return nullptr; }

    private:
	};
}


