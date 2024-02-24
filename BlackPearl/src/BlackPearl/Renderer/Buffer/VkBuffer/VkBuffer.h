#include <vulkan/vulkan.h>
namespace BlackPearl {
	uint32_t findMemoryType(
		VkPhysicalDevice physicalDevice,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties);

	void createBuffer(
		VkPhysicalDevice physicalDevice, 
		VkDevice device,
		VkDeviceSize size, 
		VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkBuffer& buffer, 
		VkDeviceMemory& bufferMemory);

	void copyBuffer(VkQueue graphicsQueue, VkDevice device, VkCommandPool commandPool, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
}