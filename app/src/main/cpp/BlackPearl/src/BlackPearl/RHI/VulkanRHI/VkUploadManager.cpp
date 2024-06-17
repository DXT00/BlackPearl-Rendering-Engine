#include "pch.h"
#if GE_API_VULKAN

#include "VkUploadManager.h"

namespace BlackPearl {
	std::shared_ptr<BufferChunk> UploadManager::CreateChunk(uint64_t size)
	{
		return std::shared_ptr<BufferChunk>();
	}

	bool UploadManager::suballocateBuffer(uint64_t size, Buffer** pBuffer, uint64_t* pOffset, void** pCpuVA, uint64_t currentVersion, uint32_t alignment)
	{
		return false;
	}

	void UploadManager::submitChunks(uint64_t currentVersion, uint64_t submittedVersion)
	{
	}

}
#endif