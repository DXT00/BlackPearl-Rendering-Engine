#include "pch.h"
#if GE_API_VULKAN

#include <unordered_map>
#include "VkBindingLayout.h"
#include "VkUtil.h"
#include "VkEnum.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/RHI/RHIBindingLayoutDesc.h"
#include "BlackPearl/RHI/RHIBindingSet.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
namespace BlackPearl {

	BindingLayout::BindingLayout(const VulkanContext& context, const RHIBindingLayoutDesc& desc)
		: desc(desc),
		isBindless(false),
		m_Context(context)
	{
        VkShaderStageFlagBits shaderStageFlags = VkUtil::convertShaderTypeToShaderStageFlagBits(desc.visibility);

        // iterate over all binding types and add to map
        for (const RHIBindingLayoutItem& binding : desc.bindings)
        {
            vk::DescriptorType descriptorType;
            uint32_t descriptorCount = 1;
            uint32_t registerOffset;

            switch (binding.type)
            {
            case RHIResourceType::RT_Texture_SRV:
                registerOffset = desc.bindingOffsets.shaderResource;
                descriptorType = vk::DescriptorType::eSampledImage;
                break;

            case RHIResourceType::RT_Texture_UAV:
                registerOffset = desc.bindingOffsets.unorderedAccess;
                descriptorType = vk::DescriptorType::eStorageImage;
                break;

            case RHIResourceType::RT_TypedBuffer_SRV:
                registerOffset = desc.bindingOffsets.shaderResource;
                descriptorType = vk::DescriptorType::eUniformTexelBuffer;
                break;

            case RHIResourceType::RT_StructuredBuffer_SRV:
            case RHIResourceType::RT_RawBuffer_SRV:
                registerOffset = desc.bindingOffsets.shaderResource;
                descriptorType = vk::DescriptorType::eStorageBuffer;
                break;

            case RHIResourceType::RT_TypedBuffer_UAV:
                registerOffset = desc.bindingOffsets.unorderedAccess;
                descriptorType = vk::DescriptorType::eStorageTexelBuffer;
                break;

            case RHIResourceType::RT_StructuredBuffer_UAV:
            case RHIResourceType::RT_RawBuffer_UAV:
                registerOffset = desc.bindingOffsets.unorderedAccess;
                descriptorType = vk::DescriptorType::eStorageBuffer;
                break;

            case RHIResourceType::RT_ConstantBuffer:
                registerOffset = desc.bindingOffsets.constantBuffer;
                descriptorType = vk::DescriptorType::eUniformBuffer;
                break;

            case RHIResourceType::RT_VolatileConstantBuffer:
                registerOffset = desc.bindingOffsets.constantBuffer;
                descriptorType = vk::DescriptorType::eUniformBufferDynamic;
                break;

            case RHIResourceType::RT_Sampler:
                registerOffset = desc.bindingOffsets.sampler;
                descriptorType = vk::DescriptorType::eSampler;
                break;

            case RHIResourceType::RT_PushConstants:
                // don't need any descriptors for the push constants, but the vulkanLayoutBindings array 
                // must match the binding layout items for further processing within nvrhi --
                // so set descriptorCount to 0 instead of skipping it
                registerOffset = desc.bindingOffsets.constantBuffer;
                descriptorType = vk::DescriptorType::eUniformBuffer;
                descriptorCount = 0;
                break;

            case RHIResourceType::RT_RayTracingAccelStruct:
                registerOffset = desc.bindingOffsets.shaderResource;
                descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
                break;

            case RHIResourceType::RT_None:
            case RHIResourceType::RT_Count:
            default:
                GE_INVALID_ENUM();
                continue;
            }

            const auto bindingLocation = registerOffset + binding.slot;

            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;

            descriptorSetLayoutBinding.binding = bindingLocation;
            descriptorSetLayoutBinding.descriptorCount = descriptorCount;
            descriptorSetLayoutBinding.descriptorType = (VkDescriptorType)descriptorType;
            descriptorSetLayoutBinding.stageFlags = shaderStageFlags;

              /*  .setBinding(bindingLocation)
                .setDescriptorCount(descriptorCount)
                .setDescriptorType(descriptorType)
                .setStageFlags(shaderStageFlags);*/

            vulkanLayoutBindings.push_back(descriptorSetLayoutBinding);
        }
	}
    /*    
     在 Binding 中可以指定 descriptorCount 字段来设置描述符数量，数量大于 1 则意味着这是个 AoD (Array of Descriptor),有限 Bindless     
    */
	BindingLayout::BindingLayout(const VulkanContext& context, const RHIBindlessLayoutDesc& _desc)
		: bindlessDesc(_desc)
		, isBindless(true)
		, m_Context(context)
	{
        desc.visibility = bindlessDesc.visibility;
        VkShaderStageFlagBits shaderStageFlags = VkUtil::convertShaderTypeToShaderStageFlagBits(bindlessDesc.visibility);
        uint32_t bindingPoint = 0;
        uint32_t arraySize = bindlessDesc.maxCapacity;

        // iterate over all binding types and add to map
        for (const RHIBindingLayoutItem& space : bindlessDesc.registerSpaces)
        {
            vk::DescriptorType descriptorType;

            switch (space.type)
            {
            case RHIResourceType::RT_Texture_SRV:
                descriptorType = vk::DescriptorType::eSampledImage;
                break;

            case RHIResourceType::RT_Texture_UAV:
                descriptorType = vk::DescriptorType::eStorageImage;
                break;

            case RHIResourceType::RT_TypedBuffer_SRV:
                descriptorType = vk::DescriptorType::eUniformTexelBuffer;
                break;

            case RHIResourceType::RT_TypedBuffer_UAV:
                descriptorType = vk::DescriptorType::eStorageTexelBuffer;
                break;

            case RHIResourceType::RT_StructuredBuffer_SRV:
            case RHIResourceType::RT_StructuredBuffer_UAV:
            case RHIResourceType::RT_RawBuffer_SRV:
            case RHIResourceType::RT_RawBuffer_UAV:
                descriptorType = vk::DescriptorType::eStorageBuffer;
                break;

            case RHIResourceType::RT_ConstantBuffer:
                descriptorType = vk::DescriptorType::eUniformBuffer;
                break;

            case RHIResourceType::RT_VolatileConstantBuffer:
                m_Context.error("Volatile constant buffers are not supported in bindless layouts");
                descriptorType = vk::DescriptorType::eUniformBufferDynamic;
                break;

            case RHIResourceType::RT_Sampler:
                descriptorType = vk::DescriptorType::eSampler;
                break;

            case RHIResourceType::RT_PushConstants:
                continue;

            case RHIResourceType::RT_RayTracingAccelStruct:
                descriptorType = vk::DescriptorType::eAccelerationStructureKHR;
                break;

            case RHIResourceType::RT_None:
            case RHIResourceType::RT_Count:
            default:
                GE_INVALID_ENUM();
                continue;
            }
   
            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding; 
            descriptorSetLayoutBinding.binding = bindingPoint;
            descriptorSetLayoutBinding.descriptorCount = arraySize;
            descriptorSetLayoutBinding.descriptorType = (VkDescriptorType)descriptorType;
            descriptorSetLayoutBinding.stageFlags = shaderStageFlags;

              /*  .setBinding(bindingPoint)
                .setDescriptorCount(arraySize)
                .setDescriptorType(descriptorType)
                .setStageFlags(shaderStageFlags);*/

            vulkanLayoutBindings.push_back(descriptorSetLayoutBinding);

            ++bindingPoint;
        }
	}

	BindingLayout::~BindingLayout()
	{
	}

	VkResult BindingLayout::bake()
	{
        // create the descriptor set layout object
        
        //auto descriptorSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo()
        //    .setBindingCount(uint32_t(vulkanLayoutBindings.size()))
        //    .setPBindings(vulkanLayoutBindings.data());

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo;
        descriptorSetLayoutInfo.bindingCount = vulkanLayoutBindings.size();
        descriptorSetLayoutInfo.pBindings = vulkanLayoutBindings.data();


        std::vector<VkDescriptorBindingFlags> bindFlag(vulkanLayoutBindings.size(), VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT);

        //auto extendedInfo = vk::DescriptorSetLayoutBindingFlagsCreateInfo()
        //    .setBindingCount(uint32_t(vulkanLayoutBindings.size()))
        //    .setPBindingFlags(bindFlag.data());

        VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo;
        extendedInfo.bindingCount = uint32_t(vulkanLayoutBindings.size());
        extendedInfo.pBindingFlags = bindFlag.data();

        if (isBindless)
        {
            descriptorSetLayoutInfo.pNext = &extendedInfo;
          //  descriptorSetLayoutInfo.setPNext(&extendedInfo);
        }


		//const vk::Result res = m_Context.device.createDescriptorSetLayout(&descriptorSetLayoutInfo,
		//    m_Context.allocationCallbacks,
		//    &descriptorSetLayout);
		//CHECK_VK_RETURN(res)

		if (vkCreateDescriptorSetLayout(m_Context.device, &descriptorSetLayoutInfo, m_Context.allocationCallbacks, &descriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}

            // count the number of descriptors required per type
        std::unordered_map<VkDescriptorType, uint32_t> poolSizeMap;
        for (auto layoutBinding : vulkanLayoutBindings)
        {
            if (poolSizeMap.find(layoutBinding.descriptorType) == poolSizeMap.end())
            {
                poolSizeMap[layoutBinding.descriptorType] = 0;
            }

            poolSizeMap[layoutBinding.descriptorType] += layoutBinding.descriptorCount;
        }

        // compute descriptor pool size info
        for (auto poolSizeIter : poolSizeMap)
        {
            if (poolSizeIter.second > 0)
            {
                VkDescriptorPoolSize&& poolSize{};
             /*   descriptorPoolSizeInfo.push_back(vk::DescriptorPoolSize()
                    .setType(poolSizeIter.first)
                    .setDescriptorCount(poolSizeIter.second));*/

                poolSize.type = poolSizeIter.first;
                poolSize.descriptorCount = poolSizeIter.second;
                descriptorPoolSizeInfo.push_back(std::move(poolSize));

            }
        }

        return VkResult::VK_SUCCESS;
	}

}
#endif
