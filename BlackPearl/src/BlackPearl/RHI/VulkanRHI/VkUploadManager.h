#pragma once
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/VulkanRHI/VkDevice.h"
namespace BlackPearl{


    struct BufferChunk
    {
        BufferHandle buffer;
        uint64_t version = 0;
        uint64_t bufferSize = 0;
        uint64_t writePointer = 0;
        void* mappedMemory = nullptr;

        static constexpr uint64_t c_sizeAlignment = 4096; // GPU page size
    };

    class UploadManager
    {
    public:
        UploadManager(Device* pParent, uint64_t defaultChunkSize, uint64_t memoryLimit, bool isScratchBuffer)
            : m_Device(pParent)
            , m_DefaultChunkSize(defaultChunkSize)
            , m_MemoryLimit(memoryLimit)
            , m_IsScratchBuffer(isScratchBuffer)
        { }

        std::shared_ptr<BufferChunk> CreateChunk(uint64_t size);

        bool suballocateBuffer(uint64_t size, Buffer** pBuffer, uint64_t* pOffset, void** pCpuVA, uint64_t currentVersion, uint32_t alignment = 256);
        void submitChunks(uint64_t currentVersion, uint64_t submittedVersion);

    private:
        Device* m_Device;
        uint64_t m_DefaultChunkSize = 0;
        uint64_t m_MemoryLimit = 0;
        uint64_t m_AllocatedMemory = 0;
        bool m_IsScratchBuffer = false;

        std::list<std::shared_ptr<BufferChunk>> m_ChunkPool;
        std::shared_ptr<BufferChunk> m_CurrentChunk;
    };

}
