#pragma once
#include <atomic>
#include "VkMemoryResource.h"
#include "BlackPearl/RHI/RHIHeap.h"
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIState.h"
#include "VkAllocator.h"
namespace BlackPearl {
    // A copyable version of std::atomic to be used in an std::vector
    class BufferVersionItem : public std::atomic<uint64_t>  // NOLINT(cppcoreguidelines-special-member-functions)
    {
    public:
        BufferVersionItem()
            : std::atomic<uint64_t>()
        { }

        BufferVersionItem(const BufferVersionItem& other)
        {
            store(other);
        }

        BufferVersionItem& operator=(const uint64_t a)
        {
            store(a);
            return *this;
        }
    };

    class Buffer : public MemoryResource, public RefCounter<IBuffer>, public BufferStateExtension
    {
    public:
        BufferDesc desc;

        VkBuffer buffer;
        VkDeviceAddress deviceAddress = 0;

        HeapHandle heap;

        std::unordered_map<uint64_t, VkBufferView> viewCache;

        std::vector<BufferVersionItem> versionTracking;
        void* mappedMemory = nullptr;
        void* sharedHandle = nullptr;
        uint32_t versionSearchStart = 0;

        // For staging buffers only
        CommandQueue lastUseQueue = CommandQueue::Graphics;
        uint64_t lastUseCommandListID = 0;

        Buffer(const VulkanContext& context, VulkanAllocator& allocator)
            : BufferStateExtension(desc)
            , m_Context(context)
            , m_Allocator(allocator)
        { }

        ~Buffer() override;
        const BufferDesc& getDesc() const override { return desc; }
       // Object getNativeObject(ObjectType type) override;

    private:
        const VulkanContext& m_Context;
        VulkanAllocator& m_Allocator;
    };




}
