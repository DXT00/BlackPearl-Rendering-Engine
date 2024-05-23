#pragma once
#include<vulkan/vulkan.h>
namespace BlackPearl {
    class MemoryResource
    {
    public:
        bool managed = true;
        VkDeviceMemory memory;
    };
}
