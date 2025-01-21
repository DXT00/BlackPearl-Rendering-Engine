#include "pch.h"
#include "OpenGLBindingLayout.h"
namespace BlackPearl {
	BindingLayout::BindingLayout(const OpenGLContext& context, const RHIBindingLayoutDesc& desc)
		: desc(desc),
		isBindless(false),
		m_Context(context)
	{
		for (const RHIBindingLayoutItem& binding : desc.bindings)
		{
			//vk::DescriptorType descriptorType;
			uint32_t descriptorCount = 1;
			uint32_t registerOffset;
			ShaderType shaderType;

			switch (binding.type)
			{
				{
			case RHIResourceType::RT_Texture_SRV:
				registerOffset = desc.bindingOffsets.shaderResource;
				//   descriptorType = vk::DescriptorType::eSampledImage;
				break;

			case RHIResourceType::RT_Texture_UAV:
				registerOffset = desc.bindingOffsets.unorderedAccess;
				// descriptorType = vk::DescriptorType::eStorageImage;
				break;

			case RHIResourceType::RT_TypedBuffer_SRV:
				registerOffset = desc.bindingOffsets.shaderResource;
				//  descriptorType = vk::DescriptorType::eUniformTexelBuffer;
				break;

			case RHIResourceType::RT_StructuredBuffer_SRV:
			case RHIResourceType::RT_RawBuffer_SRV:
				registerOffset = desc.bindingOffsets.shaderResource;
				//  descriptorType = vk::DescriptorType::eStorageBuffer;
				break;

			case RHIResourceType::RT_TypedBuffer_UAV:
				registerOffset = desc.bindingOffsets.unorderedAccess;
				// descriptorType = vk::DescriptorType::eStorageTexelBuffer;
				break;

			case RHIResourceType::RT_StructuredBuffer_UAV:
			case RHIResourceType::RT_RawBuffer_UAV:
				registerOffset = desc.bindingOffsets.unorderedAccess;
				// descriptorType = vk::DescriptorType::eStorageBuffer;
				break;

			case RHIResourceType::RT_ConstantBuffer:
				registerOffset = desc.bindingOffsets.constantBuffer;
				// descriptorType = vk::DescriptorType::eUniformBuffer;
				break;

			case RHIResourceType::RT_VolatileConstantBuffer:
				registerOffset = desc.bindingOffsets.constantBuffer;
				//  descriptorType = vk::DescriptorType::eUniformBufferDynamic;
				break;

			case RHIResourceType::RT_Sampler:
				registerOffset = desc.bindingOffsets.sampler;
				// descriptorType = vk::DescriptorType::eSampler;
				break;

			case RHIResourceType::RT_PushConstants:
				// don't need any descriptors for the push constants, but the vulkanLayoutBindings array 
				// must match the binding layout items for further processing within nvrhi --
				// so set descriptorCount to 0 instead of skipping it
				registerOffset = desc.bindingOffsets.constantBuffer;
				break;

			case RHIResourceType::RT_RayTracingAccelStruct:
				registerOffset = desc.bindingOffsets.shaderResource;
				break;

			case RHIResourceType::RT_None:
			case RHIResourceType::RT_Count:
			default:
				GE_INVALID_ENUM();
				continue;
				}
			}
		}
	}

	BindingLayout::BindingLayout(const OpenGLContext& context, const RHIBindlessLayoutDesc& desc)
		: bindlessDesc(desc)
		, isBindless(true)
		, m_Context(context)
	{
	}
}