#pragma once
#if GE_API_VULKAN

#include "../RefCountPtr.h"
#include "../RHIBindingLayoutDesc.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class BindingLayout :public RefCounter<IBindingLayout>
	{
    public:
        RHIBindingLayoutDesc desc;
        RHIBindlessLayoutDesc bindlessDesc;
        bool isBindless;

        std::vector<VkDescriptorSetLayoutBinding> vulkanLayoutBindings;

        VkDescriptorSetLayout descriptorSetLayout;

        // descriptor pool size information per binding set
        std::vector<VkDescriptorPoolSize> descriptorPoolSizeInfo;

        BindingLayout(const VulkanContext& context, const RHIBindingLayoutDesc& desc);
        BindingLayout(const VulkanContext& context, const RHIBindlessLayoutDesc& desc);
        ~BindingLayout() override;
        const RHIBindingLayoutDesc* getDesc() const override { return isBindless ? nullptr : &desc; }
        const RHIBindlessLayoutDesc* getBindlessDesc() const override { return isBindless ? &bindlessDesc : nullptr; }

        // generate the descriptor set layout
        VkResult bake();

    private:
        const VulkanContext& m_Context;
	};
}


#endif
