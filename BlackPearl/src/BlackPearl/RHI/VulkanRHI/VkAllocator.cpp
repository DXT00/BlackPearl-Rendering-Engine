#include "pch.h"
#if GE_API_VULKAN
#include "VkAllocator.h"
#include "VkBuffer.h"
#include "VkTexture.h"
#include "VkMemoryResource.h"

namespace BlackPearl {
	VkResult VulkanAllocator::allocateBufferMemory(Buffer* buffer, bool enableBufferAddress) 
	{
		return VK_SUCCESS;
	}

	void VulkanAllocator::freeBufferMemory(Buffer* buffer) 
	{
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