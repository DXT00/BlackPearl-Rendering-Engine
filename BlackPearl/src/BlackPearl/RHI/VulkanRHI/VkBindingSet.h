#pragma once
#include "../RefCountPtr.h"
#include "../RHIBindingSet.h"
#include "../RHIBindingLayoutDesc.h"
#include "VkContext.h"
#include "VkBuffer.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class BindingSet : public RefCounter<IBindingSet>
	{
    public:
        virtual const BindingSetDesc* getDesc() const override { return nullptr; };  // returns nullptr for descriptor tables
        virtual IBindingLayout* getLayout() const override { return nullptr; }


        BindingSetDesc desc;
        BindingLayoutHandle layout;
        // TODO: move pool to the context instead
        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;

        std::vector<ResourceHandle> resources;
        std::vector<Buffer*> volatileConstantBuffers;

        std::vector<uint16_t> bindingsThatNeedTransitions;

        explicit BindingSet(const VulkanContext& context)
            : m_Context(context)
        { }

        ~BindingSet() override;


    private:
        const VulkanContext& m_Context;
	};
}


