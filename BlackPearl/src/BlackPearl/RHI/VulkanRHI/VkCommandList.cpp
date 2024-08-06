#include "pch.h"
#if GE_API_VULKAN

#include "BlackPearl/Core.h"
#include "../Common/Misc.h"
#include "VkCommandList.h"
#include "VkUtil.h"
#include "VkPipeline.h"
#include "VkFrameBuffer.h"
#include "VkBindingSet.h"
#include "VkDescriptorTable.h"
#include "VkTexture.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"
#include "BlackPearl/RHI/Common/FormatInfo.h"
namespace BlackPearl {

	static VkViewport VKViewportWithDXCoords(const RHIViewport& v)
	{
		// requires VK_KHR_maintenance1 which allows negative-height to indicate an inverted coord space to match DX
		return VkViewport{ v.minX, v.maxY, v.maxX - v.minX, -(v.maxY - v.minY), v.minZ, v.maxZ };
	}

	static uint64_t getQueueLastFinishedID(Device* device, CommandQueue queueIndex)
	{
		Queue* queue = device->getQueue(queueIndex);
		if (queue)
			return queue->getLastFinishedID();
		return 0;
	}


	CommandList::CommandList(Device* device, const VulkanContext& context, const CommandListParameters& parameters)
		: m_Device(device)
		, m_Context(context)
		, m_CommandListParameters(parameters)
		, m_StateTracker(context.messageCallback)
		, m_UploadManager(std::make_unique<UploadManager>(device, parameters.uploadChunkSize, 0, false))
		, m_ScratchManager(std::make_unique<UploadManager>(device, parameters.scratchChunkSize, parameters.scratchMaxMemory, true))
	{


	}
	void CommandList::executed(Queue& queue, uint64_t submissionID)
	{
		assert(m_CurrentCmdBuf);

		m_CurrentCmdBuf->submissionID = submissionID;

		const CommandQueue queueID = queue.getQueueID();
		const uint64_t recordingID = m_CurrentCmdBuf->recordingID;

		m_CurrentCmdBuf = nullptr;

		_submitVolatileBuffers(recordingID, submissionID);

		m_StateTracker.commandListSubmitted();

		m_UploadManager->submitChunks(
			MakeVersion(recordingID, queueID, false),
			MakeVersion(submissionID, queueID, true));

		m_ScratchManager->submitChunks(
			MakeVersion(recordingID, queueID, false),
			MakeVersion(submissionID, queueID, true));

		m_VolatileBufferStates.clear();
	}
	void CommandList::open()
	{
		m_CurrentCmdBuf = m_Device->getQueue(m_CommandListParameters.queueType)->getOrCreateCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pNext = nullptr;
		vkBeginCommandBuffer(m_CurrentCmdBuf->cmdBuf, &beginInfo);
		m_CurrentCmdBuf->referencedResources.push_back(this); // prevent deletion of e.g. UploadManager

		clearState();

	}
	void CommandList::close()
	{
		_endRenderPass();

		m_StateTracker.keepBufferInitialStates();
		m_StateTracker.keepTextureInitialStates();
		commitBarriers();

#ifdef NVRHI_WITH_RTXMU
		if (!m_CurrentCmdBuf->rtxmuBuildIds.empty())
		{
			m_Context.rtxMemUtil->PopulateCompactionSizeCopiesCommandList(m_CurrentCmdBuf->cmdBuf, m_CurrentCmdBuf->rtxmuBuildIds);
		}
#endif
		vkEndCommandBuffer(m_CurrentCmdBuf->cmdBuf);

		clearState();

		_flushVolatileBufferWrites();
	}
	void CommandList::clearState()
	{
		_endRenderPass();

		m_CurrentPipelineLayout = {};
		m_CurrentPushConstantsVisibility = {};

		m_CurrentGraphicsState = {};
		m_CurrentComputeState = {};
		m_CurrentMeshletState = {};
		//m_CurrentRayTracingState = MeshletState();
		m_CurrentShaderTablePointers = ShaderTableState();

		m_AnyVolatileBufferWrites = false;

	}

	
	void CommandList::clearTextureFloat(ITexture* texture, TextureSubresourceSet subresources, const Color& clearColor)
	{
		assert(0);

	}
	void CommandList::clearDepthStencilTexture(ITexture* texture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil)
	{
		assert(0);

	}
	void CommandList::clearTextureUInt(ITexture* texture, TextureSubresourceSet subresources, uint32_t clearColor)
	{
		assert(0);

	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice)
	{
		assert(0);

	}
	void CommandList::copyTexture(IStagingTexture* dest, const TextureSlice& dstSlice, ITexture* src, const TextureSlice& srcSlice)
	{
		assert(0);

	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& dstSlice, IStagingTexture* src, const TextureSlice& srcSlice)
	{
		assert(0);

	}
	void CommandList::writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch)
	{
		assert(0);
	}
	void CommandList::resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources)
	{
		assert(0);

	}
	void CommandList::writeBuffer(IBuffer* _buffer, const void* data, size_t dataSize, uint64_t destOffsetBytes)
	{
		Buffer* buffer = dynamic_cast<Buffer*>(_buffer);

		GE_ASSERT(dataSize <= buffer->desc.byteSize, "buffer dataSize invalid");

		assert(m_CurrentCmdBuf);

		_endRenderPass();

		m_CurrentCmdBuf->referencedResources.push_back(buffer);

		if (buffer->desc.isVolatile)
		{
			assert(destOffsetBytes == 0);

			_writeVolatileBuffer(buffer, data, dataSize);

			return;
		}

		const size_t commandBufferWriteLimit = 65536;

		if (dataSize <= commandBufferWriteLimit)
		{
			if (m_EnableAutomaticBarriers)
			{
				_requireBufferState(buffer, ResourceStates::CopyDest);
			}
			commitBarriers();

			int64_t remaining = dataSize;
			const char* base = (const char*)data;
			while (remaining > 0)
			{
				// vulkan allows <= 64kb transfers via VkCmdUpdateBuffer
				int64_t thisGulpSize = std::min(remaining, int64_t(commandBufferWriteLimit));

				// we bloat the read size here past the incoming buffer since the transfer must be a multiple of 4; the extra garbage should never be used anywhere
				thisGulpSize = (thisGulpSize + 3) & ~3ll;
				
				//m_CurrentCmdBuf->cmdBuf.updateBuffer(buffer->buffer, destOffsetBytes + dataSize - remaining, thisGulpSize, &base[dataSize - remaining]);
				//Update a buffer's contents from host memory
				vkCmdUpdateBuffer(m_CurrentCmdBuf->cmdBuf, buffer->buffer, destOffsetBytes + dataSize - remaining, thisGulpSize, &base[dataSize - remaining]);
				remaining -= thisGulpSize;
			}
		}
		else
		{
			if (buffer->desc.cpuAccess != CpuAccessMode::Write)
			{
				// use the upload manager
				Buffer* uploadBuffer;
				uint64_t uploadOffset;
				void* uploadCpuVA;
				m_UploadManager->suballocateBuffer(dataSize, &uploadBuffer, &uploadOffset, &uploadCpuVA, MakeVersion(m_CurrentCmdBuf->recordingID, m_CommandListParameters.queueType, false));

				memcpy(uploadCpuVA, data, dataSize);

				copyBuffer(buffer, destOffsetBytes, uploadBuffer, uploadOffset, dataSize);
			}
			else
			{
				m_Context.error("Using writeBuffer on mappable buffers is invalid");
			}
		}
	}
	void CommandList::clearBufferUInt(IBuffer* b, uint32_t clearValue)
	{
	}
	void CommandList::copyBuffer(IBuffer* _dest, uint64_t destOffsetBytes, IBuffer* _src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes)
	{
		Buffer* dest = dynamic_cast<Buffer*>(_dest);
		Buffer* src = dynamic_cast<Buffer*>(_src);

		GE_ASSERT(destOffsetBytes + dataSizeBytes <= dest->desc.byteSize, "dest buffer size invalid");
		GE_ASSERT(srcOffsetBytes + dataSizeBytes <= src->desc.byteSize, "src buffer size invalid");

		assert(m_CurrentCmdBuf);

		if (dest->desc.cpuAccess != CpuAccessMode::None)
			m_CurrentCmdBuf->referencedStagingBuffers.push_back(dest);
		else
			m_CurrentCmdBuf->referencedResources.push_back(dest);

		if (src->desc.cpuAccess != CpuAccessMode::None)
			m_CurrentCmdBuf->referencedStagingBuffers.push_back(src);
		else
			m_CurrentCmdBuf->referencedResources.push_back(src);

		if (m_EnableAutomaticBarriers)
		{
			_requireBufferState(src, ResourceStates::CopySource);
			_requireBufferState(dest, ResourceStates::CopyDest);
		}
		commitBarriers();

		VkBufferCopy copyRegion{};
		copyRegion.size = dataSizeBytes;
		copyRegion.srcOffset = srcOffsetBytes;
		copyRegion.dstOffset = destOffsetBytes;
		/*auto copyRegion = vk::BufferCopy()
			.setSize(dataSizeBytes)
			.setSrcOffset(srcOffsetBytes)
			.setDstOffset(destOffsetBytes);*/

		vkCmdCopyBuffer(m_CurrentCmdBuf->cmdBuf, src->buffer, dest->buffer, 1, &copyRegion);


		//m_CurrentCmdBuf->cmdBuf.copyBuffer(src->buffer, dest->buffer, { copyRegion });
	}
	void CommandList::setPushConstants(const void* data, size_t byteSize)
	{
		assert(m_CurrentCmdBuf);

		vkCmdPushConstants(m_CurrentCmdBuf->cmdBuf, m_CurrentPipelineLayout, m_CurrentPushConstantsVisibility, 0, uint32_t(byteSize), data);
		//m_CurrentCmdBuf->cmdBuf.pushConstants(m_CurrentPipelineLayout, m_CurrentPushConstantsVisibility, 0, uint32_t(byteSize), data);
	}
	void CommandList::setGraphicsState(const GraphicsState& state)
	{
		assert(m_CurrentCmdBuf);

		GraphicsPipeline* pso = dynamic_cast<GraphicsPipeline*>(state.pipeline);
		Framebuffer* fb = dynamic_cast<Framebuffer*>(state.framebuffer);

		if (m_EnableAutomaticBarriers)
		{
			_trackResourcesAndBarriers(state);
		}

		bool anyBarriers = this->_anyBarriers();
		bool updatePipeline = false;

		if (m_CurrentGraphicsState.pipeline != state.pipeline)
		{
			vkCmdBindPipeline(m_CurrentCmdBuf->cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipeline);
			//m_CurrentCmdBuf->cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, pso->pipeline);

			m_CurrentCmdBuf->referencedResources.push_back(state.pipeline);
			updatePipeline = true;
		}

		if (m_CurrentGraphicsState.framebuffer != state.framebuffer || anyBarriers /* because barriers cannot be set inside a renderpass */)
		{
			_endRenderPass();
		}

		auto desc = state.framebuffer->getDesc();
		if (desc.shadingRateAttachment.valid())
		{
			setTextureState(desc.shadingRateAttachment.texture, TextureSubresourceSet(0, 1, 0, 1), ResourceStates::ShadingRateSurface);
		}

		commitBarriers();

		if (!m_CurrentGraphicsState.framebuffer)
		{
			VkRect2D renderArea;
			renderArea.extent = VkExtent2D{ fb->framebufferInfo.width, fb->framebufferInfo.height };
			renderArea.offset = VkOffset2D{ 0,0 };
			VkRenderPassBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			beginInfo.renderPass = fb->renderPass;
			beginInfo.framebuffer = fb->framebuffer;
			beginInfo.renderArea = renderArea;
			beginInfo.clearValueCount = 0;

			vkCmdBeginRenderPass(m_CurrentCmdBuf->cmdBuf, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
			
			//m_CurrentCmdBuf->cmdBuf.beginRenderPass(vk::RenderPassBeginInfo()
			//	.setRenderPass(fb->renderPass)
			//	.setFramebuffer(fb->framebuffer)
			//	.setRenderArea(vk::Rect2D()
			//		.setOffset(vk::Offset2D(0, 0))
			//		.setExtent(vk::Extent2D(fb->framebufferInfo.width, fb->framebufferInfo.height)))
			//	.setClearValueCount(0),
			//	VkSubpassContents::eInline);

			m_CurrentCmdBuf->referencedResources.push_back(state.framebuffer);
		}

		m_CurrentPipelineLayout = pso->pipelineLayout;
		m_CurrentPushConstantsVisibility = pso->pushConstantVisibility;

		if (arraysAreDifferent(m_CurrentComputeState.bindings, state.bindings) || m_AnyVolatileBufferWrites)
		{
			_bindBindingSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipelineLayout, state.bindings);
		}

		if (!state.viewport.viewports.empty() && arraysAreDifferent(state.viewport.viewports, m_CurrentGraphicsState.viewport.viewports))
		{
			nvrhi::static_vector<VkViewport, c_MaxViewports> viewports;
			for (const auto& vp : state.viewport.viewports)
			{
				viewports.push_back(VKViewportWithDXCoords(vp));
			}
			
			//m_CurrentCmdBuf->cmdBuf.setViewport(0, uint32_t(viewports.size()), viewports.data());
			vkCmdSetViewport(m_CurrentCmdBuf->cmdBuf, 0, uint32_t(viewports.size()), viewports.data());
		}

		if (!state.viewport.scissorRects.empty() && arraysAreDifferent(state.viewport.scissorRects, m_CurrentGraphicsState.viewport.scissorRects))
		{
			nvrhi::static_vector<VkRect2D, c_MaxViewports> scissors;
			for (const auto& sc : state.viewport.scissorRects)
			{
				VkOffset2D offset = { sc.minX, sc.minY };
				VkExtent2D extend = { std::abs(sc.maxX - sc.minX), std::abs(sc.maxY - sc.minY)};
				VkRect2D rect2D;
				rect2D.offset = offset;
				rect2D.extent = extend;

				scissors.push_back(rect2D);
			}

			//m_CurrentCmdBuf->cmdBuf.setScissor(0, uint32_t(scissors.size()), scissors.data());
			vkCmdSetScissor(m_CurrentCmdBuf->cmdBuf, 0, uint32_t(scissors.size()), scissors.data());
		}

		if (pso->usesBlendConstants && (updatePipeline || m_CurrentGraphicsState.blendConstantColor != state.blendConstantColor))
		{
			vkCmdSetBlendConstants(m_CurrentCmdBuf->cmdBuf, &state.blendConstantColor.r);
			//m_CurrentCmdBuf->cmdBuf.setBlendConstants(&state.blendConstantColor.r);
		}

		if (state.indexBuffer.buffer && m_CurrentGraphicsState.indexBuffer != state.indexBuffer)
		{
	/*		m_CurrentCmdBuf->cmdBuf.bindIndexBuffer(dynamic_cast<Buffer*>(state.indexBuffer.buffer)->buffer,
				state.indexBuffer.offset,
				state.indexBuffer.format == Format::R16_UINT ?
				vk::IndexType::eUint16 : vk::IndexType::eUint32);*/

			vkCmdBindIndexBuffer(m_CurrentCmdBuf->cmdBuf, dynamic_cast<Buffer*>(state.indexBuffer.buffer)->buffer,
				state.indexBuffer.offset,
				state.indexBuffer.format == Format::R16_UINT ?
				VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);


			m_CurrentCmdBuf->referencedResources.push_back(state.indexBuffer.buffer);
		}

		if (!state.vertexBuffers.empty() && arraysAreDifferent(state.vertexBuffers, m_CurrentGraphicsState.vertexBuffers))
		{
			VkBuffer vertexBuffers[c_MaxVertexAttributes];
			VkDeviceSize vertexBufferOffsets[c_MaxVertexAttributes];
			uint32_t maxVbIndex = 0;

			for (const auto& binding : state.vertexBuffers)
			{
				// This is tested by the validation layer, skip invalid slots here if VL is not used.
				if (binding.slot >= c_MaxVertexAttributes)
					continue;

				vertexBuffers[binding.slot] = dynamic_cast<Buffer*>(binding.buffer)->buffer;
				vertexBufferOffsets[binding.slot] = VkDeviceSize(binding.offset);
				maxVbIndex = std::max(maxVbIndex, binding.slot);

				m_CurrentCmdBuf->referencedResources.push_back(binding.buffer);
			}
			vkCmdBindVertexBuffers(m_CurrentCmdBuf->cmdBuf, 0, maxVbIndex + 1, vertexBuffers, vertexBufferOffsets);

			//m_CurrentCmdBuf->cmdBuf.bindVertexBuffers(0, maxVbIndex + 1, vertexBuffers, vertexBufferOffsets);
		}

		if (state.indirectParams)
		{
			m_CurrentCmdBuf->referencedResources.push_back(state.indirectParams);
		}

		if (state.shadingRateState.enabled)
		{
			VkFragmentShadingRateCombinerOpKHR combiners[2] = { VkUtil::convertShadingRateCombiner(state.shadingRateState.pipelinePrimitiveCombiner), VkUtil::convertShadingRateCombiner(state.shadingRateState.imageCombiner) };
			VkExtent2D shadingRate = VkUtil::convertFragmentShadingRate(state.shadingRateState.shadingRate);
			//m_CurrentCmdBuf->cmdBuf.setFragmentShadingRateKHR(&shadingRate, combiners);

			// 不支持
			//vkCmdSetFragmentShadingRateKHR(m_CurrentCmdBuf->cmdBuf, &shadingRate, combiners);
		}

		m_CurrentGraphicsState = state;
		m_CurrentComputeState = ComputeState();
		m_CurrentMeshletState = MeshletState();
		//m_CurrentRayTracingState = rt::State();
		m_AnyVolatileBufferWrites = false;
	}
	void CommandList::draw(const DrawArguments& args)
	{
		assert(m_CurrentCmdBuf);

		_updateGraphicsVolatileBuffers();

		vkCmdDraw(m_CurrentCmdBuf->cmdBuf, args.vertexCount,
			args.instanceCount,
			args.startVertexLocation,
			args.startInstanceLocation);
	/*	m_CurrentCmdBuf->cmdBuf.draw(args.vertexCount,
			args.instanceCount,
			args.startVertexLocation,
			args.startInstanceLocation);*/
	}
	void CommandList::drawIndexed(const DrawArguments& args)
	{
		assert(m_CurrentCmdBuf);

		_updateGraphicsVolatileBuffers();
		vkCmdDrawIndexed(
			m_CurrentCmdBuf->cmdBuf,
			args.vertexCount, 
			args.instanceCount, 
			args.startIndexLocation, 
			args.startVertexLocation,
			args.startInstanceLocation);
		//m_CurrentCmdBuf->cmdBuf.drawIndexed(args.vertexCount,
		//	args.instanceCount,
		//	args.startIndexLocation,
		//	args.startVertexLocation,
		//	args.startInstanceLocation);
	}
	void CommandList::drawIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
		assert(m_CurrentCmdBuf);

		_updateGraphicsVolatileBuffers();

		Buffer* indirectParams = dynamic_cast<Buffer*>(m_CurrentGraphicsState.indirectParams);
		assert(indirectParams);

		
		//m_CurrentCmdBuf->cmdBuf.drawIndirect(indirectParams->buffer, offsetBytes, drawCount, sizeof(DrawIndirectArguments));
	}
	void CommandList::drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
		assert(m_CurrentCmdBuf);

		_updateGraphicsVolatileBuffers();

		Buffer* indirectParams = dynamic_cast<Buffer*>(m_CurrentGraphicsState.indirectParams);
		assert(indirectParams);

		//m_CurrentCmdBuf->cmdBuf.drawIndexedIndirect(indirectParams->buffer, offsetBytes, drawCount, sizeof(DrawIndexedIndirectArguments));
	}
	void CommandList::setComputeState(const ComputeState& state)
	{
	}
	void CommandList::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
		assert(m_CurrentCmdBuf);

		_updateComputeVolatileBuffers();

		vkCmdDispatch(m_CurrentCmdBuf->cmdBuf, groupsX, groupsY, groupsZ);
		//m_CurrentCmdBuf->cmdBuf.dispatch(groupsX, groupsY, groupsZ);
	}
	void CommandList::dispatchIndirect(uint32_t offsetBytes)
	{
		assert(m_CurrentCmdBuf);

		_updateComputeVolatileBuffers();

		Buffer* indirectParams = dynamic_cast<Buffer*>(m_CurrentComputeState.indirectParams);
		assert(indirectParams);

		vkCmdDispatchIndirect(m_CurrentCmdBuf->cmdBuf, indirectParams->buffer, offsetBytes);
		//m_CurrentCmdBuf->cmdBuf.dispatchIndirect(indirectParams->buffer, offsetBytes);
	}
	void CommandList::setMeshletState(const MeshletState& state)
	{
	}
	void CommandList::dispatchMesh(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
		assert(m_CurrentCmdBuf);

		if (groupsY > 1 || groupsZ > 1)
		{
			// only 1D dispatches are supported by Vulkan
			RHIUtils::NotSupported();
			return;
		}

		_updateMeshletVolatileBuffers();


		//m_CurrentCmdBuf->cmdBuf.drawMeshTasksNV(groupsX, 0);
	}
	void CommandList::beginTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::endTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::beginMarker(const char* name)
	{
		if (m_Context.extensions.EXT_debug_marker)
		{
			assert(m_CurrentCmdBuf);

			
		/*	auto markerInfo = vk::DebugMarkerMarkerInfoEXT()
				.setPMarkerName(name);*/

			VkDebugMarkerMarkerInfoEXT markerInfo;
			markerInfo.pMarkerName = name;

			
			//m_CurrentCmdBuf->cmdBuf.debugMarkerBeginEXT(&markerInfo);

			//vkCmdDebugMarkerBeginEXT(m_CurrentCmdBuf->cmdBuf, &markerInfo);
		}
	}
	void CommandList::endMarker()
	{

		if (m_Context.extensions.EXT_debug_marker)
		{
			assert(m_CurrentCmdBuf);
			//vkCmdDebugMarkerEndEXT(m_CurrentCmdBuf->cmdBuf);
		}
	}
	void CommandList::setEnableAutomaticBarriers(bool enable)
	{
		m_EnableAutomaticBarriers = enable;
	}
	void CommandList::setResourceStatesForBindingSet(IBindingSet* _bindingSet)
	{
		if (_bindingSet->getDesc() == nullptr)
			return; // is bindless

		BindingSet* bindingSet = checked_cast<BindingSet*>(_bindingSet);

		for (auto bindingIndex : bindingSet->bindingsThatNeedTransitions)
		{
			const BindingSetItem& binding = bindingSet->desc.bindings[bindingIndex];

			switch (binding.type)  // NOLINT(clang-diagnostic-switch-enum)
			{
			case RHIResourceType::RT_Texture_SRV:
				_requireTextureState(checked_cast<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::ShaderResource);
				break;

			case RHIResourceType::RT_Texture_UAV:
				_requireTextureState(checked_cast<ITexture*>(binding.resourceHandle), binding.subresources, ResourceStates::UnorderedAccess);
				break;

			case RHIResourceType::RT_TypedBuffer_SRV:
			case RHIResourceType::RT_StructuredBuffer_SRV:
			case RHIResourceType::RT_RawBuffer_SRV:
				_requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::ShaderResource);
				break;

			case RHIResourceType::RT_TypedBuffer_UAV:
			case RHIResourceType::RT_StructuredBuffer_UAV:
			case RHIResourceType::RT_RawBuffer_UAV:
				_requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::UnorderedAccess);
				break;

			case RHIResourceType::RT_ConstantBuffer:
				_requireBufferState(checked_cast<IBuffer*>(binding.resourceHandle), ResourceStates::ConstantBuffer);
				break;

			case RHIResourceType::RT_RayTracingAccelStruct:
				//TODO::
				//_requireBufferState(checked_cast<AccelStruct*>(binding.resourceHandle)->dataBuffer, ResourceStates::AccelStructRead);
				assert(0);
			default:
				// do nothing
				break;
			}
		}
	}
	void CommandList::setEnableUavBarriersForTexture(ITexture* _texture, bool enableBarriers)
	{
		ETexture* texture = checked_cast<ETexture*>(_texture);

		m_StateTracker.setEnableUavBarriersForTexture(texture, enableBarriers);
	}
	void CommandList::setEnableUavBarriersForBuffer(IBuffer* _buffer, bool enableBarriers)
	{
		Buffer* buffer = checked_cast<Buffer*>(_buffer);

		m_StateTracker.setEnableUavBarriersForBuffer(buffer, enableBarriers);
	}
	void CommandList::beginTrackingTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
		ETexture* texture = checked_cast<ETexture*>(_texture);

		m_StateTracker.beginTrackingTextureState(texture, subresources, stateBits);
	}
	void CommandList::beginTrackingBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = checked_cast<Buffer*>(_buffer);

		m_StateTracker.beginTrackingBufferState(buffer, stateBits);
	}
	void CommandList::setTextureState(ITexture* _texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
		ETexture* texture = checked_cast<ETexture*>(_texture);

		m_StateTracker.endTrackingTextureState(texture, subresources, stateBits, false);
	}
	void CommandList::setBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = checked_cast<Buffer*>(_buffer);

		m_StateTracker.endTrackingBufferState(buffer, stateBits, false);
	}
	void CommandList::setPermanentTextureState(ITexture* _texture, ResourceStates stateBits)
	{
		ETexture* texture = checked_cast<ETexture*>(_texture);

		m_StateTracker.endTrackingTextureState(texture, AllSubresources, stateBits, true);
	}
	void CommandList::setPermanentBufferState(IBuffer* _buffer, ResourceStates stateBits)
	{
		Buffer* buffer = checked_cast<Buffer*>(_buffer);

		m_StateTracker.endTrackingBufferState(buffer, stateBits, true);
	}
	void CommandList::commitBarriers()
	{
		if (m_StateTracker.getBufferBarriers().empty() && m_StateTracker.getTextureBarriers().empty())
			return;

		_endRenderPass();

		if (m_Context.extensions.KHR_synchronization2)
		{
			_commitBarriersInternal_synchronization2();
		}
		else
		{
			_commitBarriersInternal();
		}
	}
	ResourceStates CommandList::getTextureSubresourceState(ITexture* texture, uint32_t arraySlice, uint32_t mipLevel)
	{
		return ResourceStates();
	}
	ResourceStates CommandList::getBufferState(IBuffer* buffer)
	{
		return ResourceStates();
	}
	void CommandList::_updateGraphicsVolatileBuffers()
	{
		if (m_AnyVolatileBufferWrites && m_CurrentGraphicsState.pipeline)
		{
			GraphicsPipeline* pso = checked_cast<GraphicsPipeline*>(m_CurrentGraphicsState.pipeline);

			_bindBindingSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipelineLayout, m_CurrentGraphicsState.bindings);

			m_AnyVolatileBufferWrites = false;
		}
	}
	void CommandList::_updateComputeVolatileBuffers()
	{
		if (m_AnyVolatileBufferWrites && m_CurrentComputeState.pipeline)
		{
			ComputePipeline* pso = checked_cast<ComputePipeline*>(m_CurrentComputeState.pipeline);

			_bindBindingSets(VK_PIPELINE_BIND_POINT_COMPUTE, pso->pipelineLayout, m_CurrentComputeState.bindings);

			m_AnyVolatileBufferWrites = false;
		}
	}
	void CommandList::_updateMeshletVolatileBuffers()
	{
		if (m_AnyVolatileBufferWrites && m_CurrentMeshletState.pipeline)
		{
			MeshletPipeline* pso = checked_cast<MeshletPipeline*>(m_CurrentMeshletState.pipeline);

			_bindBindingSets(VK_PIPELINE_BIND_POINT_GRAPHICS, pso->pipelineLayout, m_CurrentMeshletState.bindings);

			m_AnyVolatileBufferWrites = false;
		}
	}
	void CommandList::_updateRayTracingVolatileBuffers()
	{
	}
	void CommandList::_requireTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates state)
	{
	}
	void CommandList::_requireBufferState(IBuffer* buffer, ResourceStates state)
	{
	}
	bool CommandList::_anyBarriers() const
	{
		return false;
	}
	void CommandList::_commitBarriersInternal()
	{
		std::vector<VkImageMemoryBarrier> imageBarriers;
		std::vector<VkBufferMemoryBarrier> bufferBarriers;
		VkPipelineStageFlags beforeStageFlags = 0;// vk::PipelineStageFlags(0);
		VkPipelineStageFlags afterStageFlags = 0; //vk::PipelineStageFlags(0);

		for (const TextureBarrier& barrier : m_StateTracker.getTextureBarriers())
		{
			ResourceStateMapping before = VkUtil::convertResourceState(barrier.stateBefore);
			ResourceStateMapping after = VkUtil::convertResourceState(barrier.stateAfter);

			if ((before.stageFlags != beforeStageFlags || after.stageFlags != afterStageFlags) && !imageBarriers.empty())
			{
				
		/*		m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
					VkDependencyFlags(), {}, {}, imageBarriers);*/

				vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0,
					0, nullptr,
					0, nullptr,
					imageBarriers.size(), imageBarriers.data());
				imageBarriers.clear();
			}

			beforeStageFlags = before.stageFlags;
			afterStageFlags = after.stageFlags;

			assert(after.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED);

			ETexture* texture = static_cast<ETexture*>(barrier.texture);

			const FormatInfo& formatInfo = getFormatInfo(texture->desc.format);

			VkImageAspectFlags aspectMask = 0;
			if (formatInfo.hasDepth) aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			if (formatInfo.hasStencil) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			if (!aspectMask) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			//VkImageSubresourceRange subresourceRange{};

				//.setBaseArrayLayer(barrier.entireTexture ? 0 : barrier.arraySlice)
				//.setLayerCount(barrier.entireTexture ? texture->desc.arraySize : 1)
				//.setBaseMipLevel(barrier.entireTexture ? 0 : barrier.mipLevel)
				//.setLevelCount(barrier.entireTexture ? texture->desc.mipLevels : 1)
				//.setAspectMask(aspectMask);

			/*imageBarriers.push_back(VkImageMemoryBarrier()
				.setSrcAccessMask(before.accessMask)
				.setDstAccessMask(after.accessMask)
				.setOldLayout(before.imageLayout)
				.setNewLayout(after.imageLayout)
				.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setImage(texture->image)
				.setSubresourceRange(subresourceRange));*/

			VkImageMemoryBarrier imageBarrier{};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier.srcAccessMask = before.accessMask;
			imageBarrier.dstAccessMask = after.accessMask;
			imageBarrier.oldLayout = before.imageLayout;
			imageBarrier.newLayout = after.imageLayout;
			imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.image = texture->image;
			imageBarrier.subresourceRange.aspectMask = aspectMask;
			imageBarrier.subresourceRange.baseMipLevel = barrier.entireTexture ? 0 : barrier.mipLevel;
			imageBarrier.subresourceRange.levelCount = barrier.entireTexture ? texture->desc.mipLevels : 1;
			imageBarrier.subresourceRange.baseArrayLayer = barrier.entireTexture ? 0 : barrier.arraySlice;
			imageBarrier.subresourceRange.layerCount = barrier.entireTexture ? texture->desc.arraySize : 1;

			imageBarriers.push_back(imageBarrier);
		}

		if (!imageBarriers.empty())
		{
			vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0,
				0, nullptr,
				0, nullptr,
				imageBarriers.size(), imageBarriers.data());
	/*		m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
				vk::DependencyFlags(), {}, {}, imageBarriers);*/
		}

		beforeStageFlags = 0;
		afterStageFlags = 0;
		imageBarriers.clear();

		for (const BufferBarrier& barrier : m_StateTracker.getBufferBarriers())
		{
			ResourceStateMapping before = VkUtil::convertResourceState(barrier.stateBefore);
			ResourceStateMapping after = VkUtil::convertResourceState(barrier.stateAfter);

			if ((before.stageFlags != beforeStageFlags || after.stageFlags != afterStageFlags) && !bufferBarriers.empty())
			{
				/*m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
					vk::DependencyFlags(), {}, bufferBarriers, {});*/


				vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0,
					0, nullptr,
					bufferBarriers.size(), bufferBarriers.data(),
					0, nullptr);

				bufferBarriers.clear();
			}

			beforeStageFlags = before.stageFlags;
			afterStageFlags = after.stageFlags;

			Buffer* buffer = static_cast<Buffer*>(barrier.buffer);

			/*bufferBarriers.push_back(VkBufferMemoryBarrier()
				.setSrcAccessMask(before.accessMask)
				.setDstAccessMask(after.accessMask)
				.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setBuffer(buffer->buffer)
				.setOffset(0)
				.setSize(buffer->desc.byteSize));*/

			VkBufferMemoryBarrier bufferBarrier{};
			bufferBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			bufferBarrier.srcAccessMask = before.accessMask;
			bufferBarrier.dstAccessMask = after.accessMask;
			bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.buffer = buffer->buffer;
			bufferBarrier.offset = 0;
			bufferBarrier.size = buffer->desc.byteSize;
			bufferBarriers.push_back(bufferBarrier);
		}

		if (!bufferBarriers.empty())
		{
			//m_CurrentCmdBuf->cmdBuf.pipelineBarrier(beforeStageFlags, afterStageFlags,
			//	VkDependencyFlags(), {}, bufferBarriers, {});

			vkCmdPipelineBarrier(m_CurrentCmdBuf->cmdBuf, beforeStageFlags, afterStageFlags, 0,
				0, nullptr,
				bufferBarriers.size(), bufferBarriers.data(),
				0, nullptr);
		}
		bufferBarriers.clear();

		m_StateTracker.clearBarriers();

	}
	void CommandList::_commitBarriersInternal_synchronization2()
	{
		std::vector<VkImageMemoryBarrier2> imageBarriers;
		std::vector<VkBufferMemoryBarrier2> bufferBarriers;

		for (const TextureBarrier& barrier : m_StateTracker.getTextureBarriers())
		{
			ResourceStateMapping2 before = VkUtil::convertResourceState2(barrier.stateBefore);
			ResourceStateMapping2 after = VkUtil::convertResourceState2(barrier.stateAfter);

			assert(after.imageLayout != VK_IMAGE_LAYOUT_UNDEFINED);

			ETexture* texture = static_cast<ETexture*>(barrier.texture);

			const FormatInfo& formatInfo = getFormatInfo(texture->desc.format);

			VkImageAspectFlags aspectMask = 0;
			if (formatInfo.hasDepth) aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			if (formatInfo.hasStencil) aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			if (!aspectMask) aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;


			/*vk::ImageSubresourceRange subresourceRange = vk::ImageSubresourceRange()
				.setBaseArrayLayer(barrier.entireTexture ? 0 : barrier.arraySlice)
				.setLayerCount(barrier.entireTexture ? texture->desc.arraySize : 1)
				.setBaseMipLevel(barrier.entireTexture ? 0 : barrier.mipLevel)
				.setLevelCount(barrier.entireTexture ? texture->desc.mipLevels : 1)
				.setAspectMask(aspectMask);*/

			//imageBarriers.push_back(vk::ImageMemoryBarrier2()
			//	.setSrcAccessMask(before.accessMask)
			//	.setDstAccessMask(after.accessMask)
			//	.setSrcStageMask(before.stageFlags)
			//	.setDstStageMask(after.stageFlags)
			//	.setOldLayout(before.imageLayout)
			//	.setNewLayout(after.imageLayout)
			//	.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			//	.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			//	.setImage(texture->image)
			//	.setSubresourceRange(subresourceRange));



			VkImageMemoryBarrier2 imageBarrier{};
			imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrier.srcAccessMask = before.accessMask;
			imageBarrier.dstAccessMask = after.accessMask;
			imageBarrier.srcStageMask = before.stageFlags;
			imageBarrier.dstStageMask = after.stageFlags;
			imageBarrier.oldLayout = before.imageLayout;
			imageBarrier.newLayout = after.imageLayout;
			imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageBarrier.image = texture->image;
			imageBarrier.subresourceRange.aspectMask = aspectMask;
			imageBarrier.subresourceRange.baseMipLevel = barrier.entireTexture ? 0 : barrier.mipLevel;
			imageBarrier.subresourceRange.levelCount = barrier.entireTexture ? texture->desc.mipLevels : 1;
			imageBarrier.subresourceRange.baseArrayLayer = barrier.entireTexture ? 0 : barrier.arraySlice;
			imageBarrier.subresourceRange.layerCount = barrier.entireTexture ? texture->desc.arraySize : 1;

			imageBarriers.push_back(imageBarrier);



		}

		if (!imageBarriers.empty())
		{
			VkDependencyInfo dep_info{};
			dep_info.pImageMemoryBarriers = imageBarriers.data();

			//m_CurrentCmdBuf->cmdBuf.pipelineBarrier2(dep_info);

			vkCmdPipelineBarrier2(m_CurrentCmdBuf->cmdBuf, &dep_info);
		}

		imageBarriers.clear();

		for (const BufferBarrier& barrier : m_StateTracker.getBufferBarriers())
		{
			ResourceStateMapping2 before = VkUtil::convertResourceState2(barrier.stateBefore);
			ResourceStateMapping2 after = VkUtil::convertResourceState2(barrier.stateAfter);

			Buffer* buffer = static_cast<Buffer*>(barrier.buffer);

		/*	bufferBarriers.push_back(VkBufferMemoryBarrier2()
				.setSrcAccessMask(before.accessMask)
				.setDstAccessMask(after.accessMask)
				.setSrcStageMask(before.stageFlags)
				.setDstStageMask(after.stageFlags)
				.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.setBuffer(buffer->buffer)
				.setOffset(0)
				.setSize(buffer->desc.byteSize));
*/


			VkBufferMemoryBarrier2  bufferBarrier{};
			bufferBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2;
			bufferBarrier.srcAccessMask = before.accessMask;
			bufferBarrier.dstAccessMask = after.accessMask;
			bufferBarrier.srcStageMask = before.stageFlags;
			bufferBarrier.dstAccessMask = after.stageFlags;
			bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			bufferBarrier.buffer = buffer->buffer;
			bufferBarrier.offset = 0;
			bufferBarrier.size = buffer->desc.byteSize;
			bufferBarriers.push_back(bufferBarrier);

		}

		if (!bufferBarriers.empty())
		{
			VkDependencyInfo dep_info{};
			dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			dep_info.pBufferMemoryBarriers = bufferBarriers.data();
			vkCmdPipelineBarrier2(m_CurrentCmdBuf->cmdBuf, &dep_info);

			//m_CurrentCmdBuf->cmdBuf.pipelineBarrier2(dep_info);
		}
		bufferBarriers.clear();

		m_StateTracker.clearBarriers();

	}
	void CommandList::_clearTexture(ITexture* texture, TextureSubresourceSet subresources, const VkClearColorValue& clearValue)
	{
	}
	void CommandList::_bindBindingSets(VkPipelineBindPoint bindPoint, VkPipelineLayout pipelineLayout, const BindingSetVector& bindings)
	{
		nvrhi::static_vector<uint32_t, c_MaxVolatileConstantBuffers> dynamicOffsets;
		nvrhi::static_vector<VkDescriptorSet, c_MaxBindingLayouts> descriptorSets;

		for (const auto& bindingSetHandle : bindings)
		{
			const BindingSetDesc* desc = bindingSetHandle->getDesc();
			if (desc)
			{
				BindingSet* bindingSet = dynamic_cast<BindingSet*>(bindingSetHandle);
				descriptorSets.push_back(bindingSet->descriptorSet);

				for (Buffer* constnatBuffer : bindingSet->volatileConstantBuffers)
				{
					auto found = m_VolatileBufferStates.find(constnatBuffer);
					if (found == m_VolatileBufferStates.end())
					{
						std::stringstream ss;
						ss << "Binding volatile constant buffer " << (constnatBuffer->desc.debugName)
							<< " before writing into it is invalid.";
						m_Context.error(ss.str());

						dynamicOffsets.push_back(0); // use zero offset just to use something
					}
					else
					{
						uint32_t version = found->second.latestVersion;
						uint64_t offset = version * constnatBuffer->desc.byteSize;
						assert(offset < std::numeric_limits<uint32_t>::max());
						dynamicOffsets.push_back(uint32_t(offset));
					}
				}

				if (desc->trackLiveness)
					m_CurrentCmdBuf->referencedResources.push_back(bindingSetHandle);
			}
			else
			{
				// Vulkan 也有DescriptorTable？
				DescriptorTable* table = dynamic_cast<DescriptorTable*>(bindingSetHandle);
				descriptorSets.push_back(table->descriptorSet);
			}
		}

		if (!descriptorSets.empty())
		{
			
			//m_CurrentCmdBuf->cmdBuf.bindDescriptorSets(bindPoint, pipelineLayout,
			//	/* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
			//	uint32_t(dynamicOffsets.size()), dynamicOffsets.data());


			vkCmdBindDescriptorSets(m_CurrentCmdBuf->cmdBuf, bindPoint, pipelineLayout,
				/* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
				uint32_t(dynamicOffsets.size()), dynamicOffsets.data());
		}
	}
	void CommandList::_endRenderPass()
	{
		if (m_CurrentGraphicsState.framebuffer || m_CurrentMeshletState.framebuffer)
		{
			vkCmdEndRenderPass(m_CurrentCmdBuf->cmdBuf);
			//m_CurrentCmdBuf->cmdBuf.endRenderPass();
			m_CurrentGraphicsState.framebuffer = nullptr;
			m_CurrentMeshletState.framebuffer = nullptr;
		}
	}
	void CommandList::_trackResourcesAndBarriers(const GraphicsState& state)
	{
		assert(m_EnableAutomaticBarriers);

		if (arraysAreDifferent(state.bindings, m_CurrentGraphicsState.bindings))
		{
			for (size_t i = 0; i < state.bindings.size(); i++)
			{
				setResourceStatesForBindingSet(state.bindings[i]);
			}
		}

		if (state.indexBuffer.buffer && state.indexBuffer.buffer != m_CurrentGraphicsState.indexBuffer.buffer)
		{
			_requireBufferState(state.indexBuffer.buffer, ResourceStates::IndexBuffer);
		}

		if (arraysAreDifferent(state.vertexBuffers, m_CurrentGraphicsState.vertexBuffers))
		{
			for (const auto& vb : state.vertexBuffers)
			{
				_requireBufferState(vb.buffer, ResourceStates::VertexBuffer);
			}
		}

		if (m_CurrentGraphicsState.framebuffer != state.framebuffer)
		{
			setResourceStatesForFramebuffer(state.framebuffer);
		}

		if (state.indirectParams && state.indirectParams != m_CurrentGraphicsState.indirectParams)
		{
			_requireBufferState(state.indirectParams, ResourceStates::IndirectArgument);
		}
	}
	void CommandList::_trackResourcesAndBarriers(const MeshletState& state)
	{
	}
	void CommandList::_writeVolatileBuffer(Buffer* buffer, const void* data, size_t dataSize)
	{
		VolatileBufferState& state = m_VolatileBufferStates[buffer];

		if (!state.initialized)
		{
			state.minVersion = int(buffer->desc.maxVersions);
			state.maxVersion = -1;
			state.initialized = true;
		}

		std::array<uint64_t, uint32_t(CommandQueue::Count)> queueCompletionValues = {
			getQueueLastFinishedID(m_Device, CommandQueue::Graphics),
			getQueueLastFinishedID(m_Device, CommandQueue::Compute),
			getQueueLastFinishedID(m_Device, CommandQueue::Copy)
		};

		uint32_t searchStart = buffer->versionSearchStart;
		uint32_t maxVersions = buffer->desc.maxVersions;
		uint32_t version = 0;

		uint64_t originalVersionInfo = 0;

		// Since versionTracking[] can be accessed by multiple threads concurrently,
		// perform the search in a loop ending with compare_exchange until the exchange is successful.
		while (true)
		{
			bool found = false;

			// Search through the versions of this buffer, looking for either unused (0)
			// or submitted and already finished versions

			for (uint32_t searchIndex = 0; searchIndex < maxVersions; searchIndex++)
			{
				version = searchIndex + searchStart;
				version = (version >= maxVersions) ? (version - maxVersions) : version;

				originalVersionInfo = buffer->versionTracking[version];

				if (originalVersionInfo == 0)
				{
					// Previously unused version - definitely available
					found = true;
					break;
				}

				// Decode the bitfield
				bool isSubmitted = (originalVersionInfo & c_VersionSubmittedFlag) != 0;
				uint32_t queueIndex = uint32_t(originalVersionInfo >> c_VersionQueueShift) & c_VersionQueueMask;
				uint64_t id = originalVersionInfo & c_VersionIDMask;

				// If the version is in a recorded but not submitted command list,
				// we can't use it. So, only compare the version ID for submitted CLs.
				if (isSubmitted)
				{
					// Versions can potentially be used in CLs submitted to different queues.
					// So we store the queue index and use look at the last finished CL in that queue.

					if (queueIndex >= uint32_t(CommandQueue::Count))
					{
						// If the version points at an invalid queue, assume it's available. Signal the error too.
						//utils::InvalidEnum();
						assert(!"Invalid Enumeration Value");
						found = true;
						break;
					}

					if (id <= queueCompletionValues[queueIndex])
					{
						// If the version was used in a completed CL, it's available.
						found = true;
						break;
					}
				}
			}

			if (!found)
			{
				// Not enough versions - need to relay this information to the developer.
				// This has to be a real message and not assert, because asserts only happen in the
				// debug mode, and buffer versioning will behave differently in debug vs. release,
				// or validation on vs. off, because it is timing related.

				std::stringstream ss;
				ss << "Volatile constant buffer " << buffer->desc.debugName <<
					" has maxVersions = " << buffer->desc.maxVersions << ", which is insufficient.";

				m_Context.error(ss.str());
				return;
			}

			// Encode the current CL ID for this version of the buffer, in a "pending" state
			uint64_t newVersionInfo = (uint64_t(m_CommandListParameters.queueType) << c_VersionQueueShift) | (m_CurrentCmdBuf->recordingID);

			// Try to store the new version info, end the loop if we actually won this version, i.e. no other thread has claimed it
			if (buffer->versionTracking[version].compare_exchange_weak(originalVersionInfo, newVersionInfo))
				break;
		}

		buffer->versionSearchStart = (version + 1 < maxVersions) ? (version + 1) : 0;

		// Store the current version and expand the version range in this CL
		state.latestVersion = int(version);
		state.minVersion = std::min(int(version), state.minVersion);
		state.maxVersion = std::max(int(version), state.maxVersion);

		// Finally, write the actual data
		void* hostData = (char*)buffer->mappedMemory + version * buffer->desc.byteSize;
		memcpy(hostData, data, dataSize);

		m_AnyVolatileBufferWrites = true;
	}
	void CommandList::_flushVolatileBufferWrites()
	{
		// The volatile CBs are permanently mapped with the eHostVisible flag, but not eHostCoherent,
		// so before using the data on the GPU, we need to make sure it's available there.
		// Go over all the volatile CBs that were used in this CL and flush their written versions.

		std::vector<VkMappedMemoryRange> ranges;

		for (auto& iter : m_VolatileBufferStates)
		{
			Buffer* buffer = iter.first;
			VolatileBufferState& state = iter.second;

			if (state.maxVersion < state.minVersion || !state.initialized)
				continue;

			// Flush all the versions between min and max - that might be too conservative,
			// but that should be fine - better than using potentially hundreds of ranges.
			int numVersions = state.maxVersion - state.minVersion + 1;

			VkMappedMemoryRange range;
			range.memory = buffer->memory;
			range.offset = state.minVersion * buffer->desc.byteSize;
			range.size = numVersions * buffer->desc.byteSize;
		/*	auto range = vk::MappedMemoryRange()
				.setMemory(she->memory)
				.setOffset(state.minVersion * buffer->desc.byteSize)
				.setSize(numVersions * buffer->desc.byteSize);*/

			ranges.push_back(range);
		}

		if (!ranges.empty())
		{
			//m_Context.device.flushMappedMemoryRanges(ranges);
			vkFlushMappedMemoryRanges(m_Context.device, ranges.size(), ranges.data());
		}
	}
	void CommandList::_submitVolatileBuffers(uint64_t recordingID, uint64_t submittedID)
	{// For each volatile CB that was written in this command list, and for every version thereof,
        // we need to replace the tracking information from "pending" to "submitted".
        // This is potentially slow as there might be hundreds of versions of a buffer,
        // but at least the find-and-replace operation is constrained to the min/max version range.

		uint64_t stateToFind = (uint64_t(m_CommandListParameters.queueType) << c_VersionQueueShift) | (recordingID & c_VersionIDMask);
		uint64_t stateToReplace = (uint64_t(m_CommandListParameters.queueType) << c_VersionQueueShift) | (submittedID & c_VersionIDMask) | c_VersionSubmittedFlag;

		for (auto& iter : m_VolatileBufferStates)
		{
			Buffer* buffer = iter.first;
			VolatileBufferState& state = iter.second;

			if (!state.initialized)
				continue;

			for (int version = state.minVersion; version <= state.maxVersion; version++)
			{
				// Use compare_exchange to conditionally replace the entries equal to stateToFind with stateToReplace.
				uint64_t expected = stateToFind;
				buffer->versionTracking[version].compare_exchange_strong(expected, stateToReplace);
			}
		}
	}
}
#endif
