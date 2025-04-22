#pragma once
#if GE_API_VULKAN

#include<vulkan/vulkan_core.h>
#include "BlackPearl/RHI/RHIResources.h"
#include "BlackPearl/RHI/VulkanRHI/VkBuffer.h"
#include "BlackPearl/RHI/VulkanRHI/VkContext.h"
namespace BlackPearl {
#pragma once

	// command buffer with resource tracking
	class TrackedCommandBuffer
	{
	public:

		// the command buffer itself
		VkCommandBuffer cmdBuf;
		VkCommandPool cmdPool;

		std::vector<RefCountPtr<IResource>> referencedResources; // to keep them alive
		std::vector<RefCountPtr<Buffer>> referencedStagingBuffers; // to allow synchronous mapBuffer

		uint64_t recordingID = 0;
		uint64_t submissionID = 0;

#ifdef NVRHI_WITH_RTXMU
		std::vector<uint64_t> rtxmuBuildIds;
		std::vector<uint64_t> rtxmuCompactionIds;
#endif

		explicit TrackedCommandBuffer(const VulkanContext& context)
			: m_Context(context)
		{ }

		~TrackedCommandBuffer();

	private:
		const VulkanContext& m_Context;
	};

	typedef std::shared_ptr<TrackedCommandBuffer> TrackedCommandBufferPtr;





}

#endif
