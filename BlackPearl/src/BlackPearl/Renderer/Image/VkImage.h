#pragma once
#ifdef GE_API_VULKAN
#include <vulkan/vulkan.h>
namespace BlackPearl {

	void createImage(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t width, 
		uint32_t height, 
		VkFormat format, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& imageMemory);

	VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

	void transitionImageLayout(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
}

#endif