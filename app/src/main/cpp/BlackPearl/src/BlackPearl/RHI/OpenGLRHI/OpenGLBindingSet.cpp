#include "pch.h"
#include "OpenGLBindingSet.h"
#include "OpenGLDevice.h"
#include "OpenGLTexture.h"
namespace BlackPearl {
	BindingSet::~BindingSet()
	{
	}
	BindingSetHandle Device::createBindingSet(const BindingSetDesc& desc, IBindingLayout* _layout)
	{
		BindingLayout* layout = dynamic_cast<BindingLayout*>(_layout);

		BindingSet* ret = new BindingSet(m_Context);
		ret->desc = desc;
		ret->layout = layout;


		// collect all of the descriptor write data
		/*std::vector<VkDescriptorImageInfo> descriptorImageInfo;
		std::vector<VkDescriptorBufferInfo> descriptorBufferInfo;
		VkWriteDescriptorSet descriptorWriteInfo[200];
		VkDescriptorBufferInfo bufferinfos[100];
		VkDescriptorImageInfo imageInfos[100];
		VkBufferViewCreateInfo  bufferViewinfos[100];*/
		int descriptorCnt = 0;
		int bufferDescCnt = 0;
		int bufferViewDescCnt = 0;
		int ImageDescCnt = 0;

		//std::vector<VkWriteDescriptorSetAccelerationStructureKHR> accelStructWriteInfo;

		for (size_t bindingIndex = 0; bindingIndex < desc.bindings.size(); bindingIndex++)
		{
			const BindingSetItem& binding = desc.bindings[bindingIndex];
			const GLBindingItem& layoutBinding = layout->glLayoutBindings[bindingIndex];

			if (binding.resourceHandle == nullptr)
			{
				continue;
			}

			ret->resources.push_back(binding.resourceHandle); // keep a strong reference to the resource

			switch (binding.type)
			{
			case RHIResourceType::RT_Texture_SRV:
			{
				Texture* texture = dynamic_cast<Texture*>(binding.resourceHandle);

				const TextureSubresourceSet subresource = binding.subresources.resolve(texture->getDesc(), false);
				const Texture::TextureSubresourceViewType textureViewType = getTextureViewType(binding.format, texture->desc.format);
				auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);

				imageInfos[ImageDescCnt].imageView = view.view;
				imageInfos[ImageDescCnt].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr, nullptr, descriptorWriteInfo, ret, bindingIndex);

				ImageDescCnt++;
				descriptorCnt++;

				if (!static_cast<bool>(texture->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(texture->permanentState,
						ResourceStates::ShaderResource,
						true, texture->desc.debugName, m_Context.messageCallback);
			}

			break;

			case RHIResourceType::RT_Texture_UAV:
			{
				const auto texture = dynamic_cast<Texture*>(binding.resourceHandle);

				const auto subresource = binding.subresources.resolve(texture->desc, true);
				const auto textureViewType = getTextureViewType(binding.format, texture->desc.format);
				auto& view = texture->getSubresourceView(subresource, binding.dimension, binding.format, textureViewType);


				imageInfos[ImageDescCnt].imageView = view.view;
				imageInfos[ImageDescCnt].imageLayout = VK_IMAGE_LAYOUT_GENERAL;


				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr, nullptr, descriptorWriteInfo, ret, bindingIndex);
				ImageDescCnt++;
				descriptorCnt++;

				if (!static_cast<bool>(texture->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(texture->permanentState,
						ResourceStates::UnorderedAccess,
						true, texture->desc.debugName, m_Context.messageCallback);
			}
			break;
			case RHIResourceType::RT_TypedBuffer_SRV:
			case RHIResourceType::RT_TypedBuffer_UAV:
			{
				const auto buffer = dynamic_cast<Buffer*>(binding.resourceHandle);

				assert(buffer->desc.canHaveTypedViews);
				const bool isUAV = (binding.type == RHIResourceType::RT_TypedBuffer_UAV);
				if (isUAV)
					assert(buffer->desc.canHaveUAVs);

				Format format = binding.format;

				if (format == Format::UNKNOWN)
				{
					format = buffer->desc.format;
				}

				auto vkformat = VkUtil::convertFormat(format);
				const auto range = binding.range.resolve(buffer->desc);

				uint64_t viewInfoHash = 0;
				hash_combine(viewInfoHash, range.byteOffset);
				hash_combine(viewInfoHash, range.byteSize);
				hash_combine(viewInfoHash, (uint64_t)vkformat);

				const auto& bufferViewFound = buffer->viewCache.find(viewInfoHash);
				auto& bufferViewRef = (bufferViewFound != buffer->viewCache.end()) ? bufferViewFound->second : buffer->viewCache[viewInfoHash];
				if (bufferViewFound == buffer->viewCache.end())
				{
					assert(format != Format::UNKNOWN);

					bufferViewinfos[bufferViewDescCnt].sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
					bufferViewinfos[bufferViewDescCnt].buffer = buffer->buffer;
					bufferViewinfos[bufferViewDescCnt].offset = range.byteOffset;
					bufferViewinfos[bufferViewDescCnt].range = range.byteSize;
					bufferViewinfos[bufferViewDescCnt].format = vkformat;

					if (vkCreateBufferView(m_Context.device, &bufferViewinfos[bufferViewDescCnt], m_Context.allocationCallbacks, &bufferViewRef) != VK_SUCCESS) {
						throw std::runtime_error("failed to create buffer view!");
					}
				}

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, nullptr, &bufferViewRef, descriptorWriteInfo, ret, bindingIndex);

				bufferViewDescCnt++;
				descriptorCnt++;


				if (!static_cast<bool>(buffer->permanentState))
					ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
				else
					verifyPermanentResourceState(buffer->permanentState,
						isUAV ? ResourceStates::UnorderedAccess : ResourceStates::ShaderResource,
						false, buffer->desc.debugName, m_Context.messageCallback);
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

				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveUAVs);
				if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_StructuredBuffer_SRV)
					assert(buffer->desc.structStride != 0);
				if (binding.type == RHIResourceType::RT_RawBuffer_SRV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
					assert(buffer->desc.canHaveRawViews);

				const auto range = binding.range.resolve(buffer->desc);

				bufferinfos[bufferDescCnt].buffer = buffer->buffer;
				bufferinfos[bufferDescCnt].offset = range.byteOffset;
				bufferinfos[bufferDescCnt].range = range.byteSize;

				assert(buffer->buffer);

				descriptorWriteInfo[bindingIndex].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWriteInfo[bindingIndex].descriptorType = layoutBinding.descriptorType;
				descriptorWriteInfo[bindingIndex].dstSet = ret->descriptorSet;
				descriptorWriteInfo[bindingIndex].dstBinding = layoutBinding.binding;
				descriptorWriteInfo[bindingIndex].dstArrayElement = 0;
				descriptorWriteInfo[bindingIndex].descriptorCount = 1;
				descriptorWriteInfo[bindingIndex].pImageInfo = nullptr;
				descriptorWriteInfo[bindingIndex].pBufferInfo = &bufferinfos[bufferDescCnt];
				descriptorWriteInfo[bindingIndex].pTexelBufferView = nullptr;
				descriptorWriteInfo[bindingIndex].pNext = nullptr;

				bufferDescCnt++;
				descriptorCnt++;


				if (binding.type == RHIResourceType::RT_VolatileConstantBuffer)
				{
					assert(buffer->desc.isVolatile);
					ret->volatileConstantBuffers.push_back(buffer);
				}
				else
				{
					if (!static_cast<bool>(buffer->permanentState))
						ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));
					else
					{
						ResourceStates requiredState;
						if (binding.type == RHIResourceType::RT_StructuredBuffer_UAV || binding.type == RHIResourceType::RT_RawBuffer_UAV)
							requiredState = ResourceStates::UnorderedAccess;
						else if (binding.type == RHIResourceType::RT_ConstantBuffer)
							requiredState = ResourceStates::ConstantBuffer;
						else
							requiredState = ResourceStates::ShaderResource;

						verifyPermanentResourceState(buffer->permanentState, requiredState,
							false, buffer->desc.debugName, m_Context.messageCallback);
					}
				}
			}

			break;

			case RHIResourceType::RT_Sampler:
			{
				const auto sampler = dynamic_cast<Sampler*>(binding.resourceHandle);

	
				//imageInfos[ImageDescCnt].sampler = sampler->sampler;




			/*	generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					&imageInfos[ImageDescCnt], nullptr, nullptr, descriptorWriteInfo, ret, bindingIndex);*/

				ImageDescCnt++;
				descriptorCnt++;

			}

			break;

			case RHIResourceType::RT_RayTracingAccelStruct:
			{
				/*const auto as = dynamic_cast<AccelStruct*>(binding.resourceHandle);

				auto& accelStructWrite = accelStructWriteInfo.emplace_back();
				accelStructWrite.accelerationStructureCount = 1;
				accelStructWrite.pAccelerationStructures = &as->accelStruct;

				generateWriteDescriptorData(layoutBinding.binding,
					layoutBinding.descriptorType,
					nullptr, nullptr, nullptr, &accelStructWrite);

				ret->bindingsThatNeedTransitions.push_back(static_cast<uint16_t>(bindingIndex));*/
			}

			break;

			case RHIResourceType::RT_PushConstants:
				break;

			case RHIResourceType::RT_None:
			case RHIResourceType::RT_Count:
			default:
				assert(!"Invalid Enumeration Value");
				break;
			}
		}




		return BindingSetHandle::Create(ret);
	}
}