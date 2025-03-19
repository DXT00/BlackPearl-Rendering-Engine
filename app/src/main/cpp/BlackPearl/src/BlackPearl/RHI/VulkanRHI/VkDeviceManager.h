#pragma once

#include "BlackPearl\Renderer\DeviceManager.h"

#if GE_API_VULKAN

namespace BlackPearl {
	class VKDeviceManager : public DeviceManager
	{
	public:
		virtual ~DeviceManager() override{}
		[[nodiscard]] IDevice* GetDevice() const override
		{
			if (m_ValidationLayer)
				return m_ValidationLayer;

			return m_NvrhiDevice;
		}

		[[nodiscard]] DynamicRHI::Type GetGraphicsAPI() const override
		{
			return DynamicRHI::Type::Vulkan;
		}
		
	protected:
		virtual IFramebuffer* GetCurrentFramebuffer() override;

		bool CreateDeviceAndSwapChain() override;
		void DestroyDeviceAndSwapChain() override;
		void BackBufferResizedInner() override;
		void ResizeSwapChain() override
		{
			if (m_Device)
			{
				destroySwapChain();
				createSwapChain();
			}
		}

		ITexture* GetCurrentBackBuffer() 
		{
			return m_SwapChainImages[m_SwapChainIndex].rhiHandle;
		}
		ITexture* GetBackBuffer(uint32_t index) 
		{
			GE_CORE_INFO("backbufer id " + std::to_string(index));
			if (index < m_SwapChainImages.size())
				return m_SwapChainImages[index].rhiHandle;
			return nullptr;
		}
		uint32_t GetCurrentBackBufferIndex() 
		{
			return m_SwapChainIndex;
		}
		uint32_t GetBackBufferCount()
		{
			return uint32_t(m_SwapChainImages.size());
		}

		void BeginFrame() override;
		void Present() override;

		const char* GetRendererString() const override
		{
			return m_RendererString.c_str();
		}

		bool IsVulkanInstanceExtensionEnabled(const char* extensionName) const override
		{
			return enabledExtensions.instance.find(extensionName) != enabledExtensions.instance.end();
		}

		bool IsVulkanDeviceExtensionEnabled(const char* extensionName) const override
		{
			return enabledExtensions.device.find(extensionName) != enabledExtensions.device.end();
		}

		bool IsVulkanLayerEnabled(const char* layerName) const override
		{
			return enabledExtensions.layers.find(layerName) != enabledExtensions.layers.end();
		}

		void GetEnabledVulkanInstanceExtensions(std::vector<std::string>& extensions) const override
		{
			for (const auto& ext : enabledExtensions.instance)
				extensions.push_back(ext);
		}

		void GetEnabledVulkanDeviceExtensions(std::vector<std::string>& extensions) const override
		{
			for (const auto& ext : enabledExtensions.device)
				extensions.push_back(ext);
		}

		void GetEnabledVulkanLayers(std::vector<std::string>& layers) const override
		{
			for (const auto& ext : enabledExtensions.layers)
				layers.push_back(ext);
		}

	private:

		std::vector<FramebufferHandle> m_SwapChainFramebuffers;

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool isComplete() {
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};
		bool createInstance();
		bool createWindowSurface();
		void installDebugCallback();
		bool pickPhysicalDevice();
		bool findQueueFamilies(VkPhysicalDevice physicalDevice);
		bool createDevice();
		bool createSwapChain();
		void destroySwapChain();
		bool _isDeviceSuitable(VkPhysicalDevice device);
		bool _checkDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails _querySwapChainSupport(VkPhysicalDevice device);

		static VKAPI_ATTR VkBool32 VKAPI_CALL _debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}

		void _populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = _debugCallback;
		}
		struct VulkanExtensionSet
		{
			std::unordered_set<std::string> instance;
			std::unordered_set<std::string> layers;
			std::unordered_set<std::string> device;
		};

		// minimal set of required extensions
		VulkanExtensionSet enabledExtensions = {
			// instance
			{
				VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
			},
			// layers
			{ },
			// device
			{
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				VK_KHR_MAINTENANCE1_EXTENSION_NAME
			},
		};

		// optional extensions
		VulkanExtensionSet optionalExtensions = {
			// instance
			{
				VK_EXT_SAMPLER_FILTER_MINMAX_EXTENSION_NAME,
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME
			},
			// layers
			{ },
			// device
			{
				VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
				VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
				//VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
				VK_NV_MESH_SHADER_EXTENSION_NAME,
				VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
				//VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
				//VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME,
				VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME
			},
		};

		std::unordered_set<std::string> m_RayTracingExtensions = {
			VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
			VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
			VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
			VK_KHR_RAY_QUERY_EXTENSION_NAME,
			VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME
		};

		std::string m_RendererString;

		VkInstance m_Instance;
		VkDebugReportCallbackEXT m_DebugReportCallback = nullptr;

		VkPhysicalDevice m_PhysicalDevice;
		int m_GraphicsQueueFamily = -1;
		int m_ComputeQueueFamily = -1;
		int m_TransferQueueFamily = -1;
		int m_PresentQueueFamily = -1;

		VkDevice m_Device;
		VkQueue m_GraphicsQueue;
		VkQueue m_ComputeQueue;
		VkQueue m_TransferQueue;
		VkQueue m_PresentQueue;

		VkSurfaceKHR m_WindowSurface;

		VkSurfaceFormatKHR m_SwapChainFormat;
		VkSwapchainKHR m_SwapChain;

		struct SwapChainImage
		{
			VkImage image;
			TextureHandle rhiHandle;
		};

		std::vector<SwapChainImage> m_SwapChainImages;
		uint32_t m_SwapChainIndex = uint32_t(-1);

		DeviceHandle m_NvrhiDevice;
		DeviceHandle m_ValidationLayer;

		CommandListHandle m_BarrierCommandList;
		VkSemaphore m_PresentSemaphore;

		std::queue<EventQueryHandle> m_FramesInFlight;
		std::vector<EventQueryHandle> m_QueryPool;

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
			VkDebugReportFlagsEXT flags,
			VkDebugReportObjectTypeEXT objType,
			uint64_t obj,
			size_t location,
			int32_t code,
			const char* layerPrefix,
			const char* msg,
			void* userData)
		{
			const VKDeviceManager* manager = (const VKDeviceManager*)userData;

			if (manager)
			{
				const auto& ignored = manager->m_DeviceParams.ignoredVulkanValidationMessageLocations;
				const auto found = std::find(ignored.begin(), ignored.end(), location);
				if (found != ignored.end())
					return VK_FALSE;
			}

			GE_CORE_WARN("[Vulkan: location=0x%zx code=%d, layerPrefix='%s'] %s", location, code, layerPrefix, msg);

			return VK_FALSE;
		}

	};
}

#endif