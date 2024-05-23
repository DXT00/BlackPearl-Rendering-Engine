#pragma once
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

	template<typename T>
	void inline createSSBObuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkQueue queue,
		VkCommandPool commandPool,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory,
		const T* elements,
		const size_t numElements) {

		VkDeviceSize bufferSize = sizeof(T) * numElements;
		// Create a staging buffer used to upload data to the gpu
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, elements, (size_t)bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		//m_ShaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		//m_ShaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

		// Copy initial particle data to all storage buffers
		//for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer, bufferMemory);
		copyBuffer(queue, device, commandPool, stagingBuffer, buffer, bufferSize);
		//}

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

}