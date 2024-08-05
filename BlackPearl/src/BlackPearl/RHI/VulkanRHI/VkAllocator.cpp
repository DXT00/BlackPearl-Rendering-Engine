#include "pch.h"
#if GE_API_VULKAN
#include "VkAllocator.h"
#include "VkBuffer.h"
#include "VkTexture.h"
#include "VkMemoryResource.h"

namespace BlackPearl {

	static VkMemoryPropertyFlags pickBufferMemoryProperties(const BufferDesc& d)
	{
		VkMemoryPropertyFlags flags{};

		switch (d.cpuAccess)
		{
		case CpuAccessMode::None:
			flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			break;
		case CpuAccessMode::Read:
			flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
			break;
		case CpuAccessMode::Write:
			flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			break;
		}

		return flags;
	}

	VkResult VulkanAllocator::allocateBufferMemory(Buffer* buffer, bool enableBufferAddress) 
	{
		VkMemoryRequirements memRequirements;

		vkGetBufferMemoryRequirements(m_Context.device, buffer->buffer, &memRequirements);
		const VkResult res = allocateMemory(buffer, memRequirements, pickBufferMemoryProperties(buffer->desc), enableBufferAddress);
		assert(res == VkResult::VK_SUCCESS);

		vkBindBufferMemory(m_Context.device, buffer->buffer, buffer->memory, 0);
		return VkResult::VK_SUCCESS;
	}

	void VulkanAllocator::freeBufferMemory(Buffer* buffer) 
	{
		freeMemory(buffer);
	}

	VkResult VulkanAllocator::allocateTextureMemory(ETexture* texture) 
	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Context.device, texture->image, &memRequirements);

		// allocate memory
		const VkMemoryPropertyFlags memProperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		const VkResult res = allocateMemory(texture, memRequirements, memProperties);
		assert(res == VkResult::VK_SUCCESS);

		vkBindImageMemory(m_Context.device, texture->image, texture->memory, 0);

		return VkResult::VK_SUCCESS;
	}

	void VulkanAllocator::freeTextureMemory(ETexture* texture) 
	{
		freeMemory(texture);
	}

	VkResult VulkanAllocator::allocateMemory(MemoryResource* res, VkMemoryRequirements memRequirements, VkMemoryPropertyFlags memPropertyFlags, bool enableDeviceAddress, bool enableExportMemory, VkImage dedicatedImage, VkBuffer dedicatedBuffer) 
	{
		res->managed = true;
		// find a memory space that satisfies the requirements
		VkPhysicalDeviceMemoryProperties memProperties;
		
		vkGetPhysicalDeviceMemoryProperties(m_Context.physicalDevice, &memProperties);
		uint32_t memTypeIndex;
		for (memTypeIndex = 0; memTypeIndex < memProperties.memoryTypeCount; memTypeIndex++)
		{
			if ((memRequirements.memoryTypeBits & (1 << memTypeIndex)) &&
				((memProperties.memoryTypes[memTypeIndex].propertyFlags & memPropertyFlags) == memPropertyFlags))
			{
				break;
			}
		}

		if (memTypeIndex == memProperties.memoryTypeCount)
		{
			// xxxnsubtil: this is incorrect; need better error reporting
			return VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY;
		}

		// allocate memory
		VkMemoryAllocateFlagsInfo allocFlags{};
		allocFlags.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
		if (enableDeviceAddress)
			allocFlags.flags |= VkMemoryAllocateFlagBits::VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT;
		const void* pNext = &allocFlags;

		VkMemoryDedicatedAllocateInfo dedicatedAllocation{};
		dedicatedAllocation.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO;
		dedicatedAllocation.image = dedicatedImage;
		dedicatedAllocation.buffer = dedicatedBuffer;
		dedicatedAllocation.pNext = pNext;
		/*	.setImage(dedicatedImage)
			.setBuffer(dedicatedBuffer)
			.setPNext(pNext);*/

		if (dedicatedImage || dedicatedBuffer)
		{
			// Append the VkMemoryDedicatedAllocateInfo structure to the chain
			pNext = &dedicatedAllocation;
		}

#ifdef _WIN32
		const auto handleType = VkExternalMemoryHandleTypeFlagBits::VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#else
		const auto handleType = VkExternalMemoryHandleTypeFlagBits::VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_FD_BIT;
#endif
		VkExportMemoryAllocateInfo exportInfo{};
		exportInfo.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO;
		exportInfo.handleTypes = handleType;
		exportInfo.pNext = pNext;
		/*	.setHandleTypes(handleType)
			.setPNext(pNext);*/

		if (enableExportMemory)
		{
			// Append the VkExportMemoryAllocateInfo structure to the chain
			pNext = &exportInfo;
		}

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = memTypeIndex;
		allocInfo.pNext = pNext;

		VkResult ret;
		ret = vkAllocateMemory(m_Context.device, &allocInfo, nullptr, &res->memory);


		return ret;
	}

	void VulkanAllocator::freeMemory(MemoryResource* res) 
	{
		assert(res->managed);
		vkFreeMemory(m_Context.device, res->memory, m_Context.allocationCallbacks);
		res->memory = nullptr;
	}



}
#endif