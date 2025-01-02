#pragma once
#include "vulkan/vulkan_core.h"
#ifndef VK_NO_PROTOTYPES
#	define VK_NO_PROTOTYPES
#endif
namespace BlackPearl {

	class VkFunctionLoader
	{
    public:
        VkFunctionLoader(VkInstance instance, VkDevice device) 
        {
            vkGetDeviceProcAddr = PFN_vkGetDeviceProcAddr(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
#ifdef VK_KHR_ray_tracing_pipeline
            //=== VK_KHR_ray_tracing_pipeline ===
            vkCmdTraceRaysKHR =(PFN_vkCmdTraceRaysKHR)(vkGetInstanceProcAddr(instance, "vkCmdTraceRaysKHR"));
            vkCreateRayTracingPipelinesKHR = (PFN_vkCreateRayTracingPipelinesKHR)(vkGetInstanceProcAddr(instance, "vkCreateRayTracingPipelinesKHR"));
            vkGetRayTracingShaderGroupHandlesKHR = (PFN_vkGetRayTracingShaderGroupHandlesKHR)(vkGetInstanceProcAddr(instance, "vkGetRayTracingShaderGroupHandlesKHR"));
            vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = (PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR)(vkGetInstanceProcAddr(instance, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
            vkCmdTraceRaysIndirectKHR = (PFN_vkCmdTraceRaysIndirectKHR)(vkGetInstanceProcAddr(instance, "vkCmdTraceRaysIndirectKHR"));
            vkGetRayTracingShaderGroupStackSizeKHR = (PFN_vkGetRayTracingShaderGroupStackSizeKHR)(vkGetInstanceProcAddr(instance, "vkGetRayTracingShaderGroupStackSizeKHR"));
            vkCmdSetRayTracingPipelineStackSizeKHR = (PFN_vkCmdSetRayTracingPipelineStackSizeKHR)(vkGetInstanceProcAddr(instance, "vkCmdSetRayTracingPipelineStackSizeKHR"));

#endif

            //=== VK_KHR_acceleration_structure ===
            vkCreateAccelerationStructureKHR = PFN_vkCreateAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkCreateAccelerationStructureKHR"));
            vkDestroyAccelerationStructureKHR = PFN_vkDestroyAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkDestroyAccelerationStructureKHR"));
            vkCmdBuildAccelerationStructuresKHR = PFN_vkCmdBuildAccelerationStructuresKHR(vkGetInstanceProcAddr(instance, "vkCmdBuildAccelerationStructuresKHR"));
            vkCmdBuildAccelerationStructuresIndirectKHR = PFN_vkCmdBuildAccelerationStructuresIndirectKHR(vkGetInstanceProcAddr(instance, "vkCmdBuildAccelerationStructuresIndirectKHR"));
            vkBuildAccelerationStructuresKHR = PFN_vkBuildAccelerationStructuresKHR(vkGetInstanceProcAddr(instance, "vkBuildAccelerationStructuresKHR"));
            vkCopyAccelerationStructureKHR = PFN_vkCopyAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkCopyAccelerationStructureKHR"));
            vkCopyAccelerationStructureToMemoryKHR = PFN_vkCopyAccelerationStructureToMemoryKHR(vkGetInstanceProcAddr(instance, "vkCopyAccelerationStructureToMemoryKHR"));
            vkCopyMemoryToAccelerationStructureKHR = PFN_vkCopyMemoryToAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkCopyMemoryToAccelerationStructureKHR"));
            vkWriteAccelerationStructuresPropertiesKHR = PFN_vkWriteAccelerationStructuresPropertiesKHR(vkGetInstanceProcAddr(instance, "vkWriteAccelerationStructuresPropertiesKHR"));
            vkCmdCopyAccelerationStructureKHR = PFN_vkCmdCopyAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkCmdCopyAccelerationStructureKHR"));
            vkCmdCopyAccelerationStructureToMemoryKHR = PFN_vkCmdCopyAccelerationStructureToMemoryKHR(vkGetInstanceProcAddr(instance, "vkCmdCopyAccelerationStructureToMemoryKHR"));
            vkCmdCopyMemoryToAccelerationStructureKHR = PFN_vkCmdCopyMemoryToAccelerationStructureKHR(vkGetInstanceProcAddr(instance, "vkCmdCopyMemoryToAccelerationStructureKHR"));
            vkGetAccelerationStructureDeviceAddressKHR = PFN_vkGetAccelerationStructureDeviceAddressKHR(vkGetInstanceProcAddr(instance, "vkGetAccelerationStructureDeviceAddressKHR"));
            vkCmdWriteAccelerationStructuresPropertiesKHR = PFN_vkCmdWriteAccelerationStructuresPropertiesKHR(vkGetInstanceProcAddr(instance, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
            vkGetDeviceAccelerationStructureCompatibilityKHR = PFN_vkGetDeviceAccelerationStructureCompatibilityKHR(vkGetInstanceProcAddr(instance, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
            vkGetAccelerationStructureBuildSizesKHR = PFN_vkGetAccelerationStructureBuildSizesKHR(vkGetInstanceProcAddr(instance, "vkGetAccelerationStructureBuildSizesKHR"));


            //=== VK_EXT_opacity_micromap ===
            vkCreateMicromapEXT = PFN_vkCreateMicromapEXT(vkGetInstanceProcAddr(instance, "vkCreateMicromapEXT"));
            vkDestroyMicromapEXT = PFN_vkDestroyMicromapEXT(vkGetInstanceProcAddr(instance, "vkDestroyMicromapEXT"));
            vkCmdBuildMicromapsEXT = PFN_vkCmdBuildMicromapsEXT(vkGetInstanceProcAddr(instance, "vkCmdBuildMicromapsEXT"));
            vkBuildMicromapsEXT = PFN_vkBuildMicromapsEXT(vkGetInstanceProcAddr(instance, "vkBuildMicromapsEXT"));
            vkCopyMicromapEXT = PFN_vkCopyMicromapEXT(vkGetInstanceProcAddr(instance, "vkCopyMicromapEXT"));
            vkCopyMicromapToMemoryEXT = PFN_vkCopyMicromapToMemoryEXT(vkGetInstanceProcAddr(instance, "vkCopyMicromapToMemoryEXT"));
            vkCopyMemoryToMicromapEXT = PFN_vkCopyMemoryToMicromapEXT(vkGetInstanceProcAddr(instance, "vkCopyMemoryToMicromapEXT"));
            vkWriteMicromapsPropertiesEXT = PFN_vkWriteMicromapsPropertiesEXT(vkGetInstanceProcAddr(instance, "vkWriteMicromapsPropertiesEXT"));
            vkCmdCopyMicromapEXT = PFN_vkCmdCopyMicromapEXT(vkGetInstanceProcAddr(instance, "vkCmdCopyMicromapEXT"));
            vkCmdCopyMicromapToMemoryEXT = PFN_vkCmdCopyMicromapToMemoryEXT(vkGetInstanceProcAddr(instance, "vkCmdCopyMicromapToMemoryEXT"));
            vkCmdCopyMemoryToMicromapEXT = PFN_vkCmdCopyMemoryToMicromapEXT(vkGetInstanceProcAddr(instance, "vkCmdCopyMemoryToMicromapEXT"));
            vkCmdWriteMicromapsPropertiesEXT = PFN_vkCmdWriteMicromapsPropertiesEXT(vkGetInstanceProcAddr(instance, "vkCmdWriteMicromapsPropertiesEXT"));
            vkGetDeviceMicromapCompatibilityEXT = PFN_vkGetDeviceMicromapCompatibilityEXT(vkGetInstanceProcAddr(instance, "vkGetDeviceMicromapCompatibilityEXT"));
            vkGetMicromapBuildSizesEXT = PFN_vkGetMicromapBuildSizesEXT(vkGetInstanceProcAddr(instance, "vkGetMicromapBuildSizesEXT"));
        }
    public:
        //=== VK_VERSION_1_0 ===
        PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = 0;
        //=== VK_KHR_ray_tracing_pipeline ===
        PFN_vkCmdTraceRaysKHR vkCmdTraceRaysKHR = 0;
        PFN_vkCreateRayTracingPipelinesKHR vkCreateRayTracingPipelinesKHR = 0;
        PFN_vkGetRayTracingShaderGroupHandlesKHR vkGetRayTracingShaderGroupHandlesKHR = 0;
        PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = 0;
        PFN_vkCmdTraceRaysIndirectKHR vkCmdTraceRaysIndirectKHR = 0;
        PFN_vkGetRayTracingShaderGroupStackSizeKHR vkGetRayTracingShaderGroupStackSizeKHR = 0;
        PFN_vkCmdSetRayTracingPipelineStackSizeKHR vkCmdSetRayTracingPipelineStackSizeKHR = 0;


        //=== VK_KHR_acceleration_structure ===
        PFN_vkCreateAccelerationStructureKHR vkCreateAccelerationStructureKHR = 0;
        PFN_vkDestroyAccelerationStructureKHR vkDestroyAccelerationStructureKHR = 0;
        PFN_vkCmdBuildAccelerationStructuresKHR vkCmdBuildAccelerationStructuresKHR = 0;
        PFN_vkCmdBuildAccelerationStructuresIndirectKHR vkCmdBuildAccelerationStructuresIndirectKHR = 0;
        PFN_vkBuildAccelerationStructuresKHR vkBuildAccelerationStructuresKHR = 0;
        PFN_vkCopyAccelerationStructureKHR vkCopyAccelerationStructureKHR = 0;
        PFN_vkCopyAccelerationStructureToMemoryKHR vkCopyAccelerationStructureToMemoryKHR = 0;
        PFN_vkCopyMemoryToAccelerationStructureKHR vkCopyMemoryToAccelerationStructureKHR = 0;
        PFN_vkWriteAccelerationStructuresPropertiesKHR vkWriteAccelerationStructuresPropertiesKHR = 0;
        PFN_vkCmdCopyAccelerationStructureKHR vkCmdCopyAccelerationStructureKHR = 0;
        PFN_vkCmdCopyAccelerationStructureToMemoryKHR vkCmdCopyAccelerationStructureToMemoryKHR = 0;
        PFN_vkCmdCopyMemoryToAccelerationStructureKHR vkCmdCopyMemoryToAccelerationStructureKHR = 0;
        PFN_vkGetAccelerationStructureDeviceAddressKHR vkGetAccelerationStructureDeviceAddressKHR = 0;
        PFN_vkCmdWriteAccelerationStructuresPropertiesKHR vkCmdWriteAccelerationStructuresPropertiesKHR = 0;
        PFN_vkGetDeviceAccelerationStructureCompatibilityKHR vkGetDeviceAccelerationStructureCompatibilityKHR = 0;
        PFN_vkGetAccelerationStructureBuildSizesKHR vkGetAccelerationStructureBuildSizesKHR = 0;

        //=== VK_EXT_opacity_micromap ===
        PFN_vkCreateMicromapEXT vkCreateMicromapEXT = 0;
        PFN_vkDestroyMicromapEXT vkDestroyMicromapEXT = 0;
        PFN_vkCmdBuildMicromapsEXT vkCmdBuildMicromapsEXT = 0;
        PFN_vkBuildMicromapsEXT vkBuildMicromapsEXT = 0;
        PFN_vkCopyMicromapEXT vkCopyMicromapEXT = 0;
        PFN_vkCopyMicromapToMemoryEXT vkCopyMicromapToMemoryEXT = 0;
        PFN_vkCopyMemoryToMicromapEXT vkCopyMemoryToMicromapEXT = 0;
        PFN_vkWriteMicromapsPropertiesEXT vkWriteMicromapsPropertiesEXT = 0;
        PFN_vkCmdCopyMicromapEXT vkCmdCopyMicromapEXT = 0;
        PFN_vkCmdCopyMicromapToMemoryEXT vkCmdCopyMicromapToMemoryEXT = 0;
        PFN_vkCmdCopyMemoryToMicromapEXT vkCmdCopyMemoryToMicromapEXT = 0;
        PFN_vkCmdWriteMicromapsPropertiesEXT vkCmdWriteMicromapsPropertiesEXT = 0;
        PFN_vkGetDeviceMicromapCompatibilityEXT vkGetDeviceMicromapCompatibilityEXT = 0;
        PFN_vkGetMicromapBuildSizesEXT vkGetMicromapBuildSizesEXT = 0;




	};

}
