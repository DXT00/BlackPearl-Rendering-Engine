#pragma once
#include "BasicRenderer.h"
//#define VK_USE_PLATFORM_WIN32_KHR
#ifdef GE_API_VULKAN
//#define GLFW_INCLUDE_NONE
//#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

//#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32


namespace BlackPearl {

	class VkComputeShaderRender 
	{

	public:
		VkComputeShaderRender();
		~VkComputeShaderRender();

		void Init();
		void Render();
		void UpdateUniformBuffer(uint32_t currentImage);
	public:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();


		void CreateRenderPass();
		void CreateComputeDescriptorSetLayout();
		void CreateGraphicsPipeline();
		//compute shader pipeline
		void CreateComputePipeline();

		void CreateShaderStorageBuffers();

		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();

		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateComputeDescriptorSets();
		void CreateCommandBuffers();
		void CreateComputeCommandBuffers();
		void CreateSyncObjects();
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		void RecreateSwapChain();

		void CleanUp();
		void CleanUpSwapChain();
	public:
		struct UniformBufferObject {
			float deltaTime = 1.0f;
		};


		struct Particle {
			glm::vec2 position;
			glm::vec2 velocity;
			glm::vec4 color;

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Particle);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Particle, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Particle, color);

				return attributeDescriptions;
			}
		};

		/*const std::vector<Vertex> vertices = {
			 {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};*/
		
	private:
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsAndComputeFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsAndComputeFamily.has_value() && presentFamily.has_value();
			}
		};
	private:
		SwapChainSupportDetails _QuerySwapChainSupport(VkPhysicalDevice device);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& present);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecordComputeCommandBuffer(VkCommandBuffer commandBuffer);

		uint32_t _FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;

		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;
		VkQueue m_PresentQueue;

		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		VkRenderPass m_RenderPass;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;

		//compute shader
		VkPipelineLayout m_ComputePipelineLayout;
		VkDescriptorSetLayout m_ComputeDescriptorSetLayout;

		std::vector<VkBuffer> m_ShaderStorageBuffers;
		std::vector<VkDeviceMemory> m_ShaderStorageBuffersMemory;

		std::vector<VkImageView> m_ImageViews;
		std::vector<VkFramebuffer> m_FrameBuffers;

		VkPipeline m_GraphicsPipeline;
		VkPipeline m_ComputePipeline;
		//commandBuffer
		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkCommandBuffer> m_ComputeCommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishSemaphores;
		std::vector<VkSemaphore> m_ComputeFinishSemaphores;

		std::vector<VkFence> m_InFlightFences;
		std::vector<VkFence> m_ComputeInFlightFences;

		VkDebugUtilsMessengerEXT m_DebugMessenger;

		int m_CurrentFrame = 0;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;

		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;

		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;
		std::vector<VkDescriptorSet> m_ComputeDescriptorSets;


		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;
		VkSampler m_TextureSampler;
		VkImageView m_TextureImageView;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;

		// time
		double m_LastTime = 0.0f;
		double m_LastFrameTime;



		bool _checkDeviceExtensionSupport(VkPhysicalDevice device);
		bool _isDeviceSuitable(VkPhysicalDevice device);
		void _populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		static VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}
	};

}

#endif