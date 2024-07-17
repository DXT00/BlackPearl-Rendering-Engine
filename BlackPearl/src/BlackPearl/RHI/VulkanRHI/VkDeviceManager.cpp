/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

/*
License for glfw

Copyright (c) 2002-2006 Marcus Geelnard

Copyright (c) 2006-2019 Camilla Lowy

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would
   be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not
   be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
   distribution.
*/
#include "pch.h"
#if GE_API_VULKAN

#include <string>
#include <queue>
#include <unordered_set>


//#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
//#include <vulkan/vulkan.h>

#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Application.h"
#include "VkDevice.h"
#include "VkUtil.h"
#include "VkRenderConfig.h"

// Define the Vulkan dynamic dispatcher - this needs to occur in exactly one cpp file in the program.


namespace BlackPearl {
	class VKDeviceManager : public DeviceManager
	{
	public:
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
		bool CreateDeviceAndSwapChain() override;
		void DestroyDeviceAndSwapChain() override;

		void ResizeSwapChain() override
		{
			if (m_Device)
			{
				destroySwapChain();
				createSwapChain();
			}
		}

		ITexture* GetCurrentBackBuffer() override
		{
			return m_SwapChainImages[m_SwapChainIndex].rhiHandle;
		}
		ITexture* GetBackBuffer(uint32_t index) override
		{
			if (index < m_SwapChainImages.size())
				return m_SwapChainImages[index].rhiHandle;
			return nullptr;
		}
		uint32_t GetCurrentBackBufferIndex() override
		{
			return m_SwapChainIndex;
		}
		uint32_t GetBackBufferCount() override
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
				VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
				VK_NV_MESH_SHADER_EXTENSION_NAME,
				VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
				VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME,
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

	static std::vector<const char*> stringSetToVector(const std::unordered_set<std::string>& set)
	{
		std::vector<const char*> ret;
		for (const auto& s : set)
		{
			ret.push_back(s.c_str());
		}

		return ret;
	}

	template <typename T>
	static std::vector<T> setToVector(const std::unordered_set<T>& set)
	{
		std::vector<T> ret;
		for (const auto& s : set)
		{
			ret.push_back(s);
		}

		return ret;
	}

	bool VKDeviceManager::createInstance()
	{
		if (!glfwVulkanSupported())
		{
			return false;
		}

		// add any extensions required by GLFW
		uint32_t glfwExtCount = 0;
		const char** glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::vector<const char*> exts(glfwExt, glfwExt + glfwExtCount);

		assert(glfwExt);

		for (uint32_t i = 0; i < glfwExtCount; i++)
		{
			auto a = glfwExt[i];
			enabledExtensions.instance.insert(std::string(glfwExt[i]));
		}

		if (enableValidationLayers) {
			enabledExtensions.instance.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			enabledExtensions.layers.insert("VK_LAYER_KHRONOS_validation");

		}

		// add instance extensions requested by the user
		for (const std::string& name : m_DeviceParams.requiredVulkanInstanceExtensions)
		{
			enabledExtensions.instance.insert(name);
		}
		
		for (const std::string& name : m_DeviceParams.optionalVulkanInstanceExtensions)
		{
			optionalExtensions.instance.insert(name);
		}

		// add layers requested by the user
		for (const std::string& name : m_DeviceParams.requiredVulkanLayers)
		{
			enabledExtensions.layers.insert(name);
		}
		for (const std::string& name : m_DeviceParams.optionalVulkanLayers)
		{
			optionalExtensions.layers.insert(name);
		}

		std::unordered_set<std::string> requiredExtensions = enabledExtensions.instance;
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		// figure out which optional extensions are supported
		for (const auto& instanceExt : extensions)
		{
			const std::string name = instanceExt.extensionName;
			if (optionalExtensions.instance.find(name) != optionalExtensions.instance.end())
			{
				enabledExtensions.instance.insert(name);
			}

			requiredExtensions.erase(name);
		}

		if (!requiredExtensions.empty())
		{
			std::stringstream ss;
			ss << "Cannot create a Vulkan instance because the following required extension(s) are not supported:";
			for (const auto& ext : requiredExtensions)
				ss << std::endl << "  - " << ext;

			GE_CORE_ERROR("%s", ss.str().c_str());
			return false;
		}

		GE_CORE_INFO("Enabled Vulkan instance extensions:");
		for (const auto& ext : enabledExtensions.instance)
		{
			GE_CORE_INFO(ext.c_str());
		}


		std::unordered_set<std::string> requiredLayers = enabledExtensions.layers;

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const auto& layer : availableLayers)
		{
			const std::string name = layer.layerName;
			if (optionalExtensions.layers.find(name) != optionalExtensions.layers.end())
			{
				enabledExtensions.layers.insert(name);
			}

			requiredLayers.erase(name);
		}

		if (!requiredLayers.empty())
		{
			std::stringstream ss;
			ss << "Cannot create a Vulkan instance because the following required layer(s) are not supported:";
			for (const auto& ext : requiredLayers)
				ss << std::endl << "  - " << ext;

			GE_CORE_ERROR("%s", ss.str().c_str());
			return false;
		}

		GE_CORE_INFO( "Enabled Vulkan layers:");
		for (const auto& layer : enabledExtensions.layers)
		{
			GE_CORE_INFO(layer.c_str());
		}


		auto instanceExtVec = stringSetToVector(enabledExtensions.instance);
		auto layerVec = stringSetToVector(enabledExtensions.layers);

		/*VkApplicationInfo applicationInfo;
		applicationInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);*/
		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "BlackPearl VkSandBox";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 2, 0);
		applicationInfo.pEngineName = "BlackPearl";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 2, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_2;

		// create the vulkan instance
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.enabledLayerCount = uint32_t(layerVec.size());
		createInfo.ppEnabledLayerNames = layerVec.data();
		createInfo.enabledExtensionCount = uint32_t(instanceExtVec.size());
		createInfo.ppEnabledExtensionNames = instanceExtVec.data();
		createInfo.pApplicationInfo = &applicationInfo;


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			_populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.pNext = nullptr;
		}
		const VkResult res = vkCreateInstance(&createInfo, nullptr, &m_Instance);
		if (res != VkResult::VK_SUCCESS)
		{
			GE_CORE_ERROR("Failed to create a Vulkan instance");
			return false;
		}
		
		//VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Instance);

		return true;
	}

	void VKDeviceManager::installDebugCallback()
	{
		VkDebugReportCallbackCreateInfoEXT info;
		info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		info.pfnCallback = vulkanDebugCallback;
		info.pUserData = this;
		
		/*if(vkCreateDebugReportCallbackEXT(m_Instance ,&info, nullptr, &m_DebugReportCallback) != VK_SUCCESS) {
			throw std::runtime_error("failed to debug report call back !");
		}*/

	}

	bool VKDeviceManager::pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (_isDeviceSuitable(device)) {
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}


	bool VKDeviceManager::findQueueFamilies(VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
		for (int i = 0; i < int(queueFamilies.size()); i++)
		{
			const auto& queueFamily = queueFamilies[i];

			if (m_GraphicsQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					m_GraphicsQueueFamily = i;
				}
			}

			if (m_ComputeQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
					!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					m_ComputeQueueFamily = i;
				}
			}

			if (m_TransferQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					(queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
					!(queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
					!(queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT))
				{
					m_TransferQueueFamily = i;
				}
			}

			if (m_PresentQueueFamily == -1)
			{
				if (queueFamily.queueCount > 0 &&
					glfwGetPhysicalDevicePresentationSupport(m_Instance, physicalDevice, i))
				{
					m_PresentQueueFamily = i;
				}
			}
		}

		if (m_GraphicsQueueFamily == -1 ||
			m_PresentQueueFamily == -1 ||
			(m_ComputeQueueFamily == -1 && m_DeviceParams.enableComputeQueue) ||
			(m_TransferQueueFamily == -1 && m_DeviceParams.enableCopyQueue))
		{
			return false;
		}

		return true;
	}

	bool VKDeviceManager::createDevice()
	{
		// figure out which optional extensions are supported
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> deviceExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, nullptr, &extensionCount, deviceExtensions.data());
		std::unordered_set<std::string> requiredDeviceExt = enabledExtensions.device;

		for (const auto& ext : deviceExtensions)
		{
			const std::string name = ext.extensionName;
			if (optionalExtensions.device.find(name) != optionalExtensions.device.end())
			{
				enabledExtensions.device.insert(name);
			}

			if (m_DeviceParams.enableRayTracingExtensions && m_RayTracingExtensions.find(name) != m_RayTracingExtensions.end())
			{
				enabledExtensions.device.insert(name);
			}
			requiredDeviceExt.erase(name);
		}

		if (!requiredDeviceExt.empty())
		{
			std::stringstream ss;
			ss << "Cannot create a Vulkan device because the following required device(s) are not supported:";
			for (const auto& ext : requiredDeviceExt)
				ss << std::endl << "  - " << ext;

			GE_CORE_ERROR(ss.str().c_str());
			return false;
		}

		bool accelStructSupported = false;
		bool bufferAddressSupported = false;
		bool rayPipelineSupported = false;
		bool rayQuerySupported = false;
		bool meshletsSupported = false;
		bool vrsSupported = false;
		bool s2Supported = false;
		bool ommSupported = false;

		GE_CORE_INFO( "Enabled Vulkan device extensions:");
		for (const auto& ext : enabledExtensions.device)
		{
			GE_CORE_INFO(ext.c_str());

			if (ext == VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME)
				accelStructSupported = true;
			else if (ext == VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME)
				bufferAddressSupported = true;
			else if (ext == VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME)
				rayPipelineSupported = true;
			else if (ext == VK_KHR_RAY_QUERY_EXTENSION_NAME)
				rayQuerySupported = true;
			else if (ext == VK_NV_MESH_SHADER_EXTENSION_NAME)
				meshletsSupported = true;
			else if (ext == VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME)
				vrsSupported = true;
			else if (ext == VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME)
				s2Supported = true;
			else if (ext == VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME)
				ommSupported = true;
		}

		std::unordered_set<int> uniqueQueueFamilies = {
			m_GraphicsQueueFamily,
			m_PresentQueueFamily };

		if (m_DeviceParams.enableComputeQueue)
			uniqueQueueFamilies.insert(m_ComputeQueueFamily);

		if (m_DeviceParams.enableCopyQueue)
			uniqueQueueFamilies.insert(m_TransferQueueFamily);

		float priority = 1.f;
		std::vector<VkDeviceQueueCreateInfo> queueDesc;
	/*	for (int queueFamily : uniqueQueueFamilies)
		{
			queueDesc.push_back(vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(queueFamily)
				.setQueueCount(1)
				.setPQueuePriorities(&priority));
		}*/
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;
			queueDesc.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceAccelerationStructureFeaturesKHR accelStructFeatures{};
		accelStructFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
		accelStructFeatures.accelerationStructure = true;

		VkPhysicalDeviceBufferAddressFeaturesEXT bufferAddressFeatures{};
		bufferAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_EXT;
		bufferAddressFeatures.bufferDeviceAddress = true;
		//.setBufferDeviceAddress(true);
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayPipelineFeatures{};
		rayPipelineFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
		rayPipelineFeatures.rayTracingPipeline = true;
		rayPipelineFeatures.rayTraversalPrimitiveCulling = true;

		VkPhysicalDeviceRayQueryFeaturesKHR rayQueryFeatures{};
		rayQueryFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
		rayQueryFeatures.rayQuery = true;

		VkPhysicalDeviceMeshShaderFeaturesNV meshletFeatures{};
		meshletFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;
		meshletFeatures.taskShader = true;
		meshletFeatures.meshShader = true;
		
		VkPhysicalDeviceFragmentShadingRateFeaturesKHR vrsFeatures{};
		vrsFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;
		vrsFeatures.pipelineFragmentShadingRate = true;
		vrsFeatures.primitiveFragmentShadingRate = true;
		vrsFeatures.attachmentFragmentShadingRate = true;


		VkPhysicalDeviceSynchronization2Features s2Features{};
		s2Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
		s2Features.synchronization2 = true;
		VkPhysicalDeviceOpacityMicromapFeaturesEXT ommFeatures{};
		ommFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_OPACITY_MICROMAP_FEATURES_EXT;
		ommFeatures.micromap = true;


		void* pNext = nullptr;
#define APPEND_EXTENSION(condition, desc) if (condition) { (desc).pNext = pNext; pNext = &(desc); }  // NOLINT(cppcoreguidelines-macro-usage)
		APPEND_EXTENSION(accelStructSupported, accelStructFeatures)
		APPEND_EXTENSION(bufferAddressSupported, bufferAddressFeatures)
		APPEND_EXTENSION(rayPipelineSupported, rayPipelineFeatures)
		APPEND_EXTENSION(rayQuerySupported, rayQueryFeatures)
		APPEND_EXTENSION(meshletsSupported, meshletFeatures)
		APPEND_EXTENSION(vrsSupported, vrsFeatures)
		APPEND_EXTENSION(s2Supported, s2Features)
		APPEND_EXTENSION(ommSupported, ommFeatures)
#undef APPEND_EXTENSION

		// Determine support for Buffer Device Address, the Vulkan 1.2 way
		VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
		physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures{};
		bufferDeviceAddressFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;

		physicalDeviceFeatures2.pNext = &bufferDeviceAddressFeatures;
		vkGetPhysicalDeviceFeatures2(m_PhysicalDevice, &physicalDeviceFeatures2);

		VkPhysicalDeviceFeatures deviceFeatures{};
		
		 //.setShaderImageGatherExtended(true)
			// .setSamplerAnisotropy(true)
			// .setTessellationShader(true)
			// .setTextureCompressionBC(true)
			// .setGeometryShader(true)
			// .setImageCubeArray(true)
			// .setDualSrcBlend(true)
			// .setVertexPipelineStoresAndAtomics(true) // needed for OMM baking
			// .setFragmentStoresAndAtomics(true); // also for OMM baking

		 deviceFeatures.shaderImageGatherExtended = true;
		 deviceFeatures.samplerAnisotropy = true;
		 deviceFeatures.tessellationShader = true;
		 deviceFeatures.geometryShader = true;
		 deviceFeatures.imageCubeArray = true;
		 deviceFeatures.dualSrcBlend = true;
		 deviceFeatures.vertexPipelineStoresAndAtomics = true;
		 deviceFeatures.fragmentStoresAndAtomics = true;

	/*	 .setDescriptorIndexing(true)
			 .setRuntimeDescriptorArray(true)
			 .setDescriptorBindingPartiallyBound(true)
			 .setDescriptorBindingVariableDescriptorCount(true)
			 .setTimelineSemaphore(true)
			 .setShaderSampledImageArrayNonUniformIndexing(true)
			 .setBufferDeviceAddress(bufferDeviceAddressFeatures.bufferDeviceAddress)
			 .setPNext(pNext);*/

		VkPhysicalDeviceVulkan12Features vulkan12features{};
		vulkan12features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		vulkan12features.descriptorIndexing = VK_TRUE;
		vulkan12features.runtimeDescriptorArray = true;
		vulkan12features.descriptorBindingPartiallyBound = true;
		vulkan12features.descriptorBindingVariableDescriptorCount = true;
		vulkan12features.timelineSemaphore = true;
		vulkan12features.shaderSampledImageArrayNonUniformIndexing = true;
		vulkan12features.bufferDeviceAddress = bufferDeviceAddressFeatures.bufferDeviceAddress;
		vulkan12features.pNext = pNext;


		auto layerVec = stringSetToVector(enabledExtensions.layers);
		auto extVec = stringSetToVector(enabledExtensions.device);

		VkDeviceCreateInfo deviceDesc{};

		deviceDesc.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		deviceDesc.queueCreateInfoCount = static_cast<uint32_t>(queueDesc.size());
		deviceDesc.pQueueCreateInfos = queueDesc.data();

		deviceDesc.pEnabledFeatures = &deviceFeatures;

		deviceDesc.enabledExtensionCount = static_cast<uint32_t>(extVec.size());
		deviceDesc.ppEnabledExtensionNames = extVec.data();

		deviceDesc.enabledLayerCount = static_cast<uint32_t>(layerVec.size());
		deviceDesc.ppEnabledLayerNames = layerVec.data();
		deviceDesc.pNext = &vulkan12features;


		/*
			.setPQueueCreateInfos(queueDesc.data())
			.setQueueCreateInfoCount(uint32_t(queueDesc.size()))
			.setPEnabledFeatures(&deviceFeatures)
			.setEnabledExtensionCount(uint32_t(extVec.size()))
			.setPpEnabledExtensionNames(extVec.data())
			.setEnabledLayerCount(uint32_t(layerVec.size()))
			.setPpEnabledLayerNames(layerVec.data())
			.setPNext(&vulkan12features);*/

		if (m_DeviceParams.deviceCreateInfoCallback)
			m_DeviceParams.deviceCreateInfoCallback(deviceDesc);

		if (vkCreateDevice(m_PhysicalDevice, &deviceDesc, nullptr, &m_Device) != VkResult::VK_SUCCESS) {
			GE_CORE_ERROR("failed to create device");
			return false;
		};
		

		//m_Device.getQueue(m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
		if (m_DeviceParams.enableComputeQueue) {
			vkGetDeviceQueue(m_Device, m_ComputeQueueFamily, 0, &m_ComputeQueue);

		}
		if (m_DeviceParams.enableCopyQueue) {
			vkGetDeviceQueue(m_Device, m_TransferQueueFamily, 0, &m_TransferQueue);

		}
		vkGetDeviceQueue(m_Device, m_PresentQueueFamily, 0, &m_PresentQueue);

		

		//VULKAN_HPP_DEFAULT_DISPATCHER.init(m_Device);

		// stash the renderer string
		VkPhysicalDeviceProperties prop{};
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &prop);
		
		//m_RendererString = std::string(prop.deviceName.data());

		GE_CORE_INFO(prop.deviceName);

		return true;
	}

	bool VKDeviceManager::createWindowSurface()
	{

		if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), nullptr, &m_WindowSurface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
			return false;
		}
		return true;
	}

	void VKDeviceManager::destroySwapChain()
	{
		if (m_Device)
		{
			//m_Device.waitIdle();
			vkDeviceWaitIdle(m_Device);
		}

		if (m_SwapChain)
		{
			vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
			//m_Device.destroySwapchainKHR(m_SwapChain);
			m_SwapChain = nullptr;
		}

		m_SwapChainImages.clear();
	}

	bool VKDeviceManager::createSwapChain()
	{
		destroySwapChain();

		m_SwapChainFormat = {
			VkFormat(VkUtil::convertFormat(m_DeviceParams.swapChainFormat)),
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};

		VkExtent2D extent{};
		extent.width = m_DeviceParams.backBufferWidth;
		extent.height = m_DeviceParams.backBufferHeight;

		SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(m_PhysicalDevice);
		uint32_t imageCount = m_DeviceParams.swapChainBufferCount;// swapChainSupport.capabilities.minImageCount + 1;
		if (imageCount < swapChainSupport.capabilities.minImageCount) {
			imageCount = swapChainSupport.capabilities.minImageCount;
		}
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		std::unordered_set<uint32_t> uniqueQueues = {
			uint32_t(m_GraphicsQueueFamily),
			uint32_t(m_PresentQueueFamily) };

		std::vector<uint32_t> queues = setToVector(uniqueQueues);

		const bool enableSwapChainSharing = queues.size() > 1;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_WindowSurface;

		createInfo.minImageCount = imageCount;// m_DeviceParams.swapChainBufferCount;
		createInfo.imageFormat = m_SwapChainFormat.format;
		createInfo.imageColorSpace = m_SwapChainFormat.colorSpace;
		createInfo.imageExtent = extent;
		/** The imageArrayLayers specifies the amount of layers each image consists of.  */
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		createInfo.imageSharingMode = enableSwapChainSharing? VK_SHARING_MODE_CONCURRENT: VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = enableSwapChainSharing ? uint32_t(queues.size()) : 0;
		createInfo.pQueueFamilyIndices = enableSwapChainSharing ? queues.data() : nullptr;
	

		createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_DeviceParams.vsyncEnabled ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}


		//}
		
		// retrieve swap chain images
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.clear();
		std::vector<VkImage> images;
		images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, images.data());

		//auto images = m_Device.getSwapchainImagesKHR(m_SwapChain);
		for (auto image : images)
		{
			SwapChainImage sci;
			sci.image = image;

			TextureDesc textureDesc;
			textureDesc.width = m_DeviceParams.backBufferWidth;
			textureDesc.height = m_DeviceParams.backBufferHeight;
			textureDesc.format = m_DeviceParams.swapChainFormat;
			textureDesc.debugName = "Swap chain image";
			textureDesc.initialState = ResourceStates::Present;
			textureDesc.keepInitialState = true;
			textureDesc.isRenderTarget = true;

			sci.rhiHandle = m_NvrhiDevice->createHandleForNativeTexture(ObjectTypes::VK_Image, RHIObject(sci.image), textureDesc);
			m_SwapChainImages.push_back(sci);
		}

		m_SwapChainIndex = 0;

		return true;
	}

	bool VKDeviceManager::CreateDeviceAndSwapChain()
	{
		if (m_DeviceParams.enableDebugRuntime)
		{
			enabledExtensions.instance.insert("VK_EXT_debug_report");
			enabledExtensions.layers.insert("VK_LAYER_KHRONOS_validation");
		}

		//const vk::DynamicLoader dl;
		//const PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =   // NOLINT(misc-misplaced-const)
		//	dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
		//VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

#define CHECK(a) if (!(a)) { return false; }

		CHECK(createInstance())

			if (m_DeviceParams.enableDebugRuntime)
			{
				installDebugCallback();
			}

		if (m_DeviceParams.swapChainFormat == Format::SRGBA8_UNORM)
			m_DeviceParams.swapChainFormat = Format::SBGRA8_UNORM;
		else if (m_DeviceParams.swapChainFormat == Format::RGBA8_UNORM)
			m_DeviceParams.swapChainFormat = Format::BGRA8_UNORM;

		// add device extensions requested by the user
		for (const std::string& name : m_DeviceParams.requiredVulkanDeviceExtensions)
		{
			enabledExtensions.device.insert(name);
		}
		for (const std::string& name : m_DeviceParams.optionalVulkanDeviceExtensions)
		{
			optionalExtensions.device.insert(name);
		}

		CHECK(createWindowSurface())
		CHECK(pickPhysicalDevice())
		CHECK(findQueueFamilies(m_PhysicalDevice))
		CHECK(createDevice())

		auto vecInstanceExt = stringSetToVector(enabledExtensions.instance);
		auto vecLayers = stringSetToVector(enabledExtensions.layers);
		auto vecDeviceExt = stringSetToVector(enabledExtensions.device);

		DeviceDesc deviceDesc;
		deviceDesc.errorCB = &DefaultMessageCallback::GetInstance();
		deviceDesc.instance = m_Instance;
		deviceDesc.physicalDevice = m_PhysicalDevice;
		deviceDesc.device = m_Device;
		deviceDesc.graphicsQueue = m_GraphicsQueue;
		deviceDesc.graphicsQueueIndex = m_GraphicsQueueFamily;
		if (m_DeviceParams.enableComputeQueue)
		{
			deviceDesc.computeQueue = m_ComputeQueue;
			deviceDesc.computeQueueIndex = m_ComputeQueueFamily;
		}
		if (m_DeviceParams.enableCopyQueue)
		{
			deviceDesc.transferQueue = m_TransferQueue;
			deviceDesc.transferQueueIndex = m_TransferQueueFamily;
		}
		deviceDesc.instanceExtensions = vecInstanceExt.data();
		deviceDesc.numInstanceExtensions = vecInstanceExt.size();
		deviceDesc.deviceExtensions = vecDeviceExt.data();
		deviceDesc.numDeviceExtensions = vecDeviceExt.size();
		deviceDesc.bufferDeviceAddressSupported = true;

		m_NvrhiDevice = Device::createDevice(deviceDesc);

		/*if (m_DeviceParams.enableNvrhiValidationLayer)
		{
			m_ValidationLayer = validation::createValidationLayer(m_NvrhiDevice);
		}*/

		CHECK(createSwapChain())

		m_BarrierCommandList = m_NvrhiDevice->createCommandList();
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;	
		vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_PresentSemaphore);

#undef CHECK

		return true;
	}

	bool VKDeviceManager::_isDeviceSuitable(VkPhysicalDevice device) {
		findQueueFamilies(device);

		bool extensionsSupported = _checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = _querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return (m_GraphicsQueueFamily!=-1 || m_ComputeQueueFamily!=-1) 
			&& m_PresentQueueFamily != -1 
			&& extensionsSupported 
			&& swapChainAdequate;
	}

	bool VKDeviceManager::_checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	
	VKDeviceManager::SwapChainSupportDetails VKDeviceManager::_querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_WindowSurface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_WindowSurface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_WindowSurface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_WindowSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_WindowSurface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	void VKDeviceManager::DestroyDeviceAndSwapChain()
	{
		destroySwapChain();

		vkDestroySemaphore(m_Device, m_PresentSemaphore, nullptr);
		//m_Device.destroySemaphore(m_PresentSemaphore);
		//m_PresentSemaphore = vk::Semaphore();

		m_BarrierCommandList = nullptr;

		m_NvrhiDevice = nullptr;
		m_ValidationLayer = nullptr;
		m_RendererString.clear();

		if (m_DebugReportCallback)
		{
			//m_VulkanInstance.destroyDebugReportCallbackEXT(m_DebugReportCallback);
			//vkDestroyDebugReportCallbackEXT(m_Instance, m_DebugReportCallback, nullptr);
		}

		if (m_Device)
		{
			vkDestroyDevice(m_Device, nullptr);
			m_Device = nullptr;
		}

		if (m_WindowSurface)
		{
			assert(m_Instance);
			vkDestroySurfaceKHR(m_Instance,m_WindowSurface, nullptr);
			m_WindowSurface = nullptr;
		}

		if (m_Instance)
		{
			vkDestroyInstance(m_Instance, nullptr);
			//m_VulkanInstance.destroy();
			m_Instance = nullptr;
		}
	}

	void VKDeviceManager::BeginFrame()
	{
		const VkResult res = vkAcquireNextImageKHR(m_Device,m_SwapChain,
			std::numeric_limits<uint64_t>::max(), // timeout
			m_PresentSemaphore,
			VkFence(),
			&m_SwapChainIndex);

		assert(res == VK_SUCCESS);

		m_NvrhiDevice->queueWaitForSemaphore(CommandQueue::Graphics, m_PresentSemaphore, 0);
	}

	void VKDeviceManager::Present()
	{
		m_NvrhiDevice->queueSignalSemaphore(CommandQueue::Graphics, m_PresentSemaphore, 0);

		m_BarrierCommandList->open(); // umm...
		m_BarrierCommandList->close();
		m_NvrhiDevice->executeCommandList(m_BarrierCommandList);


		VkPresentInfoKHR info{};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &m_PresentSemaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_SwapChain;
		info.pImageIndices = &m_SwapChainIndex;

		//const vk::Result res = m_PresentQueue.presentKHR(&info);
		VkResult result = vkQueuePresentKHR(m_PresentQueue, &info);
		GE_ASSERT(result == VK_SUCCESS || result == VK_ERROR_OUT_OF_DATE_KHR, "vkQueuePresentKHR Result error");

		if (m_DeviceParams.enableDebugRuntime)
		{
			// according to vulkan-tutorial.com, "the validation layer implementation expects
			// the application to explicitly synchronize with the GPU"
			vkQueueWaitIdle(m_PresentQueue);
		}
		else
		{
#ifndef _WIN32
			if (m_DeviceParams.vsyncEnabled)
			
				vkQueueWaitIdle(m_PresentQueue);
			}
#endif

			while (m_FramesInFlight.size() > m_DeviceParams.maxFramesInFlight)
			{
				auto query = m_FramesInFlight.front();
				m_FramesInFlight.pop();

				m_NvrhiDevice->waitEventQuery(query);

				m_QueryPool.push_back(query);
			}

			EventQueryHandle query;
			if (!m_QueryPool.empty())
			{
				query = m_QueryPool.back();
				m_QueryPool.pop_back();
			}
			else
			{
				query = m_NvrhiDevice->createEventQuery();
			}

			m_NvrhiDevice->resetEventQuery(query);
			m_NvrhiDevice->setEventQuery(query, CommandQueue::Graphics);
			m_FramesInFlight.push(query);
		}
	}


	DeviceManager* DeviceManager::CreateVK()
	{
		return new VKDeviceManager();
	}
}
#endif
