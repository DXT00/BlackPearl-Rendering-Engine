#pragma once
#include "BasicRenderer.h"
#include "BlackPearl/Scene/RayTraceScene.h"
//#define VK_USE_PLATFORM_WIN32_KHR
#ifdef GE_API_VULKAN
//#define GLFW_INCLUDE_NONE
//#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

//#include <GLFW/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
#include "glm/glm.hpp"

namespace BlackPearl {

	class VkRayTracingRenderer
	{
	
	public:
		VkRayTracingRenderer();
		~VkRayTracingRenderer();

		void Init(RayTraceScene* scene);
		void Render(Camera* camera);
		void UpdateUniformBuffer(uint32_t currentImage, Camera* camera);
	public:
		void PrepareResources();



		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSwapChain();
		void CreateImageViews();


		void CreateRenderPass();

		//graphic shader pipeline -->postprocess
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateGraphicsPipeline();
		void CreateDescriptorPool();
		void CreateDescriptorSetLayout();
		void CreateDescriptorSets();
		void CreateTextureSampler();

		//compute shader pipeline
		void CreateComputePipeline();
		void CreateComputeDescriptorSets();
		void CreateShaderStorageBuffers();
		void CreateComputeDescriptorSetLayout();
		void CreateComputeDescriptorPool();
		void CreateComputeTextureImage();
		void CreateComputeTextureImageView();

		void CreateFrameBuffers();
		void CreateCommandPool();



		void CreateUniformBuffers();
		void CreateCommandBuffers();
		void CreateComputeCommandBuffers();
		void CreateSyncObjects();
		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		void RecreateSwapChain();

		void CleanUp();
		void CleanUpSwapChain();
	public:
		struct UniformBufferObject {
			alignas(16) glm::vec3 camPos;
			alignas(4) uint32_t currentSample;
			alignas(4) uint32_t numTriangles;
			alignas(4) uint32_t numLights;
			alignas(4) uint32_t numSpheres;
			alignas(8) glm::vec2 screenSize;
			alignas(16) glm::mat4 InvertProjectionView;
		};

		struct QuadVertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 color;


		};
		struct Vertex {
			glm::vec2 pos;
			glm::vec3 color;
			glm::vec2 texCoord;

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

				return attributeDescriptions;
			}
		};

		/*const std::vector<Vertex> vertices = {
			 {{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};*/
		//const std::vector<Vertex> vertices = {
		//	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		//	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		//	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f}},
		//	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
		//};
		//const std::vector<uint16_t> indices = {
		//	0, 1, 2, 2, 3, 0
		//};

		struct Quad {
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 color;

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Quad);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

				return bindingDescription;
			}

			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Quad, position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Quad, normal);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Quad, color);

				return attributeDescriptions;
			}
		};
		glm::vec3 m_LastCamPos;
		uint32_t m_CurSample = 0;
		//const std::vector<QuadVertex> vertices = {
		//	// positions			//normal				//uv
		//	{{-1.0f,  1.0f, 0.0f},  {0.0f, 0.0f, 1.0f},	  {0.0f, 1.0f}},
		//    {{-1.0f, -1.0f, 0.0f},  {0.0f, 0.0f, 1.0f},	  {0.0f, 0.0f}},
		//    {{1.0f, -1.0f, 0.0f},   {0.0f, 0.0f, 1.0f},	  {1.0f, 0.0f}},

		//    {{-1.0f,  1.0f, 0.0f},  {0.0f, 0.0f, 1.0f},	  {0.0f, 1.0f}},
		//	{{1.0f, -1.0f, 0.0f},  {0.0f, 0.0f, 1.0f},	  {1.0f, 0.0f}},
		//	{{1.0f,  1.0f, 0.0f},  {0.0f, 0.0f, 1.0f},	  {1.0f, 1.0f}}
		//};
		const std::vector<QuadVertex> vertices = {
			{{-1.0f, -1.0f,  0.0f},{0.0f, 0.0f, 1.0f},	 {1.0f, 0.0f}},
			{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},	{0.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f},	{0.0f, 1.0f}},
			{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}
		};

	/*	const std::vector<QuadVertex> vertices = {
		{{-0.5f, -0.5f,  0.0f},{0.0f, 0.0f, 1.0f},	 {1.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f},	{0.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f},	{0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}
		};*/
		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};
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
		void RecordComputeCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex);

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

		// postProces -- grapghics pipline
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkFramebuffer> m_FrameBuffers;
		VkRenderPass m_RenderPass;

		VkSampler m_TextureSampler;
		VkPipeline m_GraphicsPipeline;
		VkBuffer m_VertexBuffer;
		VkBuffer m_IndexBuffer;

		VkDeviceMemory m_VertexBufferMemory;
		VkDeviceMemory m_IndexBufferMemory;

		VkDescriptorPool m_DescriptorPool;
		VkPipelineLayout m_PipelineLayout;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		// raytracing -- compute pipline
		//compute shader
		VkDescriptorPool m_ComputeDescriptorPool;
		VkPipelineLayout m_ComputePipelineLayout;
		VkDescriptorSetLayout m_ComputeDescriptorSetLayout;
		std::vector<VkDescriptorSet> m_ComputeDescriptorSets;

		std::vector<VkBuffer> m_SSBOTriangles;
		std::vector<VkBuffer> m_SSBOMaterials;
		std::vector<VkBuffer> m_SSBOAABBs;
		std::vector<VkBuffer> m_SSBOLights;
		//std::vector<VkBuffer> m_SSBOSpheres;

		std::vector<VkDeviceMemory> m_SSBOTrianglesMemory;
		std::vector<VkDeviceMemory> m_SSBOMaterialsMemory;
		std::vector<VkDeviceMemory> m_SSBOAABBsMemory;
		std::vector<VkDeviceMemory> m_SSBOLightsMemory;
		//std::vector<VkDeviceMemory> m_SSBOSpheresMemory;

		VkImageView m_TargetTextureImageView;
		VkImageView m_AccumulationTextureImageView;

		VkImage m_TargetTexture;
		VkImage m_AccumulationTexture;

		VkDeviceMemory m_TargetTextureImageMemory;
		VkDeviceMemory m_AccumulationTextureImageMemory;

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


		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;

		// time
		double m_LastTime = 0.0f;
		double m_LastFrameTime;

		//scene
		RayTraceScene* mScene;

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