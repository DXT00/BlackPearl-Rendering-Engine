#include <vulkan/vulkan.h>
namespace BlackPearl {
	VkCommandBuffer beginCommandBuffer(VkDevice device, VkCommandPool commandPool);
	void endCommandBuffer(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer, VkQueue graphicsQueue);
}