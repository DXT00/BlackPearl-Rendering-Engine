#pragma once

//#define VK_USE_PLATFORM_WIN32_KHR
#ifdef GE_API_VULKAN
#include "vulkan/vulkan_core.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "BasicRenderer.h"
#include "BlackPearl/Scene/RayTraceScene.h"
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
		void CreateDepthImageViews();

		void CreateDescriptorPool();
		void CreateRenderPass();

		//compute shader pipeline -->rayTrace
		void CreateComputePipeline();
		void CreateComputeDescriptorSets();
		void CreateShaderStorageBuffers();
		void CreateComputeDescriptorSetLayout();
		void CreateComputeDescriptorPool();
		void CreateComputeTextureImage();
		void CreateComputeTextureImageView();

		//graphic shader pipeline -->denoise
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		void CreateGraphicsPipeline();
		void CreateDescriptorSetLayout();
		void CreateDescriptorSets();
		void CreateTextureSampler();

		//graphic shader pipeline -->postProcess

		void CreatePostProcessGraphicsPipeline();
		void CreatePostProcessDescriptorSetLayout();
		void CreatePostProcessDescriptorSets();

		
		void CreateAttachment();
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

		struct DenoiseUniformBufferObject {
			alignas(8) glm::vec2 screenSize;
			alignas(16) glm::mat4 preProjectionView;
		};

		struct QuadVertex
		{
			math::float3 position;
			math::float3 normal;
			glm::vec2 color;


		};
		struct Vertex {
			glm::vec2 pos;
			math::float3 color;
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
			math::float3 position;
			math::float3 normal;
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
		glm::vec3 m_LastCamRotate;

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
		
		void BeginRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		void RecordDenoiseCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecordComputeCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex);
		void RecordPostProcessCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void RecordLastFrameCommandBuffer( uint32_t imageIndex);

		void EndRenderPass(VkCommandBuffer commandBuffer, uint32_t imageIndex);

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
		std::vector <VkImage> m_DepthImage;
		std::vector <VkImageView> m_DepthStencilView;
		VkFormat m_DepthFormat = VK_FORMAT_D32_SFLOAT;
		std::vector <VkDeviceMemory> m_DepthImageMemory;

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		// postProces -- grapghics pipline
		std::vector<VkImageView> m_ImageViews;

		std::vector<VkFramebuffer> m_FrameBuffers;
		VkRenderPass m_RenderPass;

		VkSampler m_TextureSampler;

		struct {
			VkPipeline denoise;
			VkPipeline computeRaytrace;
			VkPipeline postprocess;
		} m_Pipelines;

		struct {
			VkPipelineLayout denoise;
			VkPipelineLayout computeRaytrace;
			VkPipelineLayout postprocess;
		} m_PipelineLayouts;

		struct {
			VkDescriptorSetLayout denoise;
			VkDescriptorSetLayout computeRaytrace;
			VkDescriptorSetLayout postprocess;
		} m_DescriptorSetLayouts;

		struct {
			std::vector<VkDescriptorSet>  denoise;
			std::vector<VkDescriptorSet>  computeRaytrace;
			std::vector<VkDescriptorSet>  postprocess;
		} m_DescriptorSets;

		// Denoise pass framebuffer attachments
		struct FrameBufferAttachment {
			VkImage image = VK_NULL_HANDLE;
			VkDeviceMemory mem = VK_NULL_HANDLE;
			VkImageView view = VK_NULL_HANDLE;
			VkFormat format;
		};

		struct Attachments {
			FrameBufferAttachment denoiseColor;
			int32_t width;
			int32_t height;
		} m_Attachments;


		VkBuffer m_VertexBuffer;
		VkBuffer m_IndexBuffer;

		VkDeviceMemory m_VertexBufferMemory;
		VkDeviceMemory m_IndexBufferMemory;

		struct DescriptorPool {
			VkDescriptorPool denoise, computeRaytrace, postprocess;
		} m_DescriptorPools;

		// raytracing -- compute pipline
		//compute shader
		std::vector<VkBuffer> m_SSBOTriangles;
		std::vector<VkBuffer> m_SSBOMaterials;
		std::vector<VkBuffer> m_SSBOAABBs;
		std::vector<VkBuffer> m_SSBOLights;
		std::vector<VkBuffer> m_SSBOObjTransforms;

		//std::vector<VkBuffer> m_SSBOSpheres;

		std::vector<VkDeviceMemory> m_SSBOTrianglesMemory;
		std::vector<VkDeviceMemory> m_SSBOMaterialsMemory;
		std::vector<VkDeviceMemory> m_SSBOAABBsMemory;
		std::vector<VkDeviceMemory> m_SSBOLightsMemory;
		std::vector<VkDeviceMemory> m_SSBOObjTransformsMemory;

		//std::vector<VkDeviceMemory> m_SSBOSpheresMemory;

		VkImageView m_TargetTexturePosImageView;
		VkImageView m_TargetTextureNormalImageView;
		VkImageView m_TargetTextureDepthImageView;
		VkImageView m_TargetTextureImageView;
		VkImageView m_AccumulationTextureImageView;
		VkImageView m_LastFrameImageView;
		VkImageView m_DenoiseColorImageView;


		VkImage m_TargetTexture;
		VkImage m_TargetPosTexture;
		VkImage m_TargetNormalTexture;
		VkImage m_TargetDepthTexture;
		VkImage m_AccumulationTexture;
		VkImage m_LastFrameTexture;
		VkImage m_DenoiseColorTexture;

		VkDeviceMemory m_TargetTextureImageMemory;
		VkDeviceMemory m_TargetPosImageMemory;
		VkDeviceMemory m_TargetNormalImageMemory;
		VkDeviceMemory m_TargetDepthImageMemory;
		VkDeviceMemory m_AccumulationTextureImageMemory;
		VkDeviceMemory m_LastFrameTextureImageMemory;
		VkDeviceMemory m_DenoiseColorTextureImageMemory;

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

		std::vector<VkBuffer> m_DenoiseUniformBuffers;
		std::vector<VkDeviceMemory> m_DenoiseUniformBuffersMemory;
		std::vector<void*> m_DenoiseUniformBuffersMapped;


		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMemory;
		std::vector<void*> m_UniformBuffersMapped;

		// time
		double m_LastTime = 0.0f;
		double m_LastFrameTime;

		glm::mat4 m_PreProjectionView = glm::mat4(1.0);

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