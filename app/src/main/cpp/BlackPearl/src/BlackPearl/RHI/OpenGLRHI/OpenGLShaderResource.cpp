#include "pch.h"
#include "OpenGLShaderResource.h"
#include "OpenGLShader.h"
#include "OpenGLBindingSet.h"
#include "OpenGLTexture.h"
#include "OpenGLSampler.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"

namespace BlackPearl {
	FOpenGLShaderParameterCache::FOpenGLShaderParameterCache()
	{
	}
	FOpenGLShaderParameterCache::~FOpenGLShaderParameterCache()
	{
	}
	void FOpenGLShaderParameterCache::InitializeResources(int32_t UniformArraySize)
	{
	}
	void FOpenGLShaderParameterCache::MarkAllDirty()
	{
	}
	void FOpenGLShaderParameterCache::Set(uint32_t BufferIndex, uint32_t ByteOffset, uint32_t NumBytes, const void* NewValues)
	{
	}
	void FOpenGLShaderParameterCache::CommitPackedGlobals(const FOpenGLLinkedProgram* LinkedProgram, int32_t Stage)
	{
	}

	void FOpenGLShaderParameterCache::CommitPackedUniformBuffers(FOpenGLLinkedProgram* LinkedProgram, int32_t Stage, const std::vector<BindingSetHandle>& bindings)
	{
		for (size_t i = 0; i < bindings.size(); i++)
		{
			BindingSet* bindingSet = static_cast<BindingSet*>(bindings[i].Get());
			for (BindingSetItem& binding : bindingSet->desc.bindings)
			{

				switch (binding.type)
				{
				case RHIResourceType::RT_Texture_SRV:
				{
					 Texture* texture = static_cast<Texture*>(binding.resourceHandle);

					//const auto subresource = binding.subresources.resolve(texture->getDesc(), false);
					//const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
					//auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

					//VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();


					////VkDescriptorImageInfo imageInfo;
					//imageInfo.imageView = view.view;
					//imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					/*		imageInfo = VkDescriptorImageInfo()
								.setImageView(view.view)
								.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal);*/

					/*generateWriteDescriptorData(layoutBinding.binding,
						layoutBinding.descriptorType,
						&imageInfo, nullptr, nullptr);*/
				}

				break;

				case RHIResourceType::RT_Texture_UAV:
				{
					Texture* texture = static_cast<Texture*>(binding.resourceHandle);

					const auto subresource = binding.subresources.resolve(texture->getDesc(), true);
				/*	const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
					auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);*/

					//VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();
					////imageInfo = vk::DescriptorImageInfo()
					////	.setImageView(view.view)
					////	.setImageLayout(vk::ImageLayout::eGeneral);


					////VkDescriptorImageInfo imageInfo;
					//imageInfo.imageView = view.view;
					//imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

					//generateWriteDescriptorData(layoutBinding.binding,
					//	layoutBinding.descriptorType,
					//	&imageInfo, nullptr, nullptr);
				}

				break;

				case RHIResourceType::RT_TypedBuffer_SRV:
				case RHIResourceType::RT_TypedBuffer_UAV:
				{
					Buffer* buffer = static_cast<Buffer*>(binding.resourceHandle);

					//auto vkformat = VkUtil::convertFormat(binding.format);

					//const auto range = binding.range.resolve(buffer->desc);
					//uint64_t viewInfoHash = 0;
					//hash_combine(viewInfoHash, range.byteOffset);
					//hash_combine(viewInfoHash, range.byteSize);
					//hash_combine(viewInfoHash, (uint64_t)vkformat);

					//const auto& bufferViewFound = buffer->viewCache.find(viewInfoHash);
					//auto& bufferViewRef = (bufferViewFound != buffer->viewCache.end()) ? bufferViewFound->second : buffer->viewCache[viewInfoHash];
					//if (bufferViewFound == buffer->viewCache.end())
					//{
					//	assert(binding.format != Format::UNKNOWN);

					//	VkBufferViewCreateInfo bufferViewInfo{};
					//	bufferViewInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
					//	bufferViewInfo.buffer = buffer->buffer;
					//	bufferViewInfo.offset = range.byteOffset;
					//	bufferViewInfo.range = range.byteSize;
					//	bufferViewInfo.format = vkformat;

					//	//vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
					//	if (vkCreateBufferView(m_Context.device, &bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef) != VK_SUCCESS) {
					//		throw std::runtime_error("failed to create buffer view!");
					//	}
					//	/*	 VkBufferViewCreateInfo bufferViewInfo{}
					//			.setBuffer(buffer->buffer)
					//			.setOffset(range.byteOffset)
					//			.setRange(range.byteSize)
					//			.setFormat(vk::Format(vkformat));

					//		res = m_Context.device.createBufferView(&bufferViewInfo, m_Context.allocationCallbacks, &bufferViewRef);
					//		ASSERT_VK_OK(res);*/
					//}

					//generateWriteDescriptorData(layoutBinding.binding,
					//	layoutBinding.descriptorType,
					//	nullptr, nullptr, &bufferViewRef);
				}
				break;

				case RHIResourceType::RT_StructuredBuffer_SRV:
				case RHIResourceType::RT_StructuredBuffer_UAV:
				case RHIResourceType::RT_RawBuffer_SRV:
				case RHIResourceType::RT_RawBuffer_UAV:
				case RHIResourceType::RT_ConstantBuffer:
				case RHIResourceType::RT_VolatileConstantBuffer:
				{
					Buffer* buffer = static_cast<Buffer*>(binding.resourceHandle);

					//const auto range = binding.range.resolve(buffer->desc);

					//auto& bufferInfo = descriptorBufferInfo.emplace_back();
					////VkDescriptorBufferInfo bufferInfo{};

					//bufferInfo.buffer = buffer->buffer;
					//bufferInfo.offset = range.byteOffset;
					//bufferInfo.range = range.byteSize;

					//assert(buffer->buffer);
					//generateWriteDescriptorData(layoutBinding.binding,
					//	layoutBinding.descriptorType,
					//	nullptr, &bufferInfo, nullptr);
				}

				break;

				case RHIResourceType::RT_Sampler:
				{
					const auto& sampler = static_cast<Sampler*>(binding.resourceHandle);

					//VkDescriptorImageInfo& imageInfo = descriptorImageInfo.emplace_back();
					//imageInfo.sampler = sampler->sampler;
					///*	imageInfo = vk::DescriptorImageInfo()
					//		.setSampler(sampler->sampler);*/

					//generateWriteDescriptorData(layoutBinding.binding,
					//	layoutBinding.descriptorType,
					//	&imageInfo, nullptr, nullptr);
				}

				break;

				case RHIResourceType::RT_RayTracingAccelStruct:
					RHIUtils::NotImplemented();
					break;

				case RHIResourceType::RT_PushConstants:
					RHIUtils::NotSupported();
					break;

				case RHIResourceType::RT_None:
				case RHIResourceType::RT_Count:
				default:
					RHIUtils::InvalidEnum();
				}
			}
			}
		

	}

	void FOpenGLShaderParameterCache::FRange::MarkDirtyRange(uint32_t NewStartVector, uint32_t NewNumVectors)
	{
	}
}

