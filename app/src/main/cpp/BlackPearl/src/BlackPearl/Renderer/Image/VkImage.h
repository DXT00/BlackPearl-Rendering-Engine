#pragma once
//#ifdef GE_API_VULKAN
#include <vulkan/vulkan_core.h>
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

		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask);

		static void transitionImageLayout(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	
		static VkImageMemoryBarrier readOnlyToGeneralBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier generalToTransferDstBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier generalToTransferSrcBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier transferDstToGeneralBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier transferDstToReadOnlyBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier transferSrcToReadOnlyBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier generalToReadOnlyBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);

		static VkImageMemoryBarrier colorAttachmentToTransferSrcBarrier(VkCommandBuffer commandBuffer, const VkImage& image, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask);


		static VkImageCopy imageCopyRegion(uint32_t width, uint32_t height);


	};
	
}

//#endif