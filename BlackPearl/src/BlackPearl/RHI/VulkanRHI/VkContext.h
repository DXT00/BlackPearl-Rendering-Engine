#pragma once
#include<vulkan/vulkan.h>
namespace BlackPearl {

	class VulkanContext
	{
	public:
		VulkanContext(VkInstance instance,
			VkPhysicalDevice physicalDevice,
			VkDevice device,
			VkAllocationCallbacks* allocationCallbacks = nullptr)
			: instance(instance)
			, physicalDevice(physicalDevice)
			, device(device)
			, allocationCallbacks(allocationCallbacks)
			, pipelineCache(nullptr)
		{ }

		VkInstance instance;
		VkPhysicalDevice physicalDevice;
		VkDevice device;
		VkAllocationCallbacks* allocationCallbacks;
		VkPipelineCache pipelineCache;

		struct {
			bool KHR_synchronization2 = false;
			bool KHR_maintenance1 = false;
			bool EXT_debug_report = false;
			bool EXT_debug_marker = false;
			bool KHR_acceleration_structure = false;
			bool buffer_device_address = false; // either KHR_ or Vulkan 1.2 versions
			bool KHR_ray_query = false;
			bool KHR_ray_tracing_pipeline = false;
			bool NV_mesh_shader = false;
			bool KHR_fragment_shading_rate = false;
			bool EXT_conservative_rasterization = false;
			bool EXT_opacity_micromap = false;
		} extensions;

		VkPhysicalDeviceProperties physicalDeviceProperties{};
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR rayTracingPipelineProperties{};
		VkPhysicalDeviceAccelerationStructurePropertiesKHR accelStructProperties{};
		VkPhysicalDeviceConservativeRasterizationPropertiesEXT conservativeRasterizationProperties{};
		VkPhysicalDeviceFragmentShadingRatePropertiesKHR shadingRateProperties{};
		VkPhysicalDeviceOpacityMicromapPropertiesEXT opacityMicromapProperties{};
		VkPhysicalDeviceFragmentShadingRateFeaturesKHR shadingRateFeatures{};


		/*void nameVKObject(const void* handle, VkDebugReportObjectTypeEXT objtype, const char* name) const;
		void error(const std::string& message) const;
		void warning(const std::string& message) const;*/
	};


}

