#pragma once
#if GE_API_VULKAN

#include<vulkan/vulkan.h>
namespace BlackPearl {
    class MemoryResource
    {
    public:
        bool managed = true;
        VkDeviceMemory memory;
    };
}
#endif