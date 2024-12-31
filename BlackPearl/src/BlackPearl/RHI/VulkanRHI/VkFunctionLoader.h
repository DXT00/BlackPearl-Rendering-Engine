#pragma once
#include "vulkan/vulkan_core.h"
namespace BlackPearl {

	class VkFunctionLoader
	{
    public:
        VkFunctionLoader(PFN_vkGetDeviceProcAddr getProcAddr, VkDevice device) : vkGetDeviceProcAddr(getProcAddr)
        {
            vkGetDeviceProcAddr = PFN_vkGetDeviceProcAddr(vkGetDeviceProcAddr(device, "vkGetDeviceProcAddr"));

            //=== VK_KHR_ray_tracing_pipeline ===
            vkCmdTraceRaysKHR = PFN_vkCmdTraceRaysKHR(vkGetDeviceProcAddr(device, "vkCmdTraceRaysKHR"));
            vkCreateRayTracingPipelinesKHR = PFN_vkCreateRayTracingPipelinesKHR(vkGetDeviceProcAddr(device, "vkCreateRayTracingPipelinesKHR"));
            vkGetRayTracingShaderGroupHandlesKHR = PFN_vkGetRayTracingShaderGroupHandlesKHR(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupHandlesKHR"));
            vkGetRayTracingCaptureReplayShaderGroupHandlesKHR = PFN_vkGetRayTracingCaptureReplayShaderGroupHandlesKHR(vkGetDeviceProcAddr(device, "vkGetRayTracingCaptureReplayShaderGroupHandlesKHR"));
            vkCmdTraceRaysIndirectKHR = PFN_vkCmdTraceRaysIndirectKHR(vkGetDeviceProcAddr(device, "vkCmdTraceRaysIndirectKHR"));
            vkGetRayTracingShaderGroupStackSizeKHR = PFN_vkGetRayTracingShaderGroupStackSizeKHR(vkGetDeviceProcAddr(device, "vkGetRayTracingShaderGroupStackSizeKHR"));
            vkCmdSetRayTracingPipelineStackSizeKHR = PFN_vkCmdSetRayTracingPipelineStackSizeKHR(vkGetDeviceProcAddr(device, "vkCmdSetRayTracingPipelineStackSizeKHR"));
        

            //=== VK_KHR_acceleration_structure ===
            vkCreateAccelerationStructureKHR = PFN_vkCreateAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkCreateAccelerationStructureKHR"));
            vkDestroyAccelerationStructureKHR = PFN_vkDestroyAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkDestroyAccelerationStructureKHR"));
            vkCmdBuildAccelerationStructuresKHR = PFN_vkCmdBuildAccelerationStructuresKHR(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresKHR"));
            vkCmdBuildAccelerationStructuresIndirectKHR = PFN_vkCmdBuildAccelerationStructuresIndirectKHR(vkGetDeviceProcAddr(device, "vkCmdBuildAccelerationStructuresIndirectKHR"));
            vkBuildAccelerationStructuresKHR = PFN_vkBuildAccelerationStructuresKHR(vkGetDeviceProcAddr(device, "vkBuildAccelerationStructuresKHR"));
            vkCopyAccelerationStructureKHR = PFN_vkCopyAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureKHR"));
            vkCopyAccelerationStructureToMemoryKHR = PFN_vkCopyAccelerationStructureToMemoryKHR(vkGetDeviceProcAddr(device, "vkCopyAccelerationStructureToMemoryKHR"));
            vkCopyMemoryToAccelerationStructureKHR = PFN_vkCopyMemoryToAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkCopyMemoryToAccelerationStructureKHR"));
            vkWriteAccelerationStructuresPropertiesKHR = PFN_vkWriteAccelerationStructuresPropertiesKHR(vkGetDeviceProcAddr(device, "vkWriteAccelerationStructuresPropertiesKHR"));
            vkCmdCopyAccelerationStructureKHR = PFN_vkCmdCopyAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureKHR"));
            vkCmdCopyAccelerationStructureToMemoryKHR = PFN_vkCmdCopyAccelerationStructureToMemoryKHR(vkGetDeviceProcAddr(device, "vkCmdCopyAccelerationStructureToMemoryKHR"));
            vkCmdCopyMemoryToAccelerationStructureKHR = PFN_vkCmdCopyMemoryToAccelerationStructureKHR(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToAccelerationStructureKHR"));
            vkGetAccelerationStructureDeviceAddressKHR = PFN_vkGetAccelerationStructureDeviceAddressKHR(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureDeviceAddressKHR"));
            vkCmdWriteAccelerationStructuresPropertiesKHR = PFN_vkCmdWriteAccelerationStructuresPropertiesKHR(vkGetDeviceProcAddr(device, "vkCmdWriteAccelerationStructuresPropertiesKHR"));
            vkGetDeviceAccelerationStructureCompatibilityKHR = PFN_vkGetDeviceAccelerationStructureCompatibilityKHR(vkGetDeviceProcAddr(device, "vkGetDeviceAccelerationStructureCompatibilityKHR"));
            vkGetAccelerationStructureBuildSizesKHR = PFN_vkGetAccelerationStructureBuildSizesKHR(vkGetDeviceProcAddr(device, "vkGetAccelerationStructureBuildSizesKHR"));


            //=== VK_EXT_opacity_micromap ===
            vkCreateMicromapEXT = PFN_vkCreateMicromapEXT(vkGetDeviceProcAddr(device, "vkCreateMicromapEXT"));
            vkDestroyMicromapEXT = PFN_vkDestroyMicromapEXT(vkGetDeviceProcAddr(device, "vkDestroyMicromapEXT"));
            vkCmdBuildMicromapsEXT = PFN_vkCmdBuildMicromapsEXT(vkGetDeviceProcAddr(device, "vkCmdBuildMicromapsEXT"));
            vkBuildMicromapsEXT = PFN_vkBuildMicromapsEXT(vkGetDeviceProcAddr(device, "vkBuildMicromapsEXT"));
            vkCopyMicromapEXT = PFN_vkCopyMicromapEXT(vkGetDeviceProcAddr(device, "vkCopyMicromapEXT"));
            vkCopyMicromapToMemoryEXT = PFN_vkCopyMicromapToMemoryEXT(vkGetDeviceProcAddr(device, "vkCopyMicromapToMemoryEXT"));
            vkCopyMemoryToMicromapEXT = PFN_vkCopyMemoryToMicromapEXT(vkGetDeviceProcAddr(device, "vkCopyMemoryToMicromapEXT"));
            vkWriteMicromapsPropertiesEXT = PFN_vkWriteMicromapsPropertiesEXT(vkGetDeviceProcAddr(device, "vkWriteMicromapsPropertiesEXT"));
            vkCmdCopyMicromapEXT = PFN_vkCmdCopyMicromapEXT(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapEXT"));
            vkCmdCopyMicromapToMemoryEXT = PFN_vkCmdCopyMicromapToMemoryEXT(vkGetDeviceProcAddr(device, "vkCmdCopyMicromapToMemoryEXT"));
            vkCmdCopyMemoryToMicromapEXT = PFN_vkCmdCopyMemoryToMicromapEXT(vkGetDeviceProcAddr(device, "vkCmdCopyMemoryToMicromapEXT"));
            vkCmdWriteMicromapsPropertiesEXT = PFN_vkCmdWriteMicromapsPropertiesEXT(vkGetDeviceProcAddr(device, "vkCmdWriteMicromapsPropertiesEXT"));
            vkGetDeviceMicromapCompatibilityEXT = PFN_vkGetDeviceMicromapCompatibilityEXT(vkGetDeviceProcAddr(device, "vkGetDeviceMicromapCompatibilityEXT"));
            vkGetMicromapBuildSizesEXT = PFN_vkGetMicromapBuildSizesEXT(vkGetDeviceProcAddr(device, "vkGetMicromapBuildSizesEXT"));
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
