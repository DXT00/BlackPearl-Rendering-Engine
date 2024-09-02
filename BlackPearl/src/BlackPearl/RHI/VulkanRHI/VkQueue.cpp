/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
#include "pch.h"
#if GE_API_VULKAN

#include "VkQueue.h"
#include "VkCommandList.h"
namespace BlackPearl {



	bool Queue::waitCommandList(uint64_t commandListID, uint64_t timeout)
	{
		if (commandListID > m_LastSubmittedID || commandListID == 0)
			return false;

		if (pollCommandList(commandListID))
			return true;

		std::array<const VkSemaphore, 1> semaphores = { trackingSemaphore };
		std::array<uint64_t, 1> waitValues = { commandListID };

		VkSemaphoreWaitInfo waitInfo{};
		waitInfo.pSemaphores = semaphores.data();
		waitInfo.pValues = waitValues.data();
		waitInfo.pNext = nullptr;
		/*	.setSemaphores(semaphores)
			.setValues(waitValues);*/
		VkResult result =vkWaitSemaphores(m_Context.device ,&waitInfo, timeout);
		//vk::Result result = m_Context.device.waitSemaphores(waitInfo, timeout);

		return (result == VkResult::VK_SUCCESS);
	}

	Queue::Queue(const VulkanContext& context, CommandQueue queueID, VkQueue queue, uint32_t queueFamilyIndex)
		: m_Context(context)
		, m_Queue(queue)
		, m_QueueID(queueID)
		, m_QueueFamilyIndex(queueFamilyIndex) {

		VkSemaphoreTypeCreateInfo semaphoreTypeInfo{};
		semaphoreTypeInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		semaphoreTypeInfo.pNext = nullptr;
		semaphoreTypeInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;

			//.setSemaphoreType(vk::SemaphoreType::eTimeline);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreInfo.pNext = &semaphoreTypeInfo;

		vkCreateSemaphore(context.device, &semaphoreInfo, context.allocationCallbacks, &trackingSemaphore);


		//trackingSemaphore = context.device.createSemaphore(semaphoreInfo, context.allocationCallbacks);
	}

	Queue::~Queue()
	{
		vkDestroySemaphore(m_Context.device, trackingSemaphore, m_Context.allocationCallbacks);
		
		//m_Context.device.destroySemaphore(trackingSemaphore, m_Context.allocationCallbacks);
		//trackingSemaphore = VkSemaphore{};
	}
	uint64_t Queue::updateLastFinishedID()
	{
		//m_LastFinishedID = m_Context.device.getSemaphoreCounterValue(trackingSemaphore);
		if (vkGetSemaphoreCounterValue(m_Context.device, trackingSemaphore, &m_LastFinishedID) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
		return m_LastFinishedID;
	}

	bool Queue::pollCommandList(uint64_t commandListID)
	{
		if (commandListID > m_LastSubmittedID || commandListID == 0)
			return false;

		bool completed = getLastFinishedID() >= commandListID;
		if (completed)
			return true;

		completed = updateLastFinishedID() >= commandListID;
		return completed;
	}

	TrackedCommandBufferPtr Queue::createCommandBuffer()
	{
		VkResult res;

		TrackedCommandBufferPtr ret = std::make_shared<TrackedCommandBuffer>(m_Context);

		VkCommandPoolCreateInfo cmdPoolInfo{};
		cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolInfo.queueFamilyIndex = m_QueueFamilyIndex;
		cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;


		if (vkCreateCommandPool(m_Context.device, &cmdPoolInfo, nullptr, &ret->cmdPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create command pool!");
		}
		
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.commandBufferCount = 1;
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = ret->cmdPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		if (vkAllocateCommandBuffers(m_Context.device, &allocInfo, &ret->cmdBuf) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		return ret;
	}

	TrackedCommandBufferPtr Queue::getOrCreateCommandBuffer()
	{
		std::lock_guard lockGuard(m_Mutex); // this is called from CommandList::open, so free-threaded

		uint64_t recordingID = ++m_LastRecordingID;

		TrackedCommandBufferPtr cmdBuf;
		if (m_CommandBuffersPool.empty())
		{
			cmdBuf = createCommandBuffer();
		}
		else
		{
			cmdBuf = m_CommandBuffersPool.front();
			m_CommandBuffersPool.pop_front();
		}

		cmdBuf->recordingID = recordingID;
		return cmdBuf;
	}

	void Queue::addWaitSemaphore(VkSemaphore semaphore, uint64_t value)
	{
		if (!semaphore)
			return;

		m_WaitSemaphores.push_back(semaphore);
		m_WaitSemaphoreValues.push_back(value);
	}

	void Queue::addSignalSemaphore(VkSemaphore semaphore, uint64_t value)
	{
		if (!semaphore)
			return;

		m_SignalSemaphores.push_back(semaphore);
		m_SignalSemaphoreValues.push_back(value);
	}

	uint64_t Queue::submit(ICommandList* const* ppCmd, size_t numCmd)
	{
		std::vector<VkPipelineStageFlags> waitStageArray(m_WaitSemaphores.size());
		std::vector<VkCommandBuffer> commandBuffers(numCmd);

		for (size_t i = 0; i < m_WaitSemaphores.size(); i++)
		{
			waitStageArray[i] = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		}

		m_LastSubmittedID++;

		for (size_t i = 0; i < numCmd; i++)
		{
			CommandList* commandList = dynamic_cast<CommandList*>(ppCmd[i]);
			TrackedCommandBufferPtr commandBuffer = commandList->getCurrentCmdBuf();

			commandBuffers[i] = commandBuffer->cmdBuf;
			m_CommandBuffersInFlight.push_back(commandBuffer);

			for (const auto& buffer : commandBuffer->referencedStagingBuffers)
			{
				buffer->lastUseQueue = m_QueueID;
				buffer->lastUseCommandListID = m_LastSubmittedID;
			}
		}

		m_SignalSemaphores.push_back(trackingSemaphore);
		m_SignalSemaphoreValues.push_back(m_LastSubmittedID);

		VkTimelineSemaphoreSubmitInfo timelineSemaphoreInfo{};
		timelineSemaphoreInfo.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
		timelineSemaphoreInfo.signalSemaphoreValueCount = uint32_t(m_SignalSemaphoreValues.size());
		timelineSemaphoreInfo.pSignalSemaphoreValues = m_SignalSemaphoreValues.data();


		if (!m_WaitSemaphoreValues.empty())
		{
			timelineSemaphoreInfo.waitSemaphoreValueCount = uint32_t(m_WaitSemaphoreValues.size());
			timelineSemaphoreInfo.pWaitSemaphoreValues = m_WaitSemaphoreValues.data();
		}
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = &timelineSemaphoreInfo;
		submitInfo.commandBufferCount = uint32_t(numCmd);
		submitInfo.pCommandBuffers = commandBuffers.data();
		submitInfo.waitSemaphoreCount = uint32_t(m_WaitSemaphores.size());
		submitInfo.pWaitSemaphores = m_WaitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStageArray.data();
		submitInfo.signalSemaphoreCount = uint32_t(m_SignalSemaphores.size());
		submitInfo.pSignalSemaphores = m_SignalSemaphores.data();

	  /*	auto submitInfo = vk::SubmitInfo()
			.setPNext(&timelineSemaphoreInfo)
			.setCommandBufferCount(uint32_t(numCmd))
			.setPCommandBuffers(commandBuffers.data())
			.setWaitSemaphoreCount(uint32_t(m_WaitSemaphores.size()))
			.setPWaitSemaphores(m_WaitSemaphores.data())
			.setPWaitDstStageMask(waitStageArray.data())
			.setSignalSemaphoreCount(uint32_t(m_SignalSemaphores.size()))
			.setPSignalSemaphores(m_SignalSemaphores.data());*/

		//m_Queue.submit(submitInfo);
		vkQueueSubmit(m_Queue, 1, &submitInfo, VK_NULL_HANDLE);

		m_WaitSemaphores.clear();
		m_WaitSemaphoreValues.clear();
		m_SignalSemaphores.clear();
		m_SignalSemaphoreValues.clear();

		return m_LastSubmittedID;
	}

	void Queue::retireCommandBuffers()
	{
		std::list<TrackedCommandBufferPtr> submissions = std::move(m_CommandBuffersInFlight);

		uint64_t lastFinishedID = updateLastFinishedID();

		for (const TrackedCommandBufferPtr& cmd : submissions)
		{
			if (cmd->submissionID <= lastFinishedID)
			{
				cmd->referencedResources.clear();
				cmd->referencedStagingBuffers.clear();
				cmd->submissionID = 0;
				m_CommandBuffersPool.push_back(cmd);

#ifdef NVRHI_WITH_RTXMU
				if (!cmd->rtxmuBuildIds.empty())
				{
					std::lock_guard lockGuard(m_Context.rtxMuResources->asListMutex);

					m_Context.rtxMuResources->asBuildsCompleted.insert(m_Context.rtxMuResources->asBuildsCompleted.end(),
						cmd->rtxmuBuildIds.begin(), cmd->rtxmuBuildIds.end());

					cmd->rtxmuBuildIds.clear();
				}
				if (!cmd->rtxmuCompactionIds.empty())
				{
					m_Context.rtxMemUtil->GarbageCollection(cmd->rtxmuCompactionIds);
					cmd->rtxmuCompactionIds.clear();
				}
#endif
			}
			else
			{
				m_CommandBuffersInFlight.push_back(cmd);
			}
		}
	}

	TrackedCommandBufferPtr Queue::getCommandBufferInFlight(uint64_t submissionID)
	{
		for (const TrackedCommandBufferPtr& cmd : m_CommandBuffersInFlight)
		{
			if (cmd->submissionID == submissionID)
				return cmd;
		}

		return nullptr;
	}

}
#endif