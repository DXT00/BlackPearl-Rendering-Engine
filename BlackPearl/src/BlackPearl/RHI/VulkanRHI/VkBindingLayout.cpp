#include "pch.h"
#include "VkBindingLayout.h"

namespace BlackPearl {
	BindingLayout::BindingLayout(const VulkanContext& context, const RHIBindingLayoutDesc& desc)
		: desc(desc),
		isBindless(false),
		m_Context(context)
	{
        //VkShaderStageFlagBits shaderStageFlags = convertShaderTypeToShaderStageFlagBits(desc.visibility);

        //// iterate over all binding types and add to map
        //for (const BindingLayoutItem& binding : desc.bindings)
        //{
        //    vk::DescriptorType descriptorType;
        //    uint32_t descriptorCount = 1;
        //    uint32_t registerOffset;

        //    switch (binding.type)
        //    {
        //    case ResourceType::Texture_SRV:
        //        registerOffset = _desc.bindingOffsets.shaderResource;
        //        descriptorType = vk::DescriptorType::eSampledImage;
        //        break;

        //    case ResourceType::Texture_UAV:
        //        registerOffset = _desc.bindingOffsets.unorderedAccess;
        //        descriptorType = vk::DescriptorType::eStorageImage;
        //        break;

        //    case ResourceType::TypedBuffer_SRV:
        //        registerOffset = _desc.bindingOffsets.shaderResource;
        //        descriptorType = vk::DescriptorType::eUniformTexelBuffer;
        //        break;

        //    case ResourceType::StructuredBuffer_SRV:
        //    case ResourceType::RawBuffer_SRV:
        //        registerOffset = _desc.bindingOffsets.shaderResource;
        //        descriptorType = vk::DescriptorType::eStorageBuffer;
        //        break;

        //    case ResourceType::TypedBuffer_UAV:
        //        registerOffset = _desc.bindingOffsets.unorderedAccess;
        //        descriptorType = vk::DescriptorType::eStorageTexelBuffer;
        //        break;

        //    case ResourceType::StructuredBuffer_UAV:
        //    case ResourceType::RawBuffer_UAV:
        //        registerOffset = _desc.bindingOffsets.unorderedAccess;
        //        descriptorType = vk::DescriptorType::eStorageBuffer;
        //        break;

        //    case ResourceType::ConstantBuffer:
        //        registerOffset = _desc.bindingOffsets.constantBuffer;
        //        descriptorType = vk::DescriptorType::eUniformBuffer;
        //        break;

        //    case ResourceType::VolatileConstantBuffer:
        //        registerOffset = _desc.bindingOffsets.constantBuffer;
        //        descriptorType = vk::DescriptorType::eUniformBufferDynamic;
        //        break;

        //    case ResourceType::Sampler:
        //        registerOffset = _desc.bindingOffsets.sampler;
        //        descriptorType = vk::DescriptorType::eSampler;
        //        break;

        //    case ResourceType::PushConstants:
        //        // don't need any descriptors for the push constants, but the vulkanLayoutBindings array 
        //        // must match the binding layout items for further processing within nvrhi --
        //        // so set descriptorCount to 0 instead of skipping it
        //        registerOffset = _desc.bindingOffsets.constantBuffer;
        //        descriptorType = vk::DescriptorType::eUniformBuffer;
        //        descriptorCount = 0;
        //        break;

        //    case ResourceType::RayTracingAccelStruct:
        //        registerOffset = _desc.bindingOffsets.shaderResource;
        //        descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
        //        break;

        //    case ResourceType::None:
        //    case ResourceType::Count:
        //    default:
        //        utils::InvalidEnum();
        //        continue;
        //    }

        //    const auto bindingLocation = registerOffset + binding.slot;

        //    vk::DescriptorSetLayoutBinding descriptorSetLayoutBinding = vk::DescriptorSetLayoutBinding()
        //        .setBinding(bindingLocation)
        //        .setDescriptorCount(descriptorCount)
        //        .setDescriptorType(descriptorType)
        //        .setStageFlags(shaderStageFlags);

        //    vulkanLayoutBindings.push_back(descriptorSetLayoutBinding);
        //}
	}

	BindingLayout::BindingLayout(const VulkanContext& context, const RHIBindlessLayoutDesc& desc)
		: bindlessDesc(desc)
		, isBindless(true)
		, m_Context(context)
	{
	}

	BindingLayout::~BindingLayout()
	{
	}

	VkResult BindingLayout::bake()
	{
		return VkResult();
	}

}
