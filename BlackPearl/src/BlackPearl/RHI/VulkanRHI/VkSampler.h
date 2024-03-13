#pragma once
#include "../RefCountPtr.h"
#include "../RHISampler.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
	class Sampler :public RefCounter<ISampler>
	{
    public:
        SamplerDesc desc;

        VkSamplerCreateInfo samplerInfo;
        VkSampler sampler;

        explicit Sampler(const VulkanContext& context)
            : m_Context(context)
        { }

        ~Sampler() override;
        const SamplerDesc& getDesc() const override { return desc; }

    private:
        const VulkanContext& m_Context;
	};
}


