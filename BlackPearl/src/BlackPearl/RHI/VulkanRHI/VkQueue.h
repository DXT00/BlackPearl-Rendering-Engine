//#include "BlackPearl/RHI/RHI.h"
#pragma once
#if GE_API_VULKAN

#include "BlackPearl/RHI/RHICommandList.h"
#include "VkContext.h"
#include "VkCommandBuffer.h"
#include <vector>
#include <mutex>
#include <list>
namespace BlackPearl {

    class Queue
    {
    public:
        VkSemaphore trackingSemaphore;

        Queue(const VulkanContext& context, CommandQueue queueID, VkQueue queue, uint32_t queueFamilyIndex);
        ~Queue();

        // creates a command buffer and its synchronization resources
        TrackedCommandBufferPtr createCommandBuffer();

        TrackedCommandBufferPtr getOrCreateCommandBuffer();

        void addWaitSemaphore(VkSemaphore semaphore, uint64_t value);
        void addSignalSemaphore(VkSemaphore semaphore, uint64_t value);

        // submits a command buffer to this queue, returns submissionID
        uint64_t submit(ICommandList* const* ppCmd, size_t numCmd);

        // retire any command buffers that have finished execution from the pending execution list
        void retireCommandBuffers();

        TrackedCommandBufferPtr getCommandBufferInFlight(uint64_t submissionID);

        uint64_t updateLastFinishedID();
        uint64_t getLastSubmittedID() const { return m_LastSubmittedID; }
        uint64_t getLastFinishedID() const { return m_LastFinishedID; }
        CommandQueue getQueueID() const { return m_QueueID; }
        VkQueue getVkQueue() const { return m_Queue; }

        bool pollCommandList(uint64_t commandListID);
        bool waitCommandList(uint64_t commandListID, uint64_t timeout);

    private:
        const VulkanContext& m_Context;

        VkQueue m_Queue;
        CommandQueue m_QueueID;
        uint32_t m_QueueFamilyIndex = uint32_t(-1);

        std::mutex m_Mutex;
        std::vector<VkSemaphore> m_WaitSemaphores;
        std::vector<uint64_t> m_WaitSemaphoreValues;
        std::vector<VkSemaphore> m_SignalSemaphores;
        std::vector<uint64_t> m_SignalSemaphoreValues;

        uint64_t m_LastRecordingID = 0;
        uint64_t m_LastSubmittedID = 0;
        uint64_t m_LastFinishedID = 0;

        // tracks the list of command buffers in flight on this queue
        std::list<TrackedCommandBufferPtr> m_CommandBuffersInFlight;
        std::list<TrackedCommandBufferPtr> m_CommandBuffersPool;
    };
}
#endif