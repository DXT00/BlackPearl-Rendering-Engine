#pragma once
#include "../RHIDevice.h"
#include "../RefCountPtr.h"
#include "../RHIPipeline.h"
#include "../RHIFrameBuffer.h"
#include "../RHIDefinitions.h"
#include "../RHICommandList.h"
#include "../RHIShader.h"
#include "../RHIInputLayout.h"
#include "../RHIQuery.h"
#include "../RHIDescriptorTable.h"
#include "../OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/Core/Container/TBitArray.h"
namespace BlackPearl {
    class OpenGLViewport;
	class Device :public RefCounter<IDevice>
	{
	public:
		friend class CommandList;
		Device();
		virtual ~Device() override {}




		virtual TextureHandle createTexture(TextureDesc& d) ;

		TextureHandle createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc) override;

		BufferHandle createBuffer(const BufferDesc& d) override;
		virtual FramebufferHandle createFramebuffer(const FramebufferDesc& desc);

		void* mapBuffer(IBuffer* b, CpuAccessMode mapFlags) override;
		void unmapBuffer(IBuffer* b) override;

		MemoryRequirements getBufferMemoryRequirements(IBuffer* buffer) override;
		GraphicsPipelineHandle createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb) override;
		ComputePipelineHandle createComputePipeline(const ComputePipelineDesc& desc) override;
		MeshletPipelineHandle createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb) override;
		RayTracingPipelineHandle createRayTracingPipeline(const RayTracingPipelineDesc& desc) override;


		BindingLayoutHandle createBindingLayout(const RHIBindingLayoutDesc& desc) override;
		BindingLayoutHandle createBindlessLayout(const RHIBindlessLayoutDesc& desc) override;
		BindingSetHandle createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout) override;

		CommandListHandle createCommandList(const CommandListParameters& params = CommandListParameters()) override;
		uint64_t executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue = CommandQueue::Graphics) override;

		IMessageCallback* getMessageCallback() override;

		SamplerHandle createSampler(const SamplerDesc& d) override;
		ShaderHandle createShader(const ShaderDesc& d, const void* binary, size_t binarySize) override;
		virtual ShaderLibraryHandle createShaderLibrary(const void* binary, size_t binarySize) override;

		InputLayoutHandle createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount);
		bool queryFeatureSupport(Feature feature, void* pInfo = nullptr, size_t infoSize = 0) override;
		

		void resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents = true) override;
		bool writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item) override;

		FormatSupport queryFormatSupport(Format format);
		// event queries
		EventQueryHandle createEventQuery() override;
		void setEventQuery(IEventQuery* query, CommandQueue queue) override;
		bool pollEventQuery(IEventQuery* query) override;
		void waitEventQuery(IEventQuery* query) override;
		void resetEventQuery(IEventQuery* query) override;



		static DeviceHandle createDevice();
	public:
        class FOpenGLContextState;
        class OpenGLContext;
        class Shader;
        class BoundShaderState;
        class FOpenGLRHIState;
        class FOpenGLLinkedProgram;
        class FOpenGLStream;
		void CachedBindUniformBuffer(FOpenGLContextState& ContextState, GLuint Buffer);

	protected:
		void InitializeStateResources();
		FOpenGLContextState& GetContextStateForCurrentContext(bool bAssertIfInvalid = true);

		/** needs to be called before each draw call */
		void BindPendingFramebuffer(FOpenGLContextState& ContextState);
		void UpdateRasterizerStateInOpenGLContext(FOpenGLContextState& ContextState);
		void UpdateDepthStencilStateInOpenGLContext(FOpenGLContextState& ContextState);
		void UpdateScissorRectInOpenGLContext(FOpenGLContextState& ContextState);
		void UpdateViewportInOpenGLContext(FOpenGLContextState& ContextState);

		void SetPendingBlendStateForActiveRenderTargets(FOpenGLContextState& ContextState);

		FORCEINLINE void CommitGraphicsResourceTables()
		{
			if (PendingState.bAnyDirtyGraphicsUniformBuffers)
			{
				CommitGraphicsResourceTablesInner();
			}
		}

		FORCEINLINE void CachedBindArrayBuffer(FOpenGLContextState& ContextState, GLuint Buffer)
		{
		//	VERIFY_GL_SCOPE();
			if (ContextState.ArrayBufferBound != Buffer)
			{
				glBindBuffer(GL_ARRAY_BUFFER, Buffer);
				ContextState.ArrayBufferBound = Buffer;
			}
		}

		void CachedBindElementArrayBuffer(FOpenGLContextState& ContextState, GLuint Buffer)
		{
			//VERIFY_GL_SCOPE();
			if (ContextState.ElementArrayBufferBound != Buffer)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
				ContextState.ElementArrayBufferBound = Buffer;
			}
		}

		/* shader */
		void BindPendingShaderState(FOpenGLContextState& ContextState);
		void BindUniformBufferBase(FOpenGLContextState& ContextState, int32_t NumUniformBuffers, uint32_t** BoundUniformBuffers, uint32_t FirstUniformBuffer, bool ForceUpdate);
	    void BindPendingComputeShaderState(FOpenGLContextState& ContextState, IShader* ComputeShader);


		void CommitGraphicsResourceTablesInner();
		void CommitComputeResourceTables(Shader* ComputeShader);
		void CommitNonComputeShaderConstants();
		void CommitComputeShaderConstants(Shader* ComputeShader);

		/* Texture */
		void SetupTexturesForDraw(FOpenGLContextState& ContextState);
		template <typename StateType>
		void SetupTexturesForDraw(FOpenGLContextState& ContextState, const StateType& ShaderState, int32_t MaxTexturesNeeded);

		/* SSBO */

		void SetupUAVsForDraw(FOpenGLContextState& ContextState);
		void SetupUAVsForCompute(FOpenGLContextState& ContextState, const Shader* ComputeShader);
		void SetupUAVsForProgram(FOpenGLContextState& ContextState, const TBitArray& NeededBits, int32_t MaxUAVUnitUsed);
		void CachedSetupUAVStage(FOpenGLContextState& ContextState, GLint UAVIndex, GLenum Format, GLuint Resource, bool bLayered, GLint Layer, GLenum Access);

		void RHIClearMRT(const bool* bClearColorArray, int32_t NumClearColors, const Color* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32_t Stencil);
		
		
	
		
		BoundShaderState* RHICreateBoundShaderState_Internal(
			IInputLayout* VertexDeclarationRHI,
			IShader* VertexShaderRHI,
			IShader* PixelShaderRHI,
			IShader* GeometryShaderRHI,
			bool bFromPSOFileCache
		);


		void SetupVertexArrays(FOpenGLContextState& ContextState, uint32_t BaseVertexIndex, FOpenGLStream* Streams, uint32_t NumStreams, uint32_t MaxVertices);

			/** RHI device state, independent of underlying OpenGL context used */
		FOpenGLRHIState						PendingState;


		/* store opengl context*/
		//FPlatformOpenGLDevice* m_PlatformDevice = nullptr;
		OpenGLContext m_Context;
		
		/** Per-context state caching */
		FOpenGLContextState InvalidContextState;
		FOpenGLContextState	SharedContextState;
		FOpenGLContextState	RenderingContextState;



		// CommandListResourceStateTracker m_StateTracker;
		bool m_EnableAutomaticBarriers = true;


		GraphicsState m_CurrentGraphicsState{};
		ComputeState m_CurrentComputeState{};
		MeshletState m_CurrentMeshletState{};
		RayTracingState m_CurrentRayTracingState;
		bool m_AnyVolatileBufferWrites = false;


		/** Counter incremented each time RHIBeginScene is called. */
		uint32_t SceneFrameCounter;

		/** Value used to detect when resource tables need to be recached. INDEX_NONE means always recache. */
		uint32_t ResourceTableFrameCounter;


		//FSamplerStateRHIRef					PointSamplerState;

		/** A list of all viewport RHIs that have been created. */
		std::vector<OpenGLViewport*>        Viewports;
		OpenGLViewport* DrawingViewport;
		bool								bRevertToSharedContextAfterDrawingViewport;

		bool								bIsRenderingContextAcquired;

		PrimitiveType						PSOPrimitiveType = PrimitiveType::NUM;

		///** A history of the most recently used bound shader states, used to keep transient bound shader states from being recreated for each use. */
		//TGlobalResource< TBoundShaderStateHistory<10000> > BoundShaderStateHistory;


		// Cached context type on BeginScene
		int32_t BeginSceneContextType;

		/*template <typename TRHIShader, typename TRHIProxyShader>
		void ApplyStaticUniformBuffers(TRHIShader* Shader, TRHIProxyShader* ProxyShader);

		std::vector<FRHIUniformBuffer*> GlobalUniformBuffers;*/

		/** Cached mip-limits for textures when ARB_texture_view is unavailable */
		std::map<GLuint, std::pair<GLenum, GLenum>> TextureMipLimits;

		private:
			/**
			 * Link vertex and pixel shaders in to an OpenGL program.
			 */
			FOpenGLLinkedProgram* LinkProgram(Shader* vertexShader, Shader* pixelShader, Shader* geometryShader);



	};

	template<typename StateType>
	inline void Device::SetupTexturesForDraw(FOpenGLContextState& ContextState, const StateType& ShaderState, int32_t MaxTexturesNeeded)
	{
		//VERIFY_GL_SCOPE();
		//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLTextureBindTime);

		int32_t MaxProgramTexture = 0;
		const TBitArray<>& NeededBits = ShaderState->GetTextureNeeds(MaxProgramTexture);

		for (int32_t TextureStageIndex = 0; TextureStageIndex <= MaxProgramTexture; ++TextureStageIndex)
		{
			if (!NeededBits[TextureStageIndex])
			{
				// Current program doesn't make use of this texture stage. No matter what UnrealEditor wants to have on in,
				// it won't be useful for this draw, so telling OpenGL we don't really need it to give the driver
				// more leeway in memory management, and avoid false alarms about same texture being set on
				// texture stage and in framebuffer.
				CachedSetupTextureStage(ContextState, TextureStageIndex, GL_NONE, 0, -1, 1);
			}
			else
			{
				const FTextureStage& TextureStage = PendingState.Textures[TextureStageIndex];

				CachedSetupTextureStage(ContextState, TextureStageIndex, TextureStage.Target, TextureStage.Resource, TextureStage.LimitMip, TextureStage.NumMips);

				bool bExternalTexture = (TextureStage.Target == GL_TEXTURE_EXTERNAL_OES);
				if (!bExternalTexture)
				{
					FOpenGLSamplerState* PendingSampler = PendingState.SamplerStates[TextureStageIndex];

					if (ContextState.SamplerStates[TextureStageIndex] != PendingSampler)
					{
						FOpenGL::BindSampler(TextureStageIndex, PendingSampler ? PendingSampler->Resource : 0);
						ContextState.SamplerStates[TextureStageIndex] = PendingSampler;
					}
				}
				else if (TextureStage.Target != GL_TEXTURE_BUFFER)
				{
					FOpenGL::BindSampler(TextureStageIndex, 0);
					ContextState.SamplerStates[TextureStageIndex] = nullptr;
					ApplyTextureStage(ContextState, TextureStageIndex, TextureStage, PendingState.SamplerStates[TextureStageIndex]);
				}
			}
		}

		// For now, continue to clear unused stages
		for (int32_t TextureStageIndex = MaxProgramTexture + 1; TextureStageIndex < MaxTexturesNeeded; ++TextureStageIndex)
		{
			CachedSetupTextureStage(ContextState, TextureStageIndex, GL_NONE, 0, -1, 1);
		}
	}

}

