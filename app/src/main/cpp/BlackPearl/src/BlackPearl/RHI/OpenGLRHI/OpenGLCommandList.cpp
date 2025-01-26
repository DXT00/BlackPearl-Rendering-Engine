#include "OpenGLCommandList.h"
#include "OpenGLBufferResource.h"
#include "OpenGLBindingSet.h"
#include "BlackPearl/RHI/RHIGlobals.h"
namespace BlackPearl {
	CommandList::CommandList(Device* device, const OpenGLContext& context, const CommandListParameters& parameters)
		:m_Context(context)
	{
	}
	void CommandList::open()
	{
	}
	void CommandList::close()
	{
	}
	void CommandList::clearState()
	{
	}
	void CommandList::clearTextureFloat(ITexture* texture, TextureSubresourceSet subresources, const Color& clearColor)
	{
	}
	void CommandList::clearDepthStencilTexture(ITexture* texture, TextureSubresourceSet subresources, bool clearDepth, float depth, bool clearStencil, uint8_t stencil)
	{
	}
	void CommandList::clearTextureUInt(ITexture* texture, TextureSubresourceSet subresources, uint32_t clearColor)
	{
	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& destSlice, ITexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::copyTexture(IStagingTexture* dest, const TextureSlice& dstSlice, ITexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::copyTexture(ITexture* dest, const TextureSlice& dstSlice, IStagingTexture* src, const TextureSlice& srcSlice)
	{
	}
	void CommandList::writeTexture(ITexture* dest, uint32_t arraySlice, uint32_t mipLevel, const void* data, size_t rowPitch, size_t depthPitch)
	{
	}
	void CommandList::resolveTexture(ITexture* dest, const TextureSubresourceSet& dstSubresources, ITexture* src, const TextureSubresourceSet& srcSubresources)
	{
	}
	void CommandList::writeBuffer(IBuffer* _buffer, const void* data, size_t dataSize, uint64_t destOffsetBytes)
	{
		// RHIUpdateUniformBuffer(FRHICommandListBase & RHICmdList, FRHIUniformBuffer * UniformBufferRHI, const void* Contents) final override;
		Buffer* buffer = dynamic_cast<Buffer*>(_buffer);
		/*if (buffer->desc.isVolatile)
		{
			assert(destOffsetBytes == 0);

			_writeVolatileBuffer(buffer, data, dataSize);

			return;
		}*/
		if (buffer->desc.isVertexBuffer) {
			VertexBuffer* vbo = static_cast<VertexBuffer*>(buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->rendererID);
			glBufferData(GL_ARRAY_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
		else if (buffer->desc.isIndexBuffer) {
			IndexBuffer* ibo = static_cast<IndexBuffer*>(buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo->rendererID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
		else if (buffer->desc.isDrawIndirectArgs) {
			IndirectBuffer* ibo = static_cast<IndirectBuffer*>(buffer);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibo->rendererID);
			glBufferData(GL_DRAW_INDIRECT_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
		else if (buffer->desc.isDrawIndirectArgs) {
			ShaderStorageBuffer* ssbo = static_cast<ShaderStorageBuffer*>(buffer);
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->rendererID);
			glBufferData(GL_SHADER_STORAGE_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
		}
	}
	void CommandList::clearBufferUInt(IBuffer* b, uint32_t clearValue)
	{
	}
	void CommandList::copyBuffer(IBuffer* dest, uint64_t destOffsetBytes, IBuffer* src, uint64_t srcOffsetBytes, uint64_t dataSizeBytes)
	{
	}
	void CommandList::setPushConstants(const void* data, size_t byteSize)
	{
	}
	void CommandList::setBoundShaderState(BoundShaderState* state)
	{
		FOpenGLBoundShaderState* BoundShaderState = static_cast<FOpenGLBoundShaderState*>(state);
		m_Device->PendingState.BoundShaderState = BoundShaderState;

		// Prevent transient bound shader states from being recreated for each use by keeping a history of the most recently used bound shader states.
		// The history keeps them alive, and the bound shader state cache allows them to be reused if needed.
		//BoundShaderStateHistory.Add(BoundShaderState);
	}
	void CommandList::_bindBindingSets(const BindingSetVector& bindings)
	{
		std::vector<uint32_t> dynamicOffsets;
		//std::vector<VkDescriptorSet> descriptorSets;

		for (const auto& bindingSetHandle : bindings)
		{
			const BindingSetDesc* desc = bindingSetHandle->getDesc();
			if (desc)
			{
				BindingSet* bindingSet = static_cast<BindingSet*>(bindingSetHandle);
				//descriptorSets.push_back(bindingSet->descriptorSet);

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

		//if (!descriptorSets.empty())
		//{

		//	//m_CurrentCmdBuf->cmdBuf.bindDescriptorSets(bindPoint, pipelineLayout,
		//	//	/* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
		//	//	uint32_t(dynamicOffsets.size()), dynamicOffsets.data());

		//	if (dynamicOffsets.empty()) {
		//		vkCmdBindDescriptorSets(m_CurrentCmdBuf->cmdBuf, bindPoint, pipelineLayout,
		//			/* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
		//			0, nullptr);
		//	}
		//	else {
		//		vkCmdBindDescriptorSets(m_CurrentCmdBuf->cmdBuf, bindPoint, pipelineLayout,
		//			/* firstSet = */ 0, uint32_t(descriptorSets.size()), descriptorSets.data(),
		//			uint32_t(dynamicOffsets.size()), dynamicOffsets.data());
		//	}


		//}
	}
	void CommandList::setGraphicsState(const GraphicsState& state)
	{
		//bind uniformbuffer

		//TODO::
		//FRHIGraphicsPipelineStateFallBack* FallbackGraphicsState = static_cast<FRHIGraphicsPipelineStateFallBack*>(GraphicsState);

		//auto& PsoInit = FallbackGraphicsState->Initializer;

		//if (PsoInit.bFromPSOFileCache)
		//{
		//	//checkNoEntry();
		//	// 		// If we're from the PSO cache we're just preparing the PSO and do not need to set the state.
		//	return;
		//}
		//TODO:: 同时设置多个 viewport 需要用glViewportArrayv
		setViewport(state.viewport.viewports[0].minX, state.viewport.viewports[0].minY, state.viewport.viewports[0].minZ, 
			state.viewport.viewports[0].maxX, state.viewport.viewports[0].maxY, state.viewport.viewports[0].maxZ);

		setScissorRect(true, state.viewport.scissorRects[0].minX, state.viewport.scissorRects[0].minY,
			state.viewport.scissorRects[0].maxX, state.viewport.scissorRects[0].maxY);

	
		//uniform buffer + texture, sampler...
		_bindBindingSets(state.bindings);
		

		setBoundShaderState(
			m_Device->RHICreateBoundShaderState_Internal(
				static_cast<InputLayout*>(state.pipeline->desc.inputLayout.Get()),
				static_cast<Shader*>(state.pipeline->desc.VS.Get()),
				static_cast<Shader*>(state.pipeline->desc.PS.Get()),
				static_cast<Shader*>(state.pipeline->desc.GS.Get()),
				state.pipeline->desc.bFromPSOFileCache
			)
		);

		setDepthStencilaState(&state.pipeline->desc.depthStencilState);
		setRasterizerState(&state.pipeline->desc.rasterState);
		setBlendState(&state.pipeline->desc.blendState);
		if (GSupportsDepthBoundsTest)
		{
			//RHIEnableDepthBoundsTest(FallbackGraphicsState->Initializer.bDepthBounds);
		}

		/*if (bApplyAdditionalState)
		{
			ApplyStaticUniformBuffers(PsoInit.BoundShaderState.VertexShaderRHI, ResourceCast(PsoInit.BoundShaderState.VertexShaderRHI));
			ApplyStaticUniformBuffers(PsoInit.BoundShaderState.GetGeometryShader(), ResourceCast(PsoInit.BoundShaderState.GetGeometryShader()));
			ApplyStaticUniformBuffers(PsoInit.BoundShaderState.PixelShaderRHI, ResourceCast(PsoInit.BoundShaderState.PixelShaderRHI));
		}*/

		// Store the PSO's primitive (after since IRHICommandContext::RHISetGraphicsPipelineState sets the BSS)
		m_Device->PSOPrimitiveType = state.pipeline->desc.primType;
	}
	void CommandList::draw(const DrawArguments& args)
	{
		//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveTime);
		//VERIFY_GL_SCOPE();
		//RHI_DRAW_CALL_STATS(PrimitiveType, NumPrimitives * NumInstances);

		FOpenGLContextState& ContextState = m_Device->GetContextStateForCurrentContext();
		m_Device->BindPendingFramebuffer(ContextState);
		m_Device->SetPendingBlendStateForActiveRenderTargets(ContextState);
		m_Device->UpdateViewportInOpenGLContext(ContextState);
		m_Device->UpdateScissorRectInOpenGLContext(ContextState);
		m_Device->UpdateRasterizerStateInOpenGLContext(ContextState);
		m_Device->UpdateDepthStencilStateInOpenGLContext(ContextState);
		m_Device->BindPendingShaderState(ContextState);
		m_Device->CommitGraphicsResourceTables();
		m_Device->SetupTexturesForDraw(ContextState);
		m_Device->SetupUAVsForDraw(ContextState);
		m_Device->CommitNonComputeShaderConstants();
		m_Device->CachedBindElementArrayBuffer(ContextState, 0);
		uint32_t VertexCount = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);
		SetupVertexArrays(ContextState, BaseVertexIndex, PendingState.Streams, NUM_OPENGL_VERTEX_STREAMS, VertexCount);

		GLenum DrawMode = GL_TRIANGLES;
		GLsizei NumElements = 0;
		FindPrimitiveType(PrimitiveType, NumPrimitives, DrawMode, NumElements);

		GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
		if (NumInstances == 1)
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			glDrawArrays(DrawMode, 0, NumElements);
		}
		else
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			FOpenGL::DrawArraysInstanced(DrawMode, 0, NumElements, NumInstances);
		}
		GOpenGLKickHint.OnDrawCall(ContextState);
	}
	void CommandList::drawIndexed(const DrawArguments& args)
	{
	}
	void CommandList::drawIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
	}
	void CommandList::drawIndexedIndirect(uint32_t offsetBytes, uint32_t drawCount)
	{
	}
	void CommandList::setComputeState(const ComputeState& state)
	{
	}
	void CommandList::dispatch(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
	}
	void CommandList::dispatchIndirect(uint32_t offsetBytes)
	{
	}
	void CommandList::setMeshletState(const MeshletState& state)
	{
	}
	void CommandList::dispatchMesh(uint32_t groupsX, uint32_t groupsY, uint32_t groupsZ)
	{
	}
	void CommandList::setRayTracingState(const RayTracingState& state)
	{
	}
	void CommandList::dispatchRays(const DispatchRaysArguments& args)
	{
	}
	void CommandList::buildOpacityMicromap(rt::IOpacityMicromap* omm, const rt::OpacityMicromapDesc& desc)
	{
	}
	void CommandList::buildBottomLevelAccelStruct(rt::IAccelStruct* as, const rt::GeometryDesc* pGeometries, size_t numGeometries, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::compactBottomLevelAccelStructs()
	{
	}
	void CommandList::buildTopLevelAccelStruct(rt::IAccelStruct* as, const rt::InstanceDesc* pInstances, size_t numInstances, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::buildTopLevelAccelStructFromBuffer(rt::IAccelStruct* as, IBuffer* instanceBuffer, uint64_t instanceBufferOffset, size_t numInstances, rt::AccelStructBuildFlags buildFlags)
	{
	}
	void CommandList::beginTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::endTimerQuery(ITimerQuery* query)
	{
	}
	void CommandList::beginMarker(const char* name)
	{
	}
	void CommandList::endMarker()
	{
	}
	void CommandList::setEnableAutomaticBarriers(bool enable)
	{
	}
	void CommandList::setResourceStatesForBindingSet(IBindingSet* bindingSet)
	{
	}
	void CommandList::setEnableUavBarriersForTexture(ITexture* texture, bool enableBarriers)
	{
	}
	void CommandList::setEnableUavBarriersForBuffer(IBuffer* buffer, bool enableBarriers)
	{
	}
	void CommandList::beginTrackingTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
	}
	void CommandList::beginTrackingBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::setTextureState(ITexture* texture, TextureSubresourceSet subresources, ResourceStates stateBits)
	{
	}
	void CommandList::setBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::setPermanentTextureState(ITexture* texture, ResourceStates stateBits)
	{
	}
	void CommandList::setPermanentBufferState(IBuffer* buffer, ResourceStates stateBits)
	{
	}
	void CommandList::commitBarriers()
	{
	}
	ResourceStates CommandList::getTextureSubresourceState(ITexture* texture, uint32_t arraySlice, uint32_t mipLevel)
	{
		return ResourceStates();
	}
	ResourceStates CommandList::getBufferState(IBuffer* buffer)
	{
		return ResourceStates();
	}
	void CommandList::setViewport(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	{
		m_Device->PendingState.Viewport.minX = (uint32_t)minX;
		m_Device->PendingState.Viewport.minY = (uint32_t)minY;
		m_Device->PendingState.Viewport.maxX = (uint32_t)maxX;
		m_Device->PendingState.Viewport.maxY = (uint32_t)maxY;
		m_Device->PendingState.DepthMinZ = minZ;
		m_Device->PendingState.DepthMaxZ = maxZ;

		setScissorRect(false, 0, 0, 0, 0);
	}
	void CommandList::setScissorRect(bool bEnable, uint32_t minX, uint32_t minY, uint32_t maxX, uint32_t maxY)
	{
		m_Device->PendingState.bScissorEnabled = bEnable;
		m_Device->PendingState.Scissor.minX = minX;
		m_Device->PendingState.Scissor.minY = minY;
		m_Device->PendingState.Scissor.maxX = maxX;
		m_Device->PendingState.Scissor.maxY = maxY;
	}



	void Device::CommitGraphicsResourceTablesInner()
	{
	}
	void Device::CommitComputeResourceTables(Shader* ComputeShader)
	{
	}
	void Device::CommitNonComputeShaderConstants()
	{
	}
	void Device::CommitComputeShaderConstants(Shader* ComputeShader)
	{
	}

	void CommandList::setDepthStencilaState(DepthStencilState* state)
	{
		m_Device->PendingState.DepthStencilState = *state;

	}

	void CommandList::setRasterizerState(RasterState* state)
	{
		m_Device->PendingState.RasterizerState = *state;

	}

	void CommandList::setBlendState(BlendState* state)
	{
		//深拷贝
		m_Device->PendingState.BlendState = *state;
		/*FOpenGLBlendState* NewState = ResourceCast(NewStateRHI);
		FMemory::Memcpy(&PendingState.BlendState, &(NewState->Data), sizeof(FOpenGLBlendStateData));*/
	}

}