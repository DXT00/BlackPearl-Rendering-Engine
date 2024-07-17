#include "pch.h"
#if GE_API_VULKAN

#include "VkUploadManager.h"
#include "VkBuffer.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/Common/CommonFunc.h"

namespace BlackPearl {
	std::shared_ptr<BufferChunk> UploadManager::CreateChunk(uint64_t size)
	{
        std::shared_ptr<BufferChunk> chunk = std::make_shared<BufferChunk>();

        if (m_IsScratchBuffer)
        {
            BufferDesc desc;
            desc.byteSize = size;
            desc.cpuAccess = CpuAccessMode::None;
            desc.debugName = "ScratchBufferChunk";
            desc.canHaveUAVs = true;

            chunk->buffer = m_Device->createBuffer(desc);
            chunk->mappedMemory = nullptr;
            chunk->bufferSize = size;
        }
        else
        {
            BufferDesc desc;
            desc.byteSize = size;
            desc.cpuAccess = CpuAccessMode::Write;
            desc.debugName = "UploadChunk";

            // The upload manager buffers are used in buildTopLevelAccelStruct to store instance data
            desc.isAccelStructBuildInput = m_Device->queryFeatureSupport(Feature::RayTracingAccelStruct);

            chunk->buffer = m_Device->createBuffer(desc);
            chunk->mappedMemory = m_Device->mapBuffer(chunk->buffer, CpuAccessMode::Write);
            chunk->bufferSize = size;
        }

        return chunk;
	}

	bool UploadManager::suballocateBuffer(uint64_t size, Buffer** pBuffer, uint64_t* pOffset, void** pCpuVA, uint64_t currentVersion, uint32_t alignment)
	{
        std::shared_ptr<BufferChunk> chunkToRetire;

        if (m_CurrentChunk)
        {
            uint64_t alignedOffset = Align(m_CurrentChunk->writePointer, (uint64_t)alignment);
            uint64_t endOfDataInChunk = alignedOffset + size;

            if (endOfDataInChunk <= m_CurrentChunk->bufferSize)
            {
                m_CurrentChunk->writePointer = endOfDataInChunk;

                *pBuffer = static_cast<Buffer*>(m_CurrentChunk->buffer.Get());
                *pOffset = alignedOffset;
                if (pCpuVA && m_CurrentChunk->mappedMemory)
                    *pCpuVA = (char*)m_CurrentChunk->mappedMemory + alignedOffset;

                return true;
            }

            chunkToRetire = m_CurrentChunk;
            m_CurrentChunk.reset();
        }

        CommandQueue queue = VersionGetQueue(currentVersion);
        uint64_t completedInstance = m_Device->queueGetCompletedInstance(queue);

        for (auto it = m_ChunkPool.begin(); it != m_ChunkPool.end(); ++it)
        {
            std::shared_ptr<BufferChunk> chunk = *it;

            if (VersionGetSubmitted(chunk->version)
                && VersionGetInstance(chunk->version) <= completedInstance)
            {
                chunk->version = 0;
            }

            if (chunk->version == 0 && chunk->bufferSize >= size)
            {
                m_ChunkPool.erase(it);
                m_CurrentChunk = chunk;
                break;
            }
        }

        if (chunkToRetire)
        {
            m_ChunkPool.push_back(chunkToRetire);
        }

        if (!m_CurrentChunk)
        {
            uint64_t sizeToAllocate = Align(std::max(size, m_DefaultChunkSize), BufferChunk::c_sizeAlignment);

            if ((m_MemoryLimit > 0) && (m_AllocatedMemory + sizeToAllocate > m_MemoryLimit))
                return false;

            m_CurrentChunk = CreateChunk(sizeToAllocate);
        }

        m_CurrentChunk->version = currentVersion;
        m_CurrentChunk->writePointer = size;

        *pBuffer = static_cast<Buffer*>(m_CurrentChunk->buffer.Get());
        *pOffset = 0;
        if (pCpuVA)
            *pCpuVA = m_CurrentChunk->mappedMemory;

        return true;
	}

	void UploadManager::submitChunks(uint64_t currentVersion, uint64_t submittedVersion)
	{
		if (m_CurrentChunk)
		{
			m_ChunkPool.push_back(m_CurrentChunk);
			m_CurrentChunk.reset();
		}

		for (const auto& chunk : m_ChunkPool)
		{
			if (chunk->version == currentVersion)
				chunk->version = submittedVersion;
		}
	}

}
#endif