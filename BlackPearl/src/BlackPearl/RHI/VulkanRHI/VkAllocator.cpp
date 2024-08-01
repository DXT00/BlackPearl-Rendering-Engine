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
		// figure out memory requirements
		VkMemoryRequirements memRequirements;
		/*m_Context.device.getBufferMemoryRequirements(buffer->buffer, &memRequirements);
		VkMemoryRequirements vulkanMemReq;*/
		vkGetBufferMemoryRequirements(m_Context.device, buffer->buffer, &memRequirements);
		// allocate memory
		const VkResult res = allocateMemory(buffer, memRequirements, pickBufferMemoryProperties(buffer->desc), enableBufferAddress);
		assert(res == VkResult::VK_SUCCESS);

			//m_Context.device.bindBufferMemory(buffer->buffer, buffer->memory, 0);
		vkBindBufferMemory(m_Context.device, buffer->buffer, buffer->memory, 0);
		return VkResult::VK_SUCCESS;
	}

	void VulkanAllocator::freeBufferMemory(Buffer* buffer) 
	{
		freeMemory(buffer);
	}

	VkResult VulkanAllocator::allocateTextureMemory(ETexture* texture) 
	{
		return VK_SUCCESS;
	}

	void VulkanAllocator::freeTextureMemory(const ETexture* texture) 
	{
	}

	VkResult VulkanAllocator::allocateMemory(MemoryResource* res, VkMemoryRequirements memRequirements, VkMemoryPropertyFlags memPropertyFlags, bool enableDeviceAddress, bool enableExportMemory, VkImage dedicatedImage, VkBuffer dedicatedBuffer) 
	{
		return VK_SUCCESS;
	}

	void VulkanAllocator::freeMemory(MemoryResource* res) 
	{
	}



}
#endif