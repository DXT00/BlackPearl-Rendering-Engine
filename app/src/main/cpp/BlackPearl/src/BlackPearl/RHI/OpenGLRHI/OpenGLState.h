#pragma once
#include "glad/glad.h"
struct FOpenGLCommonState
{
	std::vector<FTextureStage>	Textures;
	std::vector<FOpenGLSamplerState*>	SamplerStates;
	std::vector<FUAVStage>		UAVs;

	FOpenGLCommonState()
	{}

	virtual ~FOpenGLCommonState()
	{
		FOpenGLCommonState::CleanupResources();
	}

	// NumCombinedTextures must be greater than or equal to FOpenGL::GetMaxCombinedTextureImageUnits()
	// NumCombinedUAVUnits must be greater than or equal to FOpenGL::GetMaxCombinedUAVUnits()
	virtual void InitializeResources(int32_t NumCombinedTextures, int32_t NumCombinedUAVUnits)
	{
		check(NumCombinedTextures >= FOpenGL::GetMaxCombinedTextureImageUnits());
		check(NumCombinedUAVUnits >= FOpenGL::GetMaxCombinedUAVUnits());
		check(Textures.IsEmpty() && SamplerStates.IsEmpty() && UAVs.Num() == 0);
		Textures.SetNum(NumCombinedTextures);
		SamplerStates.SetNumZeroed(NumCombinedTextures);

		UAVs.Reserve(NumCombinedUAVUnits);
		UAVs.AddDefaulted(NumCombinedUAVUnits);
	}

	virtual void CleanupResources()
	{
		SamplerStates.Empty();
		Textures.Empty();
		UAVs.Empty();
	}
};

struct FOpenGLContextState final : public FOpenGLCommonState
{
	FOpenGLRasterizerStateData		RasterizerState;
	FOpenGLDepthStencilStateData	DepthStencilState;
	uint32_t							StencilRef;
	FOpenGLBlendStateData			BlendState;
	GLuint							Framebuffer;
	uint32_t							RenderTargetWidth;
	uint32_t							RenderTargetHeight;
	GLuint							OcclusionQuery;
	GLuint							Program;
	GLuint 							UniformBuffers[CrossCompiler::NUM_SHADER_STAGES * OGL_MAX_UNIFORM_BUFFER_BINDINGS];
	GLuint 							UniformBufferOffsets[CrossCompiler::NUM_SHADER_STAGES * OGL_MAX_UNIFORM_BUFFER_BINDINGS];
	std::vector<FOpenGLSamplerState*>	CachedSamplerStates;
	GLenum							ActiveTexture;
	bool							bScissorEnabled;
	FIntRect						Scissor;
	FIntRect						Viewport;
	float							DepthMinZ;
	float							DepthMaxZ;
	GLuint							ArrayBufferBound;
	GLuint							ElementArrayBufferBound;
	GLuint							StorageBufferBound;
	GLuint							PixelUnpackBufferBound;
	GLuint							UniformBufferBound;
	FLinearColor					ClearColor;
	uint16_t						ClearStencil;
	float							ClearDepth;
	int32_t							FirstNonzeroRenderTarget;
	bool							bAlphaToCoverageEnabled;

	FOpenGLVertexDeclaration* VertexDecl;
	FOpenGLCachedAttr				VertexAttrs[NUM_OPENGL_VERTEX_STREAMS];
	FOpenGLStream					VertexStreams[NUM_OPENGL_VERTEX_STREAMS];

	uint32_t							ActiveStreamMask;
	uint32_t							VertexAttrs_EnabledBits;
	FORCEINLINE bool GetVertexAttrEnabled(int32_t Index) const
	{
		static_assert(NUM_OPENGL_VERTEX_STREAMS <= sizeof(VertexAttrs_EnabledBits) * 8, "Not enough bits in VertexAttrs_EnabledBits to store NUM_OPENGL_VERTEX_STREAMS");
		return !!(VertexAttrs_EnabledBits & (1 << Index));
	}
	FORCEINLINE void SetVertexAttrEnabled(int32_t Index, bool bEnable)
	{
		if (bEnable)
		{
			VertexAttrs_EnabledBits |= (1 << Index);
		}
		else
		{
			VertexAttrs_EnabledBits &= ~(1 << Index);
		}
	}

	uint32_t ActiveUAVMask;

	FOpenGLContextState()
		: StencilRef(0)
		, Framebuffer(0)
		, Program(0)
		, ActiveTexture(GL_TEXTURE0)
		, bScissorEnabled(false)
		, DepthMinZ(0.0f)
		, DepthMaxZ(1.0f)
		, ArrayBufferBound(0)
		, ElementArrayBufferBound(0)
		, StorageBufferBound(0)
		, PixelUnpackBufferBound(0)
		, UniformBufferBound(0)
		, ClearColor(-1, -1, -1, -1)
		, ClearStencil(0xFFFF)
		, ClearDepth(-1.0f)
		, FirstNonzeroRenderTarget(0)
		, bAlphaToCoverageEnabled(false)
		, VertexDecl(0)
		, VertexAttrs()
		, VertexStreams()
		, ActiveStreamMask(0)
		, VertexAttrs_EnabledBits(0)
		, ActiveUAVMask(0)
	{
		Scissor.Min.X = Scissor.Min.Y = Scissor.Max.X = Scissor.Max.Y = 0;
		Viewport.Min.X = Viewport.Min.Y = Viewport.Max.X = Viewport.Max.Y = 0;
		FMemory::Memzero(UniformBuffers, sizeof(UniformBuffers));
		FMemory::Memzero(UniformBufferOffsets, sizeof(UniformBufferOffsets));
	}

	virtual void InitializeResources(int32_t NumCombinedTextures, int32_t NumCombinedUAVUnits) override
	{
		FOpenGLCommonState::InitializeResources(NumCombinedTextures, NumCombinedUAVUnits);
		CachedSamplerStates.Empty(NumCombinedTextures);
		CachedSamplerStates.AddZeroed(NumCombinedTextures);

		checkf(NumCombinedUAVUnits <= sizeof(ActiveUAVMask) * 8, TEXT("Not enough bits in ActiveUAVMask to store %d UAV units"), NumCombinedUAVUnits);
	}

	virtual void CleanupResources() override
	{
		CachedSamplerStates.Empty();
		FOpenGLCommonState::CleanupResources();
	}
};

struct FOpenGLRHIState final : public FOpenGLCommonState
{
	FOpenGLRasterizerStateData		RasterizerState;
	FOpenGLDepthStencilStateData	DepthStencilState;
	uint32_t							StencilRef;
	FOpenGLBlendStateData			BlendState;
	GLuint							Framebuffer;
	bool							bScissorEnabled;
	FIntRect						Scissor;
	FIntRect						Viewport;
	float							DepthMinZ;
	float							DepthMaxZ;
	GLuint							ZeroFilledDummyUniformBuffer;
	uint32_t							RenderTargetWidth;
	uint32_t							RenderTargetHeight;
	GLuint							RunningOcclusionQuery;
	bool							bAlphaToCoverageEnabled;

	// Pending framebuffer setup
	int32_t							NumRenderingSamples;// Only used with GL_EXT_multisampled_render_to_texture
	int32_t							FirstNonzeroRenderTarget;
	FOpenGLTexture* RenderTargets[MaxSimultaneousRenderTargets];
	uint32_t							RenderTargetMipmapLevels[MaxSimultaneousRenderTargets];
	uint32_t							RenderTargetArrayIndex[MaxSimultaneousRenderTargets];
	FOpenGLTexture* DepthStencil;
	ERenderTargetStoreAction		StencilStoreAction;
	uint32_t							DepthTargetWidth;
	uint32_t							DepthTargetHeight;
	bool							bFramebufferSetupInvalid;

	// Information about pending BeginDraw[Indexed]PrimitiveUP calls.
	FOpenGLStream					DynamicVertexStream;
	uint32_t							NumVertices;
	uint32_t							PrimitiveType;
	uint32_t							NumPrimitives;
	uint32_t							MinVertexIndex;
	uint32_t							IndexDataStride;

	FOpenGLStream					Streams[NUM_OPENGL_VERTEX_STREAMS];

	// we null this when the we dirty PackedGlobalUniformDirty. Thus we can skip all of CommitNonComputeShaderConstants if it matches the current program
	FOpenGLLinkedProgram* LinkedProgramAndDirtyFlag;
	FOpenGLShaderParameterCache* ShaderParameters;

	TRefCountPtr<FOpenGLBoundShaderState>	BoundShaderState;
	FComputeShaderRHIRef					CurrentComputeShader;

	/** The RHI does not allow more than 14 constant buffers per shader stage due to D3D11 limits. */
	enum { MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE = 14 };

	/** Track the currently bound uniform buffers. */
	FRHIUniformBuffer* BoundUniformBuffers[SF_NumStandardFrequencies][MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE];

	/** Array to track if any real (not emulated) uniform buffers have been bound since the last draw call */
	bool bAnyDirtyRealUniformBuffers[SF_NumStandardFrequencies];
	/** Bit array to track which uniform buffers have changed since the last draw call. */
	bool bAnyDirtyGraphicsUniformBuffers;
	uint16_t DirtyUniformBuffers[SF_NumStandardFrequencies];

	// Used for if(!FOpenGL::SupportsFastBufferData())
	uint32_t UpVertexBufferBytes;
	uint32_t UpIndexBufferBytes;
	uint32_t UpStride;
	void* UpVertexBuffer;
	void* UpIndexBuffer;

	FOpenGLRHIState()
		: StencilRef(0)
		, Framebuffer(0)
		, bScissorEnabled(false)
		, DepthMinZ(0.0f)
		, DepthMaxZ(1.0f)
		, ZeroFilledDummyUniformBuffer(0)
		, RenderTargetWidth(0)
		, RenderTargetHeight(0)
		, RunningOcclusionQuery(0)
		, bAlphaToCoverageEnabled(false)
		, NumRenderingSamples(1)
		, FirstNonzeroRenderTarget(-1)
		, DepthStencil(0)
		, StencilStoreAction(ERenderTargetStoreAction::ENoAction)
		, DepthTargetWidth(0)
		, DepthTargetHeight(0)
		, bFramebufferSetupInvalid(true)
		, NumVertices(0)
		, PrimitiveType(0)
		, NumPrimitives(0)
		, MinVertexIndex(0)
		, IndexDataStride(0)
		, LinkedProgramAndDirtyFlag(nullptr)
		, ShaderParameters(NULL)
		, BoundShaderState(NULL)
		, CurrentComputeShader(NULL)
		, UpVertexBufferBytes(0)
		, UpIndexBufferBytes(0)
		, UpVertexBuffer(0)
		, UpIndexBuffer(0)
	{
		Scissor.Min.X = Scissor.Min.Y = Scissor.Max.X = Scissor.Max.Y = 0;
		Viewport.Min.X = Viewport.Min.Y = Viewport.Max.X = Viewport.Max.Y = 0;
		FMemory::Memset(RenderTargets, 0, sizeof(RenderTargets));	// setting all to 0 at start
		FMemory::Memset(RenderTargetMipmapLevels, 0, sizeof(RenderTargetMipmapLevels));	// setting all to 0 at start
		FMemory::Memset(RenderTargetArrayIndex, 0, sizeof(RenderTargetArrayIndex));	// setting all to 0 at start
		FMemory::Memset(BoundUniformBuffers, 0, sizeof(BoundUniformBuffers));
	}

	~FOpenGLRHIState()
	{
		CleanupResources();
	}

	virtual void InitializeResources(int32_t NumCombinedTextures, int32_t NumComputeUAVUnits) override;

	virtual void CleanupResources() override
	{
		delete[] ShaderParameters;
		ShaderParameters = NULL;
		FMemory::Memset(BoundUniformBuffers, 0, sizeof(BoundUniformBuffers));
		FOpenGLCommonState::CleanupResources();
	}
};

template<>
struct TOpenGLResourceTraits<FRHISamplerState>
{
	typedef FOpenGLSamplerState TConcreteType;
};
template<>
struct TOpenGLResourceTraits<FRHIRasterizerState>
{
	typedef FOpenGLRasterizerState TConcreteType;
};
template<>
struct TOpenGLResourceTraits<FRHIDepthStencilState>
{
	typedef FOpenGLDepthStencilState TConcreteType;
};
template<>
struct TOpenGLResourceTraits<FRHIBlendState>
{
	typedef FOpenGLBlendState TConcreteType;
};
