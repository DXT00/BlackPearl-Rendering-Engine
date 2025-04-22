#pragma once
#if GE_API_VULKAN

#include<vulkan/vulkan_core.h>
namespace BlackPearl {
    class MemoryResource
    {
    public:
        bool managed = true;
        VkDeviceMemory memory;
    };
}
#endif