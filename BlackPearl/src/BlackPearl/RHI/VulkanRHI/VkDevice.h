#pragma once
#include "../RefCountPtr.h"
#include "../RHIDevice.h"
#include "../RHIPipeline.h"
#include "../RHIFrameBuffer.h"
#include "VkContext.h"
namespace BlackPearl {

    struct DeviceDesc
    {
        //IMessageCallback* errorCB = nullptr;

        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        // any of the queues can be null if this context doesn't intend to use them
        VkQueue graphicsQueue;
        int graphicsQueueIndex = -1;
        VkQueue transferQueue;
        int transferQueueIndex = -1;
        VkQueue computeQueue;
        int computeQueueIndex = -1;

        VkAllocationCallbacks* allocationCallbacks = nullptr;

        const char** instanceExtensions = nullptr;
        size_t numInstanceExtensions = 0;

        const char** deviceExtensions = nullptr;
        size_t numDeviceExtensions = 0;

        uint32_t maxTimerQueries = 256;

        // Indicates if VkPhysicalDeviceVulkan12Features::bufferDeviceAddress was set to 'true' at device creation time
        bool bufferDeviceAddressSupported = false;
    };

    class Device : public RefCounter<IDevice>
    {
    public:
        // Internal backend methods

        Device(const DeviceDesc& desc);
        ~Device() override;


        FramebufferHandle createFramebuffer(const FramebufferDesc& desc) override;

        GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;

        ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) override;

        MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) override;

        

    private:
        //VulkanContext m_Context;
    //    VulkanAllocator m_Allocator;

    //    vk::QueryPool m_TimerQueryPool = nullptr;
    //    utils::BitSetAllocator m_TimerQueryAllocator;

    //    std::mutex m_Mutex;

    //    // array of submission queues
    //    std::array<std::unique_ptr<Queue>, uint32_t(CommandQueue::Count)> m_Queues;

    //    void* mapBuffer(IBuffer* b, CpuAccessMode flags, uint64_t offset, size_t size) const;
    //
    };


}