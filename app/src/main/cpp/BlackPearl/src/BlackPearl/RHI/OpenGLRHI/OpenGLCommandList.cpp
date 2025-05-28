#include "pch.h"
#include "RHI/OpenGLRHI/OpenGLDevice.h"
#include "RHI/OpenGLRHI/OpenGLBoundShaderState.h"
#include "RHI/OpenGLRHI/OpenGLContext.h"
#include "RHI/OpenGLRHI/OpenGLCommandList.h"
#include "RHI/OpenGLRHI/OpenGLBufferResource.h"
#include "RHI/OpenGLRHI/OpenGLUniformBuffer.h"
#include "RHI/OpenGLRHI/OpenGLBindingSet.h"
#include "RHI/OpenGLRHI/OpenGLState.h"
#include "RHI/OpenGLRHI/OpenGLViewport.h"
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "RHI/OpenGLRHI/OpenGLFrameBuffer.h"
#include "RHI/OpenGLRHI/OpenGLSampler.h"
#include "RHI/OpenGLRHI/OpenGLUtil.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/RHIGlobals.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/RHI/RHIRenderTarget.h"
namespace BlackPearl {
	struct FPlatformOpenGLDevice;
	inline void FindPrimitiveType(PrimitiveType InPrimitiveType, uint32_t InNumPrimitives, GLenum& DrawMode, GLsizei& NumElements)
	{
		DrawMode = GL_TRIANGLES;
		NumElements = InNumPrimitives;

		switch (InPrimitiveType)
		{
		case PrimitiveType::TriangleList:
			DrawMode = GL_TRIANGLES;
			NumElements = InNumPrimitives * 3;
			break;
		case PrimitiveType::TriangleStrip:
			DrawMode = GL_TRIANGLE_STRIP;
			NumElements = InNumPrimitives + 2;
			break;
		case PrimitiveType::LineList:
			DrawMode = GL_LINES;
			NumElements = InNumPrimitives * 2;
			break;
		case PrimitiveType::PointList:
			DrawMode = GL_POINTS;
			NumElements = InNumPrimitives;
			break;
		default:
			GE_ASSERT(0, "Unsupported primitive type %u", InPrimitiveType);
			break;
		}
	}
	// Replaces RenderTargets with ResoveTargets to utilize GL_EXT_multisampled_render_to_texture
	static int32_t SetupMultisampleRenderingInfo(FRHISetRenderTargetsInfo& RTInfo)
	{
		if (FOpenGL::GetMaxMSAASamplesTileMem() > 1 && RTInfo.NumColorRenderTargets > 0)
		{
			int32_t NumRenderingSamples = RTInfo.ColorRenderTarget[0].Texture->getDesc().sampleCount;
			if (NumRenderingSamples > 1)
			{
				for (int32_t i = 0; i < RTInfo.NumColorRenderTargets; ++i)
				{
					if (RTInfo.ColorResolveRenderTarget[i].Texture)
					{
						RTInfo.ColorRenderTarget[i].Texture = RTInfo.ColorResolveRenderTarget[i].Texture;
					}
				}

				return NumRenderingSamples;
			}
		}

		return 1;
	}



	CommandList::CommandList(Device* device, const OpenGLContext& context, const CommandListParameters& parameters)
		:m_Context(context), m_Device(device)
	{
	}
	void CommandList::beginRenderPass(const FRHIRenderPassInfo& renderPassInfo, const std::string& passName)
	{
		GE_ERROR_JUDGE();

		FRHISetRenderTargetsInfo RTInfo;
		renderPassInfo.ConvertToRenderTargetsInfo(RTInfo);
		// Begin GL_EXT_multisampled_render_to_texture if any
		m_Device->PendingState.NumRenderingSamples = SetupMultisampleRenderingInfo(RTInfo);
		_setRenderTargetsAndClear(RTInfo);
		GE_ERROR_JUDGE();

		m_Device->RenderPassInfo = renderPassInfo;

		if (renderPassInfo.NumOcclusionQueries > 0)
		{
			/*extern void BeginOcclusionQueryBatch(uint32_t);
			BeginOcclusionQueryBatch(renderPassInfo.NumOcclusionQueries);*/
		}

#ifdef GE_PLATFORM_ANDROID
		if (m_Device->RenderPassInfo.SubpassHint == ESubpassHint::DeferredShadingSubpass &&
			FOpenGL::SupportsPixelLocalStorage() && FOpenGL::SupportsShaderDepthStencilFetch())
		{
			glEnable(GL_SHADER_PIXEL_LOCAL_STORAGE_EXT);
            GE_ERROR_JUDGE();

        }
#endif

#ifdef GE_PLATFORM_ANDROID
		if (FAndroidOpenGL::RequiresAdrenoTilingModeHint())
		{
			FAndroidOpenGL::EnableAdrenoTilingModeHint(passName ==("BasePass"));
		}
#endif
	}

	void CommandList::endRenderPass()
	{
		if (m_Device->RenderPassInfo.NumOcclusionQueries > 0)
		{
			/*extern void EndOcclusionQueryBatch();
			EndOcclusionQueryBatch();*/
		}

		// End GL_EXT_multisampled_render_to_texture
		m_Device->PendingState.NumRenderingSamples = 1;

		// Discard transient color targets
		uint32_t ColorMask = 0u;
		for (int32_t ColorIndex = 0; ColorIndex < c_MaxRenderTargets; ++ColorIndex)
		{
			const FRHIRenderPassInfo::FColorEntry& Entry = m_Device->RenderPassInfo.ColorRenderTargets[ColorIndex];
			if (!Entry.RenderTarget)
			{
				break;
			}

			if (GetStoreAction(Entry.Action) == ERenderTargetStoreAction::ENoAction)
			{
				ColorMask |= (1u << ColorIndex);
			}
		}

		// Discard transient DepthStencil
		bool bDiscardDepthStencil = false;
		if (m_Device->RenderPassInfo.DepthStencilRenderTarget.DepthStencilTarget)
		{
			ERenderTargetActions DepthActions = GetDepthActions(m_Device->RenderPassInfo.DepthStencilRenderTarget.Action);
			bDiscardDepthStencil = GetStoreAction(DepthActions) == ERenderTargetStoreAction::ENoAction;
		}

		if (bDiscardDepthStencil || ColorMask != 0)
		{
			_discardRenderTargets(bDiscardDepthStencil, bDiscardDepthStencil, ColorMask);
		}

		FRHIRenderTargetView RTV(nullptr, ERenderTargetLoadAction::ENoAction);
		FRHIDepthRenderTargetView DepthRTV(nullptr, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::ENoAction);
		_setRenderTargets(1, &RTV, &DepthRTV);

#if GE_PLATFORM_ANDROID
		if (m_Device->RenderPassInfo.SubpassHint == ESubpassHint::DeferredShadingSubpass &&
			FOpenGL::SupportsPixelLocalStorage() && FOpenGL::SupportsShaderDepthStencilFetch())
		{
			glDisable(GL_SHADER_PIXEL_LOCAL_STORAGE_EXT);
		}
#endif
	}
	void CommandList::nextSubpass()
	{
		//IRHICommandContext::RHINextSubpass();

		if (m_Device->RenderPassInfo.SubpassHint == ESubpassHint::DepthReadSubpass ||
			m_Device->RenderPassInfo.SubpassHint == ESubpassHint::DeferredShadingSubpass)
		{
			FOpenGL::FrameBufferFetchBarrier();
		}
	}

	void CommandList::_discardRenderTargets(bool Depth, bool Stencil, uint32_t ColorBitMaskIn)
	{
		if (FOpenGL::SupportsDiscardFrameBuffer())
		{
			//VERIFY_GL_SCOPE();
			const bool bDefaultFramebuffer = (m_Device->PendingState.Framebuffer == 0);
			uint32_t ColorBitMask = ColorBitMaskIn;
			// 8 Color + Depth + Stencil = 10
			GLenum Attachments[c_MaxRenderTargets + 2];
			uint32_t I = 0;
			if (Depth)
			{
				Attachments[I] = bDefaultFramebuffer ? GL_DEPTH : GL_DEPTH_ATTACHMENT;
				I++;
			}
			if (Stencil)
			{
				Attachments[I] = bDefaultFramebuffer ? GL_STENCIL : GL_STENCIL_ATTACHMENT;
				I++;
			}

			if (bDefaultFramebuffer)
			{
				if (ColorBitMask)
				{
					Attachments[I] = GL_COLOR;
					I++;
				}
			}
			else
			{
				ColorBitMask &= (1 << c_MaxRenderTargets) - 1;
				uint32_t J = 0;
				while (ColorBitMask)
				{
					if (ColorBitMask & 1)
					{
						Attachments[I] = GL_COLOR_ATTACHMENT0 + J;
						I++;
					}

					ColorBitMask >>= 1;
					++J;
				}
			}

			FOpenGL::InvalidateFramebuffer(GL_FRAMEBUFFER, I, Attachments);
		}
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
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//GE_ERROR_JUDGE();

		if (buffer->desc.isVertexBuffer) {
			VertexBuffer* vbo = static_cast<VertexBuffer*>(buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vbo->rendererID);
			GE_ERROR_JUDGE();
			glBufferSubData(GL_ARRAY_BUFFER, destOffsetBytes, dataSize, data);
			GE_ERROR_JUDGE();
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
		else if (buffer->desc.isConstantBuffer) {
			OpenGLUniformBuffer* ubo = static_cast<OpenGLUniformBuffer*>(buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo->rendererID);
			glBufferData(GL_UNIFORM_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
			//glBufferData(GL_UNIFORM_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
		}
		GE_ERROR_JUDGE();
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
	void CommandList::setBoundShaderState(IBoundShaderState* state)
	{
        BoundShaderState* BoundShaderState_ = static_cast<BoundShaderState*>(state);
		m_Device->PendingState.BoundShaderState = BoundShaderState_;

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

//				for (Buffer* constnatBuffer : bindingSet->volatileConstantBuffers)
//				{
//					auto found = m_VolatileBufferStates.find(constnatBuffer);
//					if (found == m_VolatileBufferStates.end())
//					{
//						std::stringstream ss;
//						ss << "Binding volatile constant buffer " << (constnatBuffer->desc.debugName)
//							<< " before writing into it is invalid.";
//						m_Context.error(ss.str());
//
//						dynamicOffsets.push_back(0); // use zero offset just to use something
//					}
//					else
//					{
//						uint32_t version = found->second.latestVersion;
//						uint64_t offset = version * constnatBuffer->desc.byteSize;
//						assert(offset < std::numeric_limits<uint32_t>::max());
//						dynamicOffsets.push_back(uint32_t(offset));
//					}
//				}
//
//				if (desc->trackLiveness)
//					m_CurrentCmdBuf->referencedResources.push_back(bindingSetHandle);
			}
			else
			{
//				// Vulkan Ҳ��DescriptorTable��
//				DescriptorTable* table = dynamic_cast<DescriptorTable*>(bindingSetHandle);
//				descriptorSets.push_back(table->descriptorSet);
			}
		}

		//}
	}
	void CommandList::_setRenderTargets(uint32_t NumSimultaneousRenderTargets, const FRHIRenderTargetView* NewRenderTargetsRHI, const FRHIDepthRenderTargetView* NewDepthStencilTargetRHI)
	{
		assert(NumSimultaneousRenderTargets <= c_MaxRenderTargets);

		FMemory::Memset(m_Device->PendingState.RenderTargets, 0, sizeof(m_Device->PendingState.RenderTargets));
		FMemory::Memset(m_Device->PendingState.RenderTargetMipmapLevels, 0, sizeof(m_Device->PendingState.RenderTargetMipmapLevels));
		FMemory::Memset(m_Device->PendingState.RenderTargetArrayIndex, 0, sizeof(m_Device->PendingState.RenderTargetArrayIndex));
		m_Device->PendingState.FirstNonzeroRenderTarget = -1;
        m_Device->PendingState.NumColorRenderTargets = NumSimultaneousRenderTargets;
		for (int32_t RenderTargetIndex = NumSimultaneousRenderTargets - 1; RenderTargetIndex >= 0; --RenderTargetIndex)
		{
			m_Device->PendingState.RenderTargets[RenderTargetIndex] = static_cast<Texture*>(NewRenderTargetsRHI[RenderTargetIndex].Texture);
			m_Device->PendingState.RenderTargetMipmapLevels[RenderTargetIndex] = NewRenderTargetsRHI[RenderTargetIndex].MipIndex;
			m_Device->PendingState.RenderTargetArrayIndex[RenderTargetIndex] = NewRenderTargetsRHI[RenderTargetIndex].ArraySliceIndex;

			if (m_Device->PendingState.RenderTargets[RenderTargetIndex])
			{
				m_Device->PendingState.FirstNonzeroRenderTarget = (int32_t)RenderTargetIndex;
			}
		}

		Texture* NewDepthStencilRT = static_cast<Texture*>(NewDepthStencilTargetRHI ? NewDepthStencilTargetRHI->Texture : nullptr);

		m_Device->PendingState.DepthStencil = NewDepthStencilRT;
		m_Device->PendingState.StencilStoreAction = NewDepthStencilTargetRHI ? NewDepthStencilTargetRHI->GetStencilStoreAction() : ERenderTargetStoreAction::ENoAction;
		m_Device->PendingState.DepthTargetWidth = NewDepthStencilRT ? NewDepthStencilRT->getDesc().width : 0u;
		m_Device->PendingState.DepthTargetHeight = NewDepthStencilRT ? NewDepthStencilRT->getDesc().height : 0u;

		if (m_Device->PendingState.FirstNonzeroRenderTarget == -1 && !m_Device->PendingState.DepthStencil)
		{
			// Special case - invalid setup, but sometimes performed by the engine

			m_Device->PendingState.Framebuffer = 0;
			m_Device->PendingState.bFramebufferSetupInvalid = true;
			return;
		}

		m_Device->PendingState.Framebuffer = _getOpenGLFramebuffer(NumSimultaneousRenderTargets, m_Device->PendingState.RenderTargets, m_Device->PendingState.RenderTargetArrayIndex, m_Device->PendingState.RenderTargetMipmapLevels, m_Device->PendingState.DepthStencil, m_Device->PendingState.NumRenderingSamples);
		m_Device->PendingState.bFramebufferSetupInvalid = false;

		if (m_Device->PendingState.FirstNonzeroRenderTarget != -1)
		{
			// Set viewport size to new render target size.
			m_Device->PendingState.Viewport.minX = 0;
			m_Device->PendingState.Viewport.minY = 0;

			const TextureDesc& Desc = NewRenderTargetsRHI[m_Device->PendingState.FirstNonzeroRenderTarget].Texture->getDesc();

			uint32_t MipIndex = NewRenderTargetsRHI[m_Device->PendingState.FirstNonzeroRenderTarget].MipIndex;
			uint32_t Width = math::max<uint32_t>(1, Desc.width >> MipIndex);
			uint32_t Height = math::max<uint32_t>(1, Desc.height >> MipIndex);

			m_Device->PendingState.Viewport.maxX = m_Device->PendingState.RenderTargetWidth = Width;
			m_Device->PendingState.Viewport.maxY = m_Device->PendingState.RenderTargetHeight = Height;
		}
		else if (NewDepthStencilTargetRHI)
		{
			// Set viewport size to new depth target size.
			m_Device->PendingState.Viewport.minX = 0;
			m_Device->PendingState.Viewport.minY = 0;
			m_Device->PendingState.Viewport.maxX = NewDepthStencilTargetRHI->Texture->getDesc().width;
			m_Device->PendingState.Viewport.maxY = NewDepthStencilTargetRHI->Texture->getDesc().height;
		}
	}
	void CommandList::_setRenderTargets(FramebufferHandle framebuffer)
	{
		//assert(framebuffer->getDesc().getRenderTargetCnt() <= c_MaxRenderTargets);

		//FMemory::Memset(m_Device->PendingState.RenderTargets, 0, sizeof(m_Device->PendingState.RenderTargets));
		//FMemory::Memset(m_Device->PendingState.RenderTargetMipmapLevels, 0, sizeof(m_Device->PendingState.RenderTargetMipmapLevels));
		//FMemory::Memset(m_Device->PendingState.RenderTargetArrayIndex, 0, sizeof(m_Device->PendingState.RenderTargetArrayIndex));
		//m_Device->PendingState.FirstNonzeroRenderTarget = -1;

		//for (int32_t RenderTargetIndex = framebuffer->getDesc().colorAttachments.size() - 1; RenderTargetIndex >= 0; --RenderTargetIndex)
		//{
		//	m_Device->PendingState.RenderTargets[RenderTargetIndex] = static_cast<Texture*>(framebuffer->getDesc().colorAttachments[RenderTargetIndex].texture);
		//	m_Device->PendingState.RenderTargetMipmapLevels[RenderTargetIndex] = framebuffer->getDesc().colorAttachments[RenderTargetIndex].texture->getDesc().mipLevelsCnt;
		//	m_Device->PendingState.RenderTargetArrayIndex[RenderTargetIndex] = framebuffer->getDesc().colorAttachments[RenderTargetIndex].texture->getDesc().arraySize;;// NewRenderTargetsRHI[RenderTargetIndex].ArraySliceIndex;

		//	if (m_Device->PendingState.RenderTargets[RenderTargetIndex])
		//	{
		//		m_Device->PendingState.FirstNonzeroRenderTarget = (int32_t)RenderTargetIndex;
		//	}
		//}

		//Texture* NewDepthStencilRT = static_cast<Texture*>(framebuffer->getDesc().depthAttachment.texture);

		//m_Device->PendingState.DepthStencil = NewDepthStencilRT;
		//m_Device->PendingState.StencilStoreAction = framebuffer->getDesc().depthAttachment.texture ? framebuffer->getDesc().depthAttachment.texture->GetStencilStoreAction() : ERenderTargetStoreAction::ENoAction;
		//m_Device->PendingState.DepthTargetWidth = NewDepthStencilRT ? NewDepthStencilRT->getDesc().width : 0u;
		//m_Device->PendingState.DepthTargetHeight = NewDepthStencilRT ? NewDepthStencilRT->getDesc().height : 0u;

		//if (m_Device->PendingState.FirstNonzeroRenderTarget == -1 && !m_Device->PendingState.DepthStencil)
		//{
		//	// Special case - invalid setup, but sometimes performed by the engine

		//	m_Device->PendingState.Framebuffer = 0;
		//	m_Device->PendingState.bFramebufferSetupInvalid = true;
		//	return;
		//}

		//m_Device->PendingState.Framebuffer = static_cast<Framebuffer*>(framebuffer.Get())->GetRenderID(); //_getOpenGLFramebuffer(NumSimultaneousRenderTargets, m_Device->PendingState.RenderTargets, m_Device->PendingState.RenderTargetArrayIndex, m_Device->PendingState.RenderTargetMipmapLevels, m_Device->PendingState.DepthStencil, m_Device->PendingState.NumRenderingSamples);
		//m_Device->PendingState.bFramebufferSetupInvalid = false;

		//if (m_Device->PendingState.FirstNonzeroRenderTarget != -1)
		//{
		//	// Set viewport size to new render target size.
		//	m_Device->PendingState.Viewport.minX = 0;
		//	m_Device->PendingState.Viewport.minY = 0;

		//	const TextureDesc& Desc = NewRenderTargetsRHI[m_Device->PendingState.FirstNonzeroRenderTarget].Texture->getDesc();

		//	uint32_t MipIndex = NewRenderTargetsRHI[m_Device->PendingState.FirstNonzeroRenderTarget].MipIndex;
		//	uint32_t Width = math::max<uint32_t>(1, Desc.width >> MipIndex);
		//	uint32_t Height = math::max<uint32_t>(1, Desc.height >> MipIndex);

		//	m_Device->PendingState.Viewport.maxX = m_Device->PendingState.RenderTargetWidth = Width;
		//	m_Device->PendingState.Viewport.maxY = m_Device->PendingState.RenderTargetHeight = Height;
		//}
		//else if (NewDepthStencilTargetRHI)
		//{
		//	// Set viewport size to new depth target size.
		//	m_Device->PendingState.Viewport.minX = 0;
		//	m_Device->PendingState.Viewport.minY = 0;
		//	m_Device->PendingState.Viewport.maxX = NewDepthStencilTargetRHI->Texture->getDesc().width;
		//	m_Device->PendingState.Viewport.maxY = NewDepthStencilTargetRHI->Texture->getDesc().height;
		//}
	}
	void CommandList::_setRenderTargetsAndClear(const FRHISetRenderTargetsInfo& RenderTargetsInfo)
	{
		this->_setRenderTargets(RenderTargetsInfo.NumColorRenderTargets,
			RenderTargetsInfo.ColorRenderTarget,
			&RenderTargetsInfo.DepthStencilRenderTarget);

		/**
		 * Convert all load action from NoAction to Clear for tiled GPU on OpenGL platform to avoid an unnecessary load action.
		 */
		GE_ERROR_JUDGE();

		bool bIsTiledGPU = hasTiledGPU();

		bool bClearColor = RenderTargetsInfo.bClearColor;
		bool bClearStencil = RenderTargetsInfo.bClearStencil;
		bool bClearDepth = RenderTargetsInfo.bClearDepth;

		Color ClearColors[c_MaxRenderTargets];
		float DepthClear = 1.0;
		uint32_t StencilClear = 1;
		//TODO::
		for (int32_t i = 0; i < RenderTargetsInfo.NumColorRenderTargets; ++i)
		{
			if (RenderTargetsInfo.ColorRenderTarget[i].Texture != nullptr)
			{
				//const FClearValueBinding& ClearValue = RenderTargetsInfo.ColorRenderTarget[i].Texture->GetClearBinding();

				if (bIsTiledGPU)
				{
					bClearColor |= RenderTargetsInfo.ColorRenderTarget[i].LoadAction == ERenderTargetLoadAction::ENoAction;

					ClearColors[i] = Color(0.0f, 0.0f, 0.0f, 1.0f);// ClearValue.ColorBinding == EClearBinding::EColorBound ? ClearValue.GetClearColor() : FLinearColor::Black;
				}
				else if (bClearColor)
				{
					//checkf(ClearValue.ColorBinding == EClearBinding::EColorBound, TEXT("Texture: %s does not have a color bound for fast clears"), *RenderTargetsInfo.ColorRenderTarget[i].Texture->GetName().GetPlainNameString());

					ClearColors[i] = Color(0.0f, 0.0f , 0.0f ,1.0f);// ClearValue.GetClearColor();
				}
			}
		}

		if (RenderTargetsInfo.DepthStencilRenderTarget.Texture != nullptr)
		{
			//const FClearValueBinding& ClearValue = RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetClearBinding();

			if (bIsTiledGPU)
			{
				bClearStencil |= RenderTargetsInfo.DepthStencilRenderTarget.StencilLoadAction == ERenderTargetLoadAction::ENoAction;

				bClearDepth |= RenderTargetsInfo.DepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::ENoAction;

				/*if (ClearValue.ColorBinding == EClearBinding::EDepthStencilBound)
				{
					ClearValue.GetDepthStencil(DepthClear, StencilClear);
				}*/
			}
			else if (bClearDepth || bClearStencil)
			{
				//checkf(ClearValue.ColorBinding == EClearBinding::EDepthStencilBound, TEXT("Texture: %s does not have a DS value bound for fast clears"), *RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetName().GetPlainNameString());

				//ClearValue.GetDepthStencil(DepthClear, StencilClear);
			}
		}

		if (bClearColor || bClearStencil || bClearDepth)
		{
			this->clearMRT(bClearColor, RenderTargetsInfo.NumColorRenderTargets, ClearColors, bClearDepth, DepthClear, bClearStencil, StencilClear);
		}
	}
	void CommandList::beginDrawingViewport(RHIViewport* viewport, ITexture* renderTarget)
	{
		OpenGLViewport* Viewport = static_cast<OpenGLViewport*>(viewport);

		//SCOPE_CYCLE_COUNTER(STAT_OpenGLPresentTime);

		GE_ASSERT(!m_Device->DrawingViewport, "DrawingViewport is not nullptr");
		m_Device->DrawingViewport = Viewport;

		m_Device->bRevertToSharedContextAfterDrawingViewport = false;
		EOpenGLCurrentContext CurrentContext = PlatformOpenGLCurrentContext(m_Device->m_Context->PlatformDevice);
		if (CurrentContext != CONTEXT_Rendering)
		{
			GE_ASSERT(CurrentContext == CONTEXT_Shared, "invalid current context");
			//check(!bIsRenderingContextAcquired || !GUseThreadedRendering);

			m_Device->bRevertToSharedContextAfterDrawingViewport = true;
			PlatformRenderingContextSetup(m_Device->m_Context->PlatformDevice);
		}

		// Set the render target and viewport.
		if (renderTarget)
		{
			FRHIRenderTargetView RTV(renderTarget, ERenderTargetLoadAction::ELoad);
			_setRenderTargets(1, &RTV, nullptr);
		}
		else
		{
			FRHIRenderTargetView RTV(m_Device->DrawingViewport->GetBackBuffer(), ERenderTargetLoadAction::ELoad);
			_setRenderTargets(1, &RTV, nullptr);
		}

	/*	if (IsValidRef(CustomPresent))
		{
			CustomPresent->BeginDrawing();
		}*/
	}

	// Raster operations.
	static inline void ClearCurrentDepthStencilWithCurrentScissor(int8_t ClearType, float Depth, uint32_t Stencil)
	{
		switch (ClearType)
		{
		case CT_DepthStencil:	// Clear depth and stencil
			FOpenGL::ClearBufferfi(GL_DEPTH_STENCIL, 0, Depth, Stencil);
			break;

		case CT_Stencil:	// Clear stencil only
			FOpenGL::ClearBufferiv(GL_STENCIL, 0, (const GLint*)&Stencil);
			break;

		case CT_Depth:	// Clear depth only
			FOpenGL::ClearBufferfv(GL_DEPTH, 0, &Depth);
			break;

		default:
			break;	// impossible anyway
		}
		GE_ERROR_JUDGE();

	}
	void CommandList::_clearCurrentFramebufferWithCurrentScissor(FOpenGLContextState& ContextState, int8_t ClearType, int32_t NumClearColors, const Color* ClearColorArray, float Depth, uint32_t Stencil)
	{
		//VERIFY_GL_SCOPE();

		// Clear color buffers
		if (ClearType & CT_Color)
		{
			for (int32_t ColorIndex = 0; ColorIndex < NumClearColors; ++ColorIndex)
			{
				FOpenGL::ClearBufferfv(GL_COLOR, ColorIndex, (const GLfloat*)&ClearColorArray[ColorIndex]);
				GE_ERROR_JUDGE();

			}
		}

		if (ClearType & CT_DepthStencil)
		{
			ClearCurrentDepthStencilWithCurrentScissor(ClearType & CT_DepthStencil, Depth, Stencil);
		}
	}
	void CommandList::clearMRT(bool bClearColor, int32_t NumClearColors, const Color* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32_t Stencil)
	{
		RHIRect ExcludeRect;
		//VERIFY_GL_SCOPE();

		GE_ASSERT((GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM5) || !m_Device->PendingState.bFramebufferSetupInvalid, "invalid framebufffer");

		if (bClearColor)
		{
			// This is copied from DirectX11 code - apparently there's a silent assumption that there can be no valid render target set at index higher than an invalid one.
			int32_t NumActiveRenderTargets = 0;
			for (int32_t TargetIndex = 0; TargetIndex < c_MaxRenderTargets; TargetIndex++)
			{
				if (m_Device->PendingState.RenderTargets[TargetIndex] != 0)
				{
					NumActiveRenderTargets++;
				}
				else
				{
					break;
				}
			}

			// Must specify enough clear colors for all active RTs
			GE_ASSERT(NumClearColors >= NumActiveRenderTargets);
		}

		// Remember cached scissor state, and set one to cover viewport
		RHIRect PrevScissor = m_Device->PendingState.Scissor;
		bool bPrevScissorEnabled = m_Device->PendingState.bScissorEnabled;

		bool bScissorChanged = false;
		//GPUProfilingData.RegisterGPUWork(0);
		FOpenGLContextState& ContextState = m_Device->GetContextStateForCurrentContext();
		m_Device->BindPendingFramebuffer(ContextState);

		if (bPrevScissorEnabled 
			|| m_Device->PendingState.Viewport.minX != 0 
			|| m_Device->PendingState.Viewport.minY != 0 
			|| m_Device->PendingState.Viewport.maxX != m_Device->PendingState.RenderTargetWidth 
			|| m_Device->PendingState.Viewport.maxY != m_Device->PendingState.RenderTargetHeight)
		{
			setScissorRect(false, 0, 0, 0, 0);
			bScissorChanged = true;
		}

		// Always update in case there are uncommitted changes to disable scissor
		m_Device->UpdateScissorRectInOpenGLContext(ContextState);

		int8_t ClearType = CT_None;

		// Prepare color buffer masks, if applicable
		if (bClearColor)
		{
			ClearType |= CT_Color;

			for (int32_t ColorIndex = 0; ColorIndex < NumClearColors; ++ColorIndex)
			{
				if (!ContextState.BlendState.targets[ColorIndex].colorWriteMaskRed() ||
					!ContextState.BlendState.targets[ColorIndex].colorWriteMaskGreen() ||
					!ContextState.BlendState.targets[ColorIndex].colorWriteMaskGreen() ||
					!ContextState.BlendState.targets[ColorIndex].colorWriteMaskAlpha())
				{
					FOpenGL::ColorMaskIndexed(ColorIndex, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
					ContextState.BlendState.targets[ColorIndex].setColorWriteMask(ColorMask::Red);
					ContextState.BlendState.targets[ColorIndex].setColorWriteMask(ColorMask::Green);
					ContextState.BlendState.targets[ColorIndex].setColorWriteMask(ColorMask::Blue);
					ContextState.BlendState.targets[ColorIndex].setColorWriteMask(ColorMask::Alpha);
				}
			}
		}

		// Prepare depth mask, if applicable
		if (bClearDepth && m_Device->PendingState.DepthStencil)
		{
			ClearType |= CT_Depth;

			if (!ContextState.DepthStencilState.depthWriteEnable)
			{
				glDepthMask(GL_TRUE);
				ContextState.DepthStencilState.depthWriteEnable = true;
			}
		}

		// Prepare stencil mask, if applicable
		if (bClearStencil && m_Device->PendingState.DepthStencil)
		{
			ClearType |= CT_Stencil;

			if (ContextState.DepthStencilState.stencilWriteMask != 0xFFFFFFFF)
			{
				glStencilMask(0xFFFFFFFF);
				ContextState.DepthStencilState.stencilWriteMask = 0xFFFFFFFF;
			}
		}

		// Just one clear
		_clearCurrentFramebufferWithCurrentScissor(ContextState, ClearType, NumClearColors, ColorArray, Depth, Stencil);

		if (bScissorChanged)
		{
			// Change it back
			setScissorRect(bPrevScissorEnabled, PrevScissor.minX, PrevScissor.minY, PrevScissor.maxX, PrevScissor.maxY);
		}
	}

	void CommandList::endDrawingViewport(RHIViewport* viewport, bool bPresent, bool bLockToVsync)
	{

		OpenGLViewport* Viewport = static_cast<OpenGLViewport*>(viewport);

		//SCOPE_CYCLE_COUNTER(STAT_OpenGLPresentTime);
		{
			//FRenderThreadIdleScope IdleScope(ERenderThreadIdleTypes::WaitingForGPUPresent);

			assert(m_Device->DrawingViewport == Viewport);

			Texture* BackBuffer = static_cast<Texture*>(Viewport->GetBackBuffer().Get());

			FOpenGLContextState& ContextState = m_Device->GetContextStateForCurrentContext();

			if (ContextState.bScissorEnabled)
			{
				ContextState.bScissorEnabled = false;
				glDisable(GL_SCISSOR_TEST);
			}

			bool bNeedFinishFrame = PlatformBlitToViewport(m_Device->m_Context->PlatformDevice,
				*Viewport,
				BackBuffer->getDesc().width,
				BackBuffer->getDesc().height,
				bPresent,
				bLockToVsync
			);

			// Always consider the Framebuffer in the rendering context dirty after the blit
			m_Device->RenderingContextState.Framebuffer = -1;

			m_Device->DrawingViewport = NULL;

			if (bNeedFinishFrame)
			{
				static const auto CFinishFrameVar = false;// IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FinishCurrentFrame"));
				//if (!CFinishFrameVar->GetValueOnRenderThread())
				if (!CFinishFrameVar)
				{
					// Wait for the GPU to finish rendering the previous frame before finishing this frame.
					Viewport->WaitForFrameEventCompletion();
					Viewport->IssueFrameEvent();
				}
				else
				{
					// Finish current frame immediately to reduce latency
					Viewport->IssueFrameEvent();
					Viewport->WaitForFrameEventCompletion();
				}
			}

			// If the input latency timer has been triggered, block until the GPU is completely
			// finished displaying this frame and calculate the delta time.
	/*		if (GInputLatencyTimer.RenderThreadTrigger)
			{
				Viewport->WaitForFrameEventCompletion();
				uint32_t EndTime = FPlatformTime::Cycles();
				GInputLatencyTimer.DeltaTime = EndTime - GInputLatencyTimer.StartTime;
				GInputLatencyTimer.RenderThreadTrigger = false;
			}*/

			if (m_Device->bRevertToSharedContextAfterDrawingViewport)
			{
				PlatformSharedContextSetup(m_Device->m_Context->PlatformDevice);
				m_Device->bRevertToSharedContextAfterDrawingViewport = false;
			}
		}





	}

	static void ConditionallyAllocateRenderbufferStorage(Texture& RenderTarget)
	{
		/*if (RenderTarget.bMultisampleRenderbuffer &&
			RenderTarget.GetAllocatedStorageForMip(0, 0) == false)
		{
			check(RenderTarget.IsMultisampled());
			check(RenderTarget.Target == GL_RENDERBUFFER);

			GLuint TextureID = RenderTarget.GetRawResourceName();
			const FRHITextureDesc& Desc = RenderTarget.GetDesc();
			const FOpenGLTextureFormat& GLFormat = GOpenGLTextureFormats[Desc.Format];
			const bool bSRGB = EnumHasAnyFlags(Desc.Flags, TexCreate_SRGB);

			glBindRenderbuffer(GL_RENDERBUFFER, TextureID);
			FOpenGL::RenderbufferStorageMultisample(GL_RENDERBUFFER, Desc.NumSamples, GLFormat.InternalFormat[bSRGB], Desc.Extent.X, Desc.Extent.Y);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			RenderTarget.SetAllocatedStorage(true);*/
		//}
	}
	
	GLuint CommandList::_getOpenGLFramebuffer(uint32_t NumSimultaneousRenderTargets, Texture** RenderTargets, const uint32_t* ArrayIndices, const uint32_t* MipmapLevels, Texture* DepthStencilTarget)
	{
		const int32_t NumRenderingSamples = 1;
		return _getOpenGLFramebuffer(NumSimultaneousRenderTargets, RenderTargets, ArrayIndices, MipmapLevels, DepthStencilTarget, NumRenderingSamples);
	}

	GLuint CommandList::_getOpenGLFramebuffer(uint32_t NumSimultaneousRenderTargets, Texture** RenderTargets, const uint32_t* ArrayIndices, const uint32_t* MipmapLevels, Texture* DepthStencilTarget, int32_t NumRenderingSamples)
	{
		const bool bRenderTargetsDefined = (RenderTargets != nullptr) && RenderTargets[0];

		// Check for rendering to screen back buffer.
		if (NumSimultaneousRenderTargets > 0 && bRenderTargetsDefined && RenderTargets[0]->GetRendererID() == GL_NONE)
		{
			// Use the default framebuffer (screen back/depth buffer)
			return GL_NONE;
		}
        //GE_ERROR_JUDGE_EGL();

        GE_ERROR_JUDGE();
		// Not found. Preparing new one.
		GLuint Framebuffer;
		glGenFramebuffers(1, &Framebuffer);
		GE_ERROR_JUDGE();

		//VERIFY_GL(glGenFramebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		//VERIFY_GL(glBindFramebuffer)
		GE_ERROR_JUDGE();

		static const bool CVarMobileMultiView = Configuration::MobileMultiView;

		// Allocate mobile multi-view frame buffer if enabled and supported.
		// Multi-view doesn't support read buffers, explicitly disable and only bind GL_DRAW_FRAMEBUFFER
		// TODO: We can't reliably use packed depth stencil?
		const bool bValidMultiViewDepthTarget = !DepthStencilTarget || DepthStencilTarget->getDesc().dimension == TextureDimension::Texture2DArray;
		const bool bUsingArrayTextures = (bRenderTargetsDefined) ? (RenderTargets[0]->getDesc().dimension == TextureDimension::Texture2DArray && bValidMultiViewDepthTarget) : false;
		const bool bMultiViewCVar = CVarMobileMultiView;

		if (bUsingArrayTextures && FOpenGL::SupportsMobileMultiView() && bMultiViewCVar)
		{
			Texture* const RenderTarget = RenderTargets[0];
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, Framebuffer);

			if (NumRenderingSamples > 1)
			{
				FOpenGL::FramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RenderTarget->GetRendererID(), 0, NumRenderingSamples, 0, 2);
				//VERIFY_GL(glFramebufferTextureMultisampleMultiviewOVR);
				GE_ERROR_JUDGE();

				if (DepthStencilTarget)
				{
					FOpenGL::FramebufferTextureMultisampleMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthStencilTarget->GetRendererID(), 0, NumRenderingSamples, 0, 2);
					//VERIFY_GL(glFramebufferTextureMultisampleMultiviewOVR);
				}
			}
			else
			{
				FOpenGL::FramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RenderTarget->GetRendererID(), 0, 0, 2);
				//VERIFY_GL(glFramebufferTextureMultiviewOVR);

				if (DepthStencilTarget)
				{
					FOpenGL::FramebufferTextureMultiviewOVR(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, DepthStencilTarget->GetRendererID(), 0, 0, 2);
					//VERIFY_GL(glFramebufferTextureMultiviewOVR);
				}
			}
			GE_ERROR_JUDGE();

			FOpenGL::CheckFrameBuffer();
			GE_ERROR_JUDGE();

			FOpenGL::ReadBuffer(GL_NONE);
			FOpenGL::DrawBuffer(GL_COLOR_ATTACHMENT0);
			GE_ERROR_JUDGE();

			//GetOpenGLFramebufferCache().Add(FOpenGLFramebufferKey(NumSimultaneousRenderTargets, RenderTargets, ArrayIndices, MipmapLevels, DepthStencilTarget, NumRenderingSamples, PlatformOpenGLCurrentContext(PlatformDevice)), Framebuffer + 1);

			return Framebuffer;
		}
		int32_t FirstNonzeroRenderTarget = -1;
		for (int32_t RenderTargetIndex = NumSimultaneousRenderTargets - 1; RenderTargetIndex >= 0 && bRenderTargetsDefined; --RenderTargetIndex)
		{
			Texture* RenderTarget = RenderTargets[RenderTargetIndex];
			if (!RenderTarget)
			{
				continue;
			}

			if (ArrayIndices == NULL || ArrayIndices[RenderTargetIndex] == -1)
			{
				// If no index was specified, bind the entire object, rather than a slice
				GLenum dim = OpenGLUtil::convertTextureDimension(RenderTarget->getDesc().dimension);
				switch (dim)
				{
				case GL_RENDERBUFFER:
				{
					//todo:: renderbuffer 分开处理
					// lazily allocate render buffer storage in case it's multisampled
					ConditionallyAllocateRenderbufferStorage(*RenderTarget);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetRendererID());
					break;
				}
				case GL_TEXTURE_2D:
				case GL_TEXTURE_EXTERNAL_OES:
				case GL_TEXTURE_2D_MULTISAMPLE:
				{
					if (NumRenderingSamples > 1)
					{
						// GL_EXT_multisampled_render_to_texture
						FOpenGL::FramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, dim, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex], NumRenderingSamples);
					}
					else
					{
						FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, dim, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex]);
					}
					break;
				}
				case GL_TEXTURE_3D:
				case GL_TEXTURE_2D_ARRAY:
				case GL_TEXTURE_CUBE_MAP:
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					FOpenGL::FramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex]);
					break;
				default:
					FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetRendererID());
					break;
				}
			}
			else
			{
				GLenum dim = OpenGLUtil::convertTextureDimension(RenderTarget->getDesc().dimension);

				// Bind just one slice of the object
				switch (dim)
				{
				case GL_RENDERBUFFER:
				{
					assert(ArrayIndices[RenderTargetIndex] == 0);
					// lazily allocate render buffer storage in case it's multisampled
					ConditionallyAllocateRenderbufferStorage(*RenderTarget);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetRendererID());
					break;
				}
				case GL_TEXTURE_2D:
				case GL_TEXTURE_EXTERNAL_OES:
				case GL_TEXTURE_2D_MULTISAMPLE:
				{
					assert(ArrayIndices[RenderTargetIndex] == 0);
					if (NumRenderingSamples > 1)
					{
						// GL_EXT_multisampled_render_to_texture
						FOpenGL::FramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, dim, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex], NumRenderingSamples);
					}
					else
					{
						FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, dim, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex]);
					}
					break;
				}
				case GL_TEXTURE_3D:
					FOpenGL::FramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, dim, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex], ArrayIndices[RenderTargetIndex]);
					break;
				case GL_TEXTURE_CUBE_MAP:
					assert(ArrayIndices[RenderTargetIndex] < 6);
					FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + ArrayIndices[RenderTargetIndex], RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex]);
					break;
				case GL_TEXTURE_2D_ARRAY:
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					FOpenGL::FramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->GetRendererID(), MipmapLevels[RenderTargetIndex], ArrayIndices[RenderTargetIndex]);
					break;
				default:
					assert(ArrayIndices[RenderTargetIndex] == 0);
					FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetRendererID());
					break;
				}
			}
			FirstNonzeroRenderTarget = RenderTargetIndex;
		}

		if (DepthStencilTarget)
		{
			GLenum dim = OpenGLUtil::convertTextureDimension(DepthStencilTarget->getDesc().dimension);

			switch (dim)
			{
			case GL_TEXTURE_2D:
			case GL_TEXTURE_EXTERNAL_OES:
			case GL_TEXTURE_2D_MULTISAMPLE:
			{
				FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, dim, DepthStencilTarget->GetRendererID(), 0);
				//				FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, dim, DepthStencilTarget->GetRendererID(), 0);

				break;
			}
			case GL_RENDERBUFFER:
			{
				// lazily allocate render buffer storage in case it's multisampled
				ConditionallyAllocateRenderbufferStorage(*DepthStencilTarget);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthStencilTarget->GetRendererID());
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilTarget->GetRendererID());
				//VERIFY_GL(glFramebufferRenderbuffer);
				break;
			}
			case GL_TEXTURE_3D:
			case GL_TEXTURE_2D_ARRAY:
			case GL_TEXTURE_CUBE_MAP:
			case GL_TEXTURE_CUBE_MAP_ARRAY:
				//TODO::
				//FOpenGL::FramebufferTexture(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, DepthStencilTarget->GetRendererID(), 0);
				break;
			default:
				//FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, GL_RENDERBUFFER, DepthStencilTarget->GetRendererID());
				break;
			}
		}

		if (FirstNonzeroRenderTarget != -1)
		{
			FOpenGL::ReadBuffer(GL_COLOR_ATTACHMENT0 + FirstNonzeroRenderTarget);
			FOpenGL::DrawBuffer(GL_COLOR_ATTACHMENT0 + FirstNonzeroRenderTarget);
		}
		else
		{
			FOpenGL::ReadBuffer(GL_NONE);
			FOpenGL::DrawBuffer(GL_NONE);
		}

		//  End frame can bind NULL / NULL 
		//  An FBO with no attachments is framebuffer incomplete (INCOMPLETE_MISSING_ATTACHMENT)
		//  In this case just delete the FBO and map in the default
		//  In GL 4.x, NULL/NULL is valid and can be done =by specifying a default width/height
		if (FirstNonzeroRenderTarget == -1 && !DepthStencilTarget)
		{
			glDeleteFramebuffers(1, &Framebuffer);
			Framebuffer = 0;
			glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		}

		FOpenGL::CheckFrameBuffer();

		//GetOpenGLFramebufferCache().Add(FOpenGLFramebufferKey(NumSimultaneousRenderTargets, RenderTargets, ArrayIndices, MipmapLevels, DepthStencilTarget, NumRenderingSamples, PlatformOpenGLCurrentContext(PlatformDevice)), Framebuffer + 1);

		return Framebuffer;
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
		//TODO:: ͬʱ���ö�� viewport ��Ҫ��glViewportArrayv
		setViewport(state.viewport.viewports[0].minX, state.viewport.viewports[0].minY, state.viewport.viewports[0].minZ, 
			state.viewport.viewports[0].maxX, state.viewport.viewports[0].maxY, state.viewport.viewports[0].maxZ);

		setScissorRect(true, state.viewport.scissorRects[0].minX, state.viewport.scissorRects[0].minY,
			state.viewport.scissorRects[0].maxX, state.viewport.scissorRects[0].maxY);

	


		setBoundShaderState(
			m_Device->RHICreateBoundShaderState_Internal(
				state.pipeline->desc.inputLayout.Get(),
				state.pipeline->desc.VS.Get(),
				state.pipeline->desc.PS.Get(),
				state.pipeline->desc.GS.Get(),
				state.bindings,
				state.pipeline->desc.bFromPSOFileCache
			)
		);

		setDepthStencilState(&state.pipeline->desc.depthStencilState);
		setRasterizerState(&state.pipeline->desc.rasterState);
		setBlendState(&state.pipeline->desc.blendState);
		setIndexBufferState(state.indexBuffer.buffer);
		setVertexBufferState(state.vertexBuffers);//?? buffer 呢
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
		/*m_Device->CommitGraphicsResourceTables();
		m_Device->SetupTexturesForDraw(ContextState);
		m_Device->SetupUAVsForDraw(ContextState);
		m_Device->CommitNonComputeShaderConstants();*/
		m_Device->CommitDescriptorSets(ContextState);

		m_Device->CachedBindElementArrayBuffer(ContextState);

        int NumInstances = args.instanceCount;

		uint32_t VertexCount = args.vertexCount;//GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);
        uint32_t BaseVertexIndex = args.startVertexLocation;

		m_Device->SetupVertexArrays(ContextState, BaseVertexIndex, m_Device->PendingState.Streams, NUM_OPENGL_VERTEX_STREAMS, VertexCount);

		GLenum DrawMode = GL_TRIANGLES;
		GLsizei NumElements = args.vertexCount;

       // FindPrimitiveType(PrimitiveType, NumPrimitives, DrawMode, NumElements);

		//GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
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
		//GOpenGLKickHint.OnDrawCall(ContextState);
	}
	void CommandList::drawIndexed(const DrawArguments& args)
	{


		FOpenGLContextState& ContextState = m_Device->GetContextStateForCurrentContext();
		//{
			m_Device->BindPendingFramebuffer(ContextState);
			GE_ERROR_JUDGE();

			m_Device->SetPendingBlendStateForActiveRenderTargets(ContextState);
			GE_ERROR_JUDGE();

			m_Device->UpdateViewportInOpenGLContext(ContextState);
			GE_ERROR_JUDGE();

			m_Device->UpdateScissorRectInOpenGLContext(ContextState);
			GE_ERROR_JUDGE();

			m_Device->UpdateRasterizerStateInOpenGLContext(ContextState);
			GE_ERROR_JUDGE();

			m_Device->UpdateDepthStencilStateInOpenGLContext(ContextState);
			GE_ERROR_JUDGE();

			m_Device->BindPendingShaderState(ContextState);
			GE_ERROR_JUDGE();

			m_Device->CommitDescriptorSets(ContextState);
			GE_ERROR_JUDGE();

		//{
		//	//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_CommitGraphicsResourceTables);
		//	m_Device->CommitGraphicsResourceTables();
		//}
		//{
		//	//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupTexturesForDraw);
		//	m_Device->SetupTexturesForDraw(ContextState);
		//}
		//{
		//	//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupUAVsForDraw);
		//	m_Device->SetupUAVsForDraw(ContextState);
		//}
		//{
		//	//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_CommitNonComputeShaderConstants);
		//	m_Device->CommitNonComputeShaderConstants();
		//}
		//{
			m_Device->BindContextVAO();
			GE_ERROR_JUDGE();


			m_Device->CachedBindElementArrayBuffer(ContextState);
			GE_ERROR_JUDGE();
			

			m_Device->SetupVertexArrays(ContextState, args.startIndexLocation, m_Device->PendingState.Streams, NUM_OPENGL_VERTEX_STREAMS, args.vertexCount);
			GE_ERROR_JUDGE();

			//}

		GLenum DrawMode = GL_TRIANGLES;
		GLsizei NumElements = args.vertexCount;
		/*FindPrimitiveType(PrimitiveType, NumPrimitives, DrawMode, NumElements);*/
		GLenum IndexType = GL_UNSIGNED_INT;// GL_UNSIGNED_INT;// IndexBuffer->GetStride() == sizeof(uint32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		//StartIndex *= IndexBuffer->GetStride() == sizeof(uint32) ? sizeof(uint32) : sizeof(uint16);
		uint32_t StartIndex = 0;// args.startIndexLocation* args.indexBufferStride;
		//uint32_t StartIndex = 0;// args.startIndexLocation* args.indexBufferStride;
		//GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, NumElements * NumInstances);
		if (args.instanceCount > 1)
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			//checkf(FirstInstance == 0, TEXT("FirstInstance is currently unsupported on this RHI"));
			FOpenGL::DrawElementsInstanced(DrawMode, NumElements, IndexType, (void*)(uint64_t)(StartIndex), args.instanceCount);
			GE_ERROR_JUDGE();

		}
		else
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			if (FOpenGL::SupportsDrawIndexOffset())
			{
				GLint program;
				glGetIntegerv(GL_CURRENT_PROGRAM, &program);
				if (program == 0) {
					assert(0);
				}
			//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
				m_Device->BindContextVAO();
				assert(glIsBuffer(m_Device->PendingState.ibo));
			
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Device->PendingState.ibo);
				GE_ERROR_JUDGE();
				int siz = sizeof(args.indices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, args.indices.size()* sizeof(uint32_t), args.indices.data(), GL_STATIC_DRAW);
				GE_ERROR_JUDGE();

				// 2. 检查索引缓冲区大小
				GLint iboSize;
				glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &iboSize);
				if (StartIndex + NumElements * sizeof(GL_UNSIGNED_INT) > iboSize) {
					std::cerr << "索引缓冲区越界！" << std::endl;
				}

				//HGLRC currentContext = wglGetCurrentContext();
				GE_ERROR_JUDGE();
				//FOpenGL::DrawRangeElements(GL_TRIANGLES, 0, args.vertexCount-1, NumElements, GL_UNSIGNED_SHORT, (void*)(0));
				glDrawElements(GL_TRIANGLES, NumElements, GL_UNSIGNED_INT, 0);
				GE_ERROR_JUDGE();

			}
			else
			{
				glDrawElements(DrawMode, NumElements, IndexType, (void*)(uint64_t)(StartIndex));
			}
		}
		//GOpenGLKickHint.OnDrawCall(ContextState);
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
//		if (glCheckExtension("GL_EXT_debug_marker")) {
//			glInsertEventMarkerEXT(0, "My Render Pass");  // 插入事件标记
			//glPushGroupMarkerEXT(0, name);         // 开始一个调试组
			// ... 渲染代码 ...
			
	//	}
	}
	void CommandList::endMarker()
	{
		//glPopGroupMarkerEXT();                       // 结束调试组
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

    IDevice* CommandList::getDevice()
    {
        return m_Device;
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

	bool CommandList::hasTiledGPU()
	{
#ifdef GE_PLATFORM_WINDOWS
		return false;
#elif defined GE_PLATFORM_ANDROID 
		return true;
#endif
	}



	void Device::CommitDescriptorSets(FOpenGLContextState& ContextState)
	{

		std::vector<BindingSet*>& bindingSets = PendingState.BoundShaderState->LinkedProgram->Config.bindingSet;

		std::vector<std::pair<Texture*, uint32_t>> textures;
		std::vector<std::pair<Texture*, uint32_t>> images;
		std::vector<std::pair<Sampler*, uint32_t>> samplers;
		std::vector<std::pair<Buffer*, uint32_t>>  ubos;
		std::vector<std::pair<Buffer*, uint32_t>>  ssbos;

		for (BindingSet* set : bindingSets) {

			for (const BindingSetItem& item : set->getDesc()->bindings)
			{
				switch (item.type)
				{
				case RHIResourceType::RT_Texture_SRV: {
					Texture* texture = static_cast<Texture*>(item.resourceHandle);
					textures.push_back({ texture,item.slot });
					Sampler* sampler = texture->sampler;
					if (sampler) {
						samplers.push_back({ static_cast<Sampler*>(sampler), item.slot });
					}
					else {
						// push 占位符
						samplers.push_back({ nullptr, item.slot });
					}
					break;
				}

				case RHIResourceType::RT_Texture_UAV: {
					Texture* texture = static_cast<Texture*>(item.resourceHandle);
					images.push_back({ texture,item.slot });
					break;
				}


				case RHIResourceType::RT_TypedBuffer_SRV:
				case RHIResourceType::RT_StructuredBuffer_SRV:
				case RHIResourceType::RT_RawBuffer_SRV:
					//break;

				case RHIResourceType::RT_TypedBuffer_UAV:
				case RHIResourceType::RT_StructuredBuffer_UAV:
				case RHIResourceType::RT_RawBuffer_UAV: {
					Buffer* buffer = static_cast<Buffer*>(item.resourceHandle);
					ssbos.push_back({ buffer, item.slot });
					break;

				}

				case RHIResourceType::RT_ConstantBuffer: {
					Buffer* buffer = static_cast<Buffer*>(item.resourceHandle);
					ubos.push_back({ buffer, item.slot });

					break;
				}
				//gl 的sampler 和材质是绑定的，这里不需要处理									   
				case RHIResourceType::RT_Sampler: {
					/*Sampler* sampler = static_cast<Sampler*>(item.resourceHandle);
					samplers.push_back({ sampler,item.slot });*/
					break;
				}
				case RHIResourceType::RT_RayTracingAccelStruct:
				{
					//TODO::
					//_requireBufferState(checked_cast<AccelStruct*>(binding.resourceHandle)->dataBuffer, ResourceStates::AccelStructRead);
					GE_ASSERT(0, "imcomplete yet");
					break;
				}
				default:
					// do nothing
					break;
				}

			}
		}

        GE_ERROR_JUDGE();

		_commitSSBOs(ssbos, ContextState);
		_commitTexturesAndSamplers(textures, samplers, ContextState);
		_commitImages(images, ContextState);
		_commitUBOs(ubos, ContextState);
		//_commitSamplers(samplers, ContextState);
	}

	//todo:: buffer 合并
	void Device::_commitUBOs(const std::vector<std::pair<Buffer*, uint32_t>>& ubos, FOpenGLContextState& ContextState)
	{
        GE_ERROR_JUDGE();
		int bindIndex = 0;
		for (auto& _ubo : ubos) {
			OpenGLUniformBuffer* ubo = static_cast<OpenGLUniformBuffer*>(_ubo.first);
			if (ContextState.UniformBuffers[bindIndex] != ubo->rendererID){
				FOpenGL::BindBufferBase(GL_UNIFORM_BUFFER, _ubo.second, ubo->rendererID);
				GE_ERROR_JUDGE();
				ContextState.UniformBuffers[bindIndex] = ubo->rendererID;
			}
			bindIndex++;

		
		}
	}

	void Device::_commitSSBOs(const std::vector<std::pair<Buffer*, uint32_t>>& ssbos, FOpenGLContextState& ContextState)
	{
		int bindIndex = 0;
		for (auto& _ssbo : ssbos) {
			ShaderStorageBuffer* ssbo = static_cast<ShaderStorageBuffer*>(_ssbo.first);
			if (ContextState.UAVsBuffers[bindIndex].Resource != ssbo->rendererID) {
				//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->rendererID);
				FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, _ssbo.second, ssbo->rendererID);
				GE_ERROR_JUDGE();
				ContextState.UAVsBuffers[bindIndex].Resource = ssbo->rendererID;
			}
			bindIndex++;

		
		}
	}

	void Device::_commitTexturesAndSamplers(const std::vector<std::pair<Texture*, uint32_t>>& textures, const std::vector<std::pair<Sampler*, uint32_t>>& samplers, FOpenGLContextState& ContextState)
	{
        GE_ERROR_JUDGE();
		int bindIndex = 0;
       // if (textures.empty()) {
            //reset texture
            /*glBindTexture(GL_TEXTURE_2D, 0);
            glBindTexture(GL_TEXTURE_3D, 0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);*/

            for (size_t i = 0; i < ContextState.Textures.size(); i++)
            {
                ContextState.Textures[i].Reset();
            }

      //  }
		for (auto& _tex : textures) {
			Texture* tex = static_cast<Texture*>(_tex.first);
			uint32_t slot = _tex.second;
			GLenum targetDim = OpenGLUtil::convertTextureDimension(tex->getDesc().dimension);
			GLenum ContextStateDim = ContextState.Textures[bindIndex].Dimension;

			if (ContextState.Textures[bindIndex].Resource != tex->GetRendererID()) 
            {
				glActiveTexture(GL_TEXTURE0 + slot);
				GE_ERROR_JUDGE();
				ContextState.ActiveTexture = slot;
				ContextState.Textures[bindIndex].Resource = tex->GetRendererID();



				if (targetDim == ContextStateDim)
				{
					glBindTexture(targetDim, tex->GetRendererID());
                    GE_ERROR_JUDGE();
				}
				else
				{
					if (ContextStateDim != GL_NONE)
					{
						// Unbind different texture target on the same stage, to avoid OpenGL keeping its data, and potential driver problems.
						glBindTexture(ContextStateDim, 0);
                        GE_ERROR_JUDGE();
					}

					if (targetDim != GL_NONE)
					{
						glBindTexture(targetDim, tex->GetRendererID());
                        GE_ERROR_JUDGE();
					}
				}
				ContextState.Textures[bindIndex].Dimension = targetDim;

			}

			Sampler* sampler = static_cast<Sampler*>(samplers[bindIndex].first);
			if (!sampler)
				continue;

			if (ContextState.SamplerStates[bindIndex]->Resource == sampler->GetRenderID())
				continue;

			ContextState.SamplerStates[bindIndex]->Resource = sampler->GetRenderID();

			bool bExternalTexture = tex->getDesc().isExternalTexture;//(TextureStage.Target == GL_TEXTURE_EXTERNAL_OES);
			if (!bExternalTexture)
			{
				FOpenGLSamplerState* PendingSampler = PendingState.SamplerStates[bindIndex];

				if (ContextState.SamplerStates[bindIndex] != PendingSampler)
				{
					FOpenGL::BindSampler(bindIndex, PendingSampler ? PendingSampler->Resource : 0);
					GE_ERROR_JUDGE();
					ContextState.SamplerStates[bindIndex] = PendingSampler;
				}
			}
			else if (targetDim != GL_TEXTURE_BUFFER)
			{
				FOpenGL::BindSampler(bindIndex, 0);
				GE_ERROR_JUDGE();
				ContextState.SamplerStates[bindIndex] = nullptr;
				ApplyTextureStage(ContextState, bindIndex, ContextState.Textures[bindIndex], PendingState.SamplerStates[bindIndex]);
			}



			bindIndex++;

		}
	}

	void Device::_commitImages(const std::vector<std::pair<Texture*, uint32_t>>& images, FOpenGLContextState& ContextState)
	{
		int bindIndex = 0;
		for (auto& _img : images) {
			Texture* img = static_cast<Texture*>(_img.first);
			if (ContextState.UAVsTextures[bindIndex].Resource != img->GetRendererID()) {
				//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->rendererID);
				//FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, _ssbo.second, ssbo->rendererID);
				FOpenGL::BindImageTexture(_img.second,
					img->GetRendererID(),
					0,
					img->getDesc().isLayer ? GL_TRUE : GL_FALSE, 
					img->getDesc().layer, 
					OpenGLUtil::convertTextureAccess(img->getDesc().access),
					OpenGLUtil::convertTextureInnerFormat(img->getDesc().format));

				ContextState.UAVsTextures[bindIndex].Resource = img->GetRendererID();
				ContextState.UAVsTextures[bindIndex].Access = OpenGLUtil::convertTextureAccess(img->getDesc().access);
				ContextState.UAVsTextures[bindIndex].bLayered = img->getDesc().isLayer;
				ContextState.UAVsTextures[bindIndex].Layer = img->getDesc().layer;
				GE_ERROR_JUDGE();

			}
			bindIndex++;

		}
	}

	//void Device::_commitSamplers(const std::vector<std::pair<Sampler*, uint32_t>>& samplers, FOpenGLContextState& ContextState)
	//{
	//	int bindIndex = 0;
	//	for (auto& _sample : samplers) {
	//		Sampler* sampler = static_cast<Sampler*>(_sample.first);

	//		ContextState.SamplerStates[bindIndex]->Resource = sampler->GetRenderID();

	//		bool bExternalTexture = (TextureStage.Target == GL_TEXTURE_EXTERNAL_OES);
	//		if (!bExternalTexture)
	//		{
	//			FOpenGLSamplerState* PendingSampler = PendingState.SamplerStates[bindIndex];

	//			if (ContextState.SamplerStates[bindIndex] != PendingSampler)
	//			{
	//				FOpenGL::BindSampler(bindIndex, PendingSampler ? PendingSampler->Resource : 0);
	//				ContextState.SamplerStates[bindIndex] = PendingSampler;
	//			}
	//		}
	//		else if (TextureStage.Target != GL_TEXTURE_BUFFER)
	//		{
	//			FOpenGL::BindSampler(bindIndex, 0);
	//			ContextState.SamplerStates[bindIndex] = nullptr;
	//			ApplyTextureStage(ContextState, bindIndex, TextureStage, PendingState.SamplerStates[bindIndex]);
	//		}


	//		//if (ContextState.Textures[bindIndex] != img->GetRendererID()) {
	//		//	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->rendererID);
	//		//	//FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, _ssbo.second, ssbo->rendererID);
	//		//	FOpenGL::BindImageTexture(_sample.second, img->GetRendererID(), 0, img->getDesc().isLayer ? GL_TRUE : GL_FALSE, img->getDesc().layer, OpenGLUtil::convertTextureAccess(img->getDesc().access), OpenGLUtil::Format);

	//		//	ContextState.Textures[bindIndex] = img->GetRendererID();
	//		//}
	//		bindIndex++;

	//	}
	//}

	void Device::CommitGraphicsResourceTablesInner()
	{
	}
	void Device::CommitComputeResourceTables(Shader* ComputeShader)
	{
	}
	// shader->setUniform 在这里
	void Device::CommitNonComputeShaderConstants()
	{
		//VERIFY_GL_SCOPE();

		FOpenGLLinkedProgram* LinkedProgram = PendingState.BoundShaderState->LinkedProgram;
		if (GUseEmulatedUniformBuffers)
		{
			PendingState.ShaderParameters[ShaderType::VertexShader].CommitPackedUniformBuffers(LinkedProgram, ShaderType::VertexShader, PendingState.ShaderParameters[ShaderType::VertexShader].bindingSets);
			PendingState.ShaderParameters[ShaderType::Pixel].CommitPackedUniformBuffers(LinkedProgram, ShaderType::Pixel, PendingState.ShaderParameters[ShaderType::Pixel].bindingSets);
			if (PendingState.BoundShaderState->GetGeometryShader())
			{
				PendingState.ShaderParameters[ShaderType::Geometry].CommitPackedUniformBuffers(LinkedProgram, ShaderType::Geometry, PendingState.ShaderParameters[ShaderType::Geometry].bindingSets);
			}
		}

		if (LinkedProgram == PendingState.LinkedProgramAndDirtyFlag)
		{
			return;
		}

		// commit packed global only if current program has changed or any global parameter has changed (RHISetShaderParameter)
		PendingState.ShaderParameters[ShaderType::VertexShader].CommitPackedGlobals(LinkedProgram, ShaderType::VertexShader);
		PendingState.ShaderParameters[ShaderType::Pixel].CommitPackedGlobals(LinkedProgram, ShaderType::Pixel);
		if (PendingState.BoundShaderState->GetGeometryShader())
		{
			PendingState.ShaderParameters[ShaderType::Geometry].CommitPackedGlobals(LinkedProgram, ShaderType::Geometry);
		}

		PendingState.LinkedProgramAndDirtyFlag = LinkedProgram;
	}
	void Device::CommitComputeShaderConstants(Shader* ComputeShader)
	{
	}

	void CommandList::setDepthStencilState(DepthStencilState* state)
	{
		m_Device->PendingState.DepthStencilState = *(state);

	}

	void CommandList::setRasterizerState(RasterState* state)
	{
		m_Device->PendingState.RasterizerState = *state;

	}

	void CommandList::setBlendState(BlendState* state)
	{
		//���
		m_Device->PendingState.BlendState = *state;
		/*FOpenGLBlendState* NewState = ResourceCast(NewStateRHI);
		FMemory::Memcpy(&PendingState.BlendState, &(NewState->Data), sizeof(FOpenGLBlendStateData));*/
	}

	void CommandList::setIndexBufferState(IBuffer* IndexBuffer)
	{
		IBuffer* buf = IndexBuffer;
		if (buf)
			m_Device->PendingState.ibo = static_cast<Buffer*>(buf)->rendererID;
		else {
			m_Device->PendingState.ibo = 0;
			GE_CORE_WARN("no indexbuffer found");
		}
	}

	void CommandList::setVertexBufferState(const std::vector<VertexBufferBinding>& vertexBuffers)
	{
        m_Device->PendingState.vbos.clear();
		for (size_t i = 0; i < vertexBuffers.size(); i++)
		{
			IBuffer* buf = vertexBuffers[i].buffer;
			m_Device->PendingState.vbos.push_back(static_cast<Buffer*>(buf)->rendererID);

			VertexBuffer* vbo = static_cast<VertexBuffer*>(buf);
			if (vbo) {
				//TODO:: 目前所有offset都是0
				m_Device->RHISetStreamSource(i, vbo, 0);
			}
		}
		
	}

}