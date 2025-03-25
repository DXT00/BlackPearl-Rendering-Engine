#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLBoundShaderState.h"
#include "OpenGLContext.h"
#include "OpenGLCommandList.h"
#include "OpenGLBufferResource.h"
#include "OpenGLUniformBuffer.h"
#include "OpenGLBindingSet.h"
#include "OpenGLState.h"
#include "OpenGLTexture.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/RHI/RHIGlobals.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"

namespace BlackPearl {

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
		FRHISetRenderTargetsInfo RTInfo;
		renderPassInfo.ConvertToRenderTargetsInfo(RTInfo);
		// Begin GL_EXT_multisampled_render_to_texture if any
		m_Device->PendingState.NumRenderingSamples = SetupMultisampleRenderingInfo(RTInfo);
		_setRenderTargetsAndClear(RTInfo);

		m_Device->RenderPassInfo = renderPassInfo;

		if (renderPassInfo.NumOcclusionQueries > 0)
		{
			extern void BeginOcclusionQueryBatch(uint32_t);
			BeginOcclusionQueryBatch(renderPassInfo.NumOcclusionQueries);
		}

#ifdef GE_PLATFORM_ANDROID
		if (m_Device->RenderPassInfo.SubpassHint == ESubpassHint::DeferredShadingSubpass &&
			FOpenGL::SupportsPixelLocalStorage() && FOpenGL::SupportsShaderDepthStencilFetch())
		{
			glEnable(GL_SHADER_PIXEL_LOCAL_STORAGE_EXT);
		}
#endif

#ifdef GE_PLATFORM_ANDROID
		if (FAndroidOpenGL::RequiresAdrenoTilingModeHint())
		{
			FAndroidOpenGL::EnableAdrenoTilingModeHint(passName ==("SceneColorRendering"));
		}
#endif
	}

	void CommandList::endRenderPass()
	{
		if (m_Device->RenderPassInfo.NumOcclusionQueries > 0)
		{
			extern void EndOcclusionQueryBatch();
			EndOcclusionQueryBatch();
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
			RHIDiscardRenderTargets(bDiscardDepthStencil, bDiscardDepthStencil, ColorMask);
		}

		FRHIRenderTargetView RTV(nullptr, ERenderTargetLoadAction::ENoAction);
		FRHIDepthRenderTargetView DepthRTV(nullptr, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::ENoAction);
		_setRenderTargets(1, &RTV, &DepthRTV);

#if GE_PLATFORM_ANDROID
		if (RenderPassInfo.SubpassHint == ESubpassHint::DeferredShadingSubpass &&
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
		else if (buffer->desc.isConstantBuffer) {
			OpenGLUniformBuffer* ubo = static_cast<OpenGLUniformBuffer*>(buffer);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo->rendererID);
			glBufferData(GL_UNIFORM_BUFFER, dataSize, data, buffer->desc.isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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
	void CommandList::_setRenderTargetsAndClear(const FRHISetRenderTargetsInfo& RenderTargetsInfo)
	{
		this->_setRenderTargets(RenderTargetsInfo.NumColorRenderTargets,
			RenderTargetsInfo.ColorRenderTarget,
			&RenderTargetsInfo.DepthStencilRenderTarget);

		/**
		 * Convert all load action from NoAction to Clear for tiled GPU on OpenGL platform to avoid an unnecessary load action.
		 */

		bool bIsTiledGPU = hasTiledGPU();

		bool bClearColor = RenderTargetsInfo.bClearColor;
		bool bClearStencil = RenderTargetsInfo.bClearStencil;
		bool bClearDepth = RenderTargetsInfo.bClearDepth;

		Color ClearColors[c_MaxRenderTargets];
		float DepthClear = 0.0;
		uint32_t StencilClear = 0;
		//TODO::
		//for (int32_t i = 0; i < RenderTargetsInfo.NumColorRenderTargets; ++i)
		//{
		//	if (RenderTargetsInfo.ColorRenderTarget[i].Texture != nullptr)
		//	{
		//		const FClearValueBinding& ClearValue = RenderTargetsInfo.ColorRenderTarget[i].Texture->GetClearBinding();

		//		if (bIsTiledGPU)
		//		{
		//			bClearColor |= RenderTargetsInfo.ColorRenderTarget[i].LoadAction == ERenderTargetLoadAction::ENoAction;

		//			ClearColors[i] = ClearValue.ColorBinding == EClearBinding::EColorBound ? ClearValue.GetClearColor() : FLinearColor::Black;
		//		}
		//		else if (bClearColor)
		//		{
		//			checkf(ClearValue.ColorBinding == EClearBinding::EColorBound, TEXT("Texture: %s does not have a color bound for fast clears"), *RenderTargetsInfo.ColorRenderTarget[i].Texture->GetName().GetPlainNameString());

		//			ClearColors[i] = ClearValue.GetClearColor();
		//		}
		//	}
		//}

		//if (RenderTargetsInfo.DepthStencilRenderTarget.Texture != nullptr)
		//{
		//	const FClearValueBinding& ClearValue = RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetClearBinding();

		//	if (bIsTiledGPU)
		//	{
		//		bClearStencil |= RenderTargetsInfo.DepthStencilRenderTarget.StencilLoadAction == ERenderTargetLoadAction::ENoAction;

		//		bClearDepth |= RenderTargetsInfo.DepthStencilRenderTarget.DepthLoadAction == ERenderTargetLoadAction::ENoAction;

		//		if (ClearValue.ColorBinding == EClearBinding::EDepthStencilBound)
		//		{
		//			ClearValue.GetDepthStencil(DepthClear, StencilClear);
		//		}
		//	}
		//	else if (bClearDepth || bClearStencil)
		//	{
		//		checkf(ClearValue.ColorBinding == EClearBinding::EDepthStencilBound, TEXT("Texture: %s does not have a DS value bound for fast clears"), *RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetName().GetPlainNameString());

		//		ClearValue.GetDepthStencil(DepthClear, StencilClear);
		//	}
		//}

		//if (bClearColor || bClearStencil || bClearDepth)
		//{
		//	this->RHIClearMRT(bClearColor, RenderTargetsInfo.NumColorRenderTargets, ClearColors, bClearDepth, DepthClear, bClearStencil, StencilClear);
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

		// Not found. Preparing new one.
		GLuint Framebuffer;
		glGenFramebuffers(1, &Framebuffer);
		//VERIFY_GL(glGenFramebuffer)
		glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
		//VERIFY_GL(glBindFramebuffer)

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

			FOpenGL::CheckFrameBuffer();

			FOpenGL::ReadBuffer(GL_NONE);
			FOpenGL::DrawBuffer(GL_COLOR_ATTACHMENT0);

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
					switch (RenderTarget->Target)
					{
					case GL_RENDERBUFFER:
					{
						// lazily allocate render buffer storage in case it's multisampled
						ConditionallyAllocateRenderbufferStorage(*RenderTarget);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetResource());
						break;
					}
					case GL_TEXTURE_2D:
					case GL_TEXTURE_EXTERNAL_OES:
					case GL_TEXTURE_2D_MULTISAMPLE:
					{
						if (NumRenderingSamples > 1)
						{
							// GL_EXT_multisampled_render_to_texture
							FOpenGL::FramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->Target, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex], NumRenderingSamples);
						}
						else
						{
							FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->Target, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex]);
						}
						break;
					}
					case GL_TEXTURE_3D:
					case GL_TEXTURE_2D_ARRAY:
					case GL_TEXTURE_CUBE_MAP:
					case GL_TEXTURE_CUBE_MAP_ARRAY:
						FOpenGL::FramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex]);
						break;
					default:
						FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetResource());
						break;
					}
				}
				else
				{
					// Bind just one slice of the object
					switch (RenderTarget->Target)
					{
					case GL_RENDERBUFFER:
					{
						assert(ArrayIndices[RenderTargetIndex] == 0);
						// lazily allocate render buffer storage in case it's multisampled
						ConditionallyAllocateRenderbufferStorage(*RenderTarget);
						glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetResource());
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
							FOpenGL::FramebufferTexture2DMultisample(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->Target, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex], NumRenderingSamples);
						}
						else
						{
							FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->Target, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex]);
						}
						break;
					}
					case GL_TEXTURE_3D:
						FOpenGL::FramebufferTexture3D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->Target, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex], ArrayIndices[RenderTargetIndex]);
						break;
					case GL_TEXTURE_CUBE_MAP:
						assert(ArrayIndices[RenderTargetIndex] < 6);
						FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_TEXTURE_CUBE_MAP_POSITIVE_X + ArrayIndices[RenderTargetIndex], RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex]);
						break;
					case GL_TEXTURE_2D_ARRAY:
					case GL_TEXTURE_CUBE_MAP_ARRAY:
						FOpenGL::FramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, RenderTarget->GetResource(), MipmapLevels[RenderTargetIndex], ArrayIndices[RenderTargetIndex]);
						break;
					default:
						check(ArrayIndices[RenderTargetIndex] == 0);
						FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + RenderTargetIndex, GL_RENDERBUFFER, RenderTarget->GetResource());
						break;
					}
				}
				FirstNonzeroRenderTarget = RenderTargetIndex;
			}

			if (DepthStencilTarget)
			{
				switch (DepthStencilTarget->Target)
				{
				case GL_TEXTURE_2D:
				case GL_TEXTURE_EXTERNAL_OES:
				case GL_TEXTURE_2D_MULTISAMPLE:
				{
					FOpenGL::FramebufferTexture2D(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, DepthStencilTarget->Target, DepthStencilTarget->GetResource(), 0);
					break;
				}
				case GL_RENDERBUFFER:
				{
					// lazily allocate render buffer storage in case it's multisampled
					ConditionallyAllocateRenderbufferStorage(*DepthStencilTarget);
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthStencilTarget->GetResource());
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DepthStencilTarget->GetResource());
					VERIFY_GL(glFramebufferRenderbuffer);
					break;
				}
				case GL_TEXTURE_3D:
				case GL_TEXTURE_2D_ARRAY:
				case GL_TEXTURE_CUBE_MAP:
				case GL_TEXTURE_CUBE_MAP_ARRAY:
					FOpenGL::FramebufferTexture(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, DepthStencilTarget->GetResource(), 0);
					break;
				default:
					FOpenGL::FramebufferRenderbuffer(GL_FRAMEBUFFER, DepthStencilTarget->Attachment, GL_RENDERBUFFER, DepthStencilTarget->GetResource());
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

			GetOpenGLFramebufferCache().Add(FOpenGLFramebufferKey(NumSimultaneousRenderTargets, RenderTargets, ArrayIndices, MipmapLevels, DepthStencilTarget, NumRenderingSamples, PlatformOpenGLCurrentContext(PlatformDevice)), Framebuffer + 1);

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

	
		//uniform buffer + texture, sampler...
		//_bindBindingSets(state.bindings);
		

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
		setVertexBufferState(state.vertexBuffers);
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

		//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveTime);
		//VERIFY_GL_SCOPE();

		//FOpenGLBuffer* IndexBuffer = ResourceCast(IndexBufferRHI);

		//RHI_DRAW_CALL_STATS(PrimitiveType, NumPrimitives * NumInstances);

		FOpenGLContextState& ContextState = m_Device->GetContextStateForCurrentContext();
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_BindPendingFramebuffer);
			m_Device->BindPendingFramebuffer(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetPendingBlendStateForActiveRenderTargets);
			m_Device->SetPendingBlendStateForActiveRenderTargets(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_UpdateViewportInOpenGLContext);
			m_Device->UpdateViewportInOpenGLContext(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_UpdateScissorRectInOpenGLContext);
			m_Device->UpdateScissorRectInOpenGLContext(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_UpdateRasterizerStateInOpenGLContext);
			m_Device->UpdateRasterizerStateInOpenGLContext(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_UpdateDepthStencilStateInOpenGLContext);
			m_Device->UpdateDepthStencilStateInOpenGLContext(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_BindPendingShaderState);
			m_Device->BindPendingShaderState(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_CommitGraphicsResourceTables);
			m_Device->CommitGraphicsResourceTables();
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupTexturesForDraw);
			m_Device->SetupTexturesForDraw(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupUAVsForDraw);
			m_Device->SetupUAVsForDraw(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_CommitNonComputeShaderConstants);
			m_Device->CommitNonComputeShaderConstants();
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_CachedBindElementArrayBuffer);
			m_Device->CachedBindElementArrayBuffer(ContextState);
		}
		{
			//DETAILED_QUICK_SCOPE_CYCLE_COUNTER(STAT_SetupVertexArrays);
			m_Device->SetupVertexArrays(ContextState, args.startIndexLocation, m_Device->PendingState.Streams, NUM_OPENGL_VERTEX_STREAMS, args.vertexCount);
		}

		GLenum DrawMode = GL_TRIANGLES;
		GLsizei NumElements = 0;
		/*FindPrimitiveType(PrimitiveType, NumPrimitives, DrawMode, NumElements);*/
		GLenum IndexType = GL_UNSIGNED_INT;// IndexBuffer->GetStride() == sizeof(uint32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		//StartIndex *= IndexBuffer->GetStride() == sizeof(uint32) ? sizeof(uint32) : sizeof(uint16);
		uint32_t StartIndex = args.startIndexLocation * args.indexBufferStride;
		//GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, NumElements * NumInstances);
		if (args.instanceCount > 1)
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			//checkf(FirstInstance == 0, TEXT("FirstInstance is currently unsupported on this RHI"));
			FOpenGL::DrawElementsInstanced(DrawMode, NumElements, IndexType, (void*)(uint64_t)(StartIndex), args.instanceCount);
		}
		else
		{
			//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLDrawPrimitiveDriverTime);
			//CONDITIONAL_SCOPE_CYCLE_COUNTER(STAT_OpenGLShaderFirstDrawTime, PendingState.BoundShaderState->RequiresDriverInstantiation());
			if (FOpenGL::SupportsDrawIndexOffset())
			{
				FOpenGL::DrawRangeElements(DrawMode, 0, args.vertexCount, NumElements, IndexType, (void*)(uint64_t)(StartIndex));
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
		else
			m_Device->PendingState.ibo = 0;
	}

	void CommandList::setVertexBufferState(const std::vector<VertexBufferBinding>& vertexBuffers)
	{
		for (size_t i = 0; i < vertexBuffers.size(); i++)
		{
			IBuffer* buf = vertexBuffers[i].buffer;
			m_Device->PendingState.vbos.push_back(static_cast<Buffer*>(buf)->rendererID);
		}
		
	}

}