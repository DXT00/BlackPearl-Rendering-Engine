#pragma once
#ifdef GE_API_VULKAN
#include <vulkan/vulkan.h>
namespace BlackPearl {

	class ImageUtils {
	public:
		static void createImage(
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

		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format);

		static void transitionImageLayout(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	
		static VkImageMemoryBarrier readOnlyToGeneralBarrier(const VkImage& image);

		static VkImageMemoryBarrier generalToTransferDstBarrier(const VkImage& image);
		
		static VkImageMemoryBarrier generalToTransferSrcBarrier(const VkImage& image);

		static VkImageMemoryBarrier transferDstToGeneralBarrier(const VkImage& image);

		static VkImageMemoryBarrier transferSrcToReadOnlyBarrier(const VkImage& image);

		static VkImageCopy imageCopyRegion(uint32_t width, uint32_t height);


	};
	
}

#endif