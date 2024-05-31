#pragma once
#if GE_API_VULKAN

#include "VkContext.h"


namespace BlackPearl {
    class Buffer;
    class MemoryResource;
    class ETexture;

	class VulkanAllocator
    {
    public:

        VulkanAllocator(const VulkanContext& context)
            : m_Context(context){ }

        VkResult allocateBufferMemory(Buffer* buffer, bool enableBufferAddress = false) ;
        void freeBufferMemory(Buffer* buffer) ;

        VkResult allocateTextureMemory(ETexture* texture) ;
        void freeTextureMemory(const ETexture* texture) ;

        VkResult allocateMemory(MemoryResource* res,
            VkMemoryRequirements memRequirements,
            VkMemoryPropertyFlags memPropertyFlags,
            bool enableDeviceAddress = false,
            bool enableExportMemory = false,
            VkImage dedicatedImage = nullptr,
            VkBuffer dedicatedBuffer = nullptr) ;
        void freeMemory(MemoryResource* res) ;

    private:
        const VulkanContext& m_Context;

	};

}
#endif