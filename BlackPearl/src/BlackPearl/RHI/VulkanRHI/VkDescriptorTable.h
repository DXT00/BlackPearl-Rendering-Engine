#pragma once
#include "../RHIDescriptorTable.h"
#include "VkContext.h"
namespace BlackPearl {

    class DescriptorTable : public RefCounter<IDescriptorTable>
    {
    public:
        BindingLayoutHandle layout;
        uint32_t capacity = 0;

        VkDescriptorPool descriptorPool;
        VkDescriptorSet descriptorSet;

        explicit DescriptorTable(const VulkanContext& context)
            : m_Context(context)
        { }

        ~DescriptorTable() override;
        const BindingSetDesc* getDesc() const override { return nullptr; }
        IBindingLayout* getLayout() const override { return layout; }
        uint32_t getCapacity() const override { return capacity; }
       // Object getNativeObject(ObjectType objectType) override;

    private:
        const VulkanContext& m_Context;
    };
}