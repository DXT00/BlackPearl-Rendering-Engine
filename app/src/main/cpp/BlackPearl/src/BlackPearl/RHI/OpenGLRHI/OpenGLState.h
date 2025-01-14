#pragma once
#include "glad/glad.h"
#include "OpenGLDriver/OpenGL.h"
#include "OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/Core/Memory.h"
#include "BlackPearl/RHI/RHIShader.h"
#include "OpenGLShaderResource.h"
namespace BlackPearl {


#define ZERO_FILLED_DUMMY_UNIFORM_BUFFER_SIZE 65536

class FRenderTarget;

struct FOpenGLSamplerStateData
{
	// These enum is just used to count the number of members in this struct
	enum EGLSamplerData
	{
		EGLSamplerData_WrapS,
		EGLSamplerData_WrapT,
		EGLSamplerData_WrapR,
		EGLSamplerData_LODBias,
		EGLSamplerData_MagFilter,
		EGLSamplerData_MinFilter,
		EGLSamplerData_MaxAniso,
		EGLSamplerData_CompareMode,
		EGLSamplerData_CompareFunc,
		EGLSamplerData_Num,
	};

	GLint WrapS;
	GLint WrapT;
	GLint WrapR;
	GLint LODBias;
	GLint MagFilter;
	GLint MinFilter;
	GLint MaxAnisotropy;
	GLint CompareMode;
	GLint CompareFunc;

	FOpenGLSamplerStateData()
		: WrapS(GL_REPEAT)
		, WrapT(GL_REPEAT)
		, WrapR(GL_REPEAT)
		, LODBias(0)
		, MagFilter(GL_NEAREST)
		, MinFilter(GL_NEAREST)
		, MaxAnisotropy(1)
		, CompareMode(GL_NONE)
		, CompareFunc(GL_ALWAYS)
	{
	}
};

class FOpenGLSamplerState : public FRHISamplerState
{
public:
	GLuint Resource;
	FOpenGLSamplerStateData Data;

	~FOpenGLSamplerState();
};

struct FOpenGLRasterizerStateData
{
	GLenum FillMode = GL_FILL;
	GLenum CullMode = GL_NONE;
	float DepthBias = 0.0f;
	float SlopeScaleDepthBias = 0.0f;
	ERasterizerDepthClipMode DepthClipMode = ERasterizerDepthClipMode::DepthClip;
};

class FOpenGLRasterizerState : public FRHIRasterizerState
{
public:
	virtual bool GetInitializer(FRasterizerStateInitializerRHI& Init) override final;

	FOpenGLRasterizerStateData Data;
};

struct FOpenGLDepthStencilStateData
{
	bool bZEnable;
	bool bZWriteEnable;
	GLenum ZFunc;


	bool bStencilEnable;
	bool bTwoSidedStencilMode;
	GLenum StencilFunc;
	GLenum StencilFail;
	GLenum StencilZFail;
	GLenum StencilPass;
	GLenum CCWStencilFunc;
	GLenum CCWStencilFail;
	GLenum CCWStencilZFail;
	GLenum CCWStencilPass;
	uint32_t StencilReadMask;
	uint32_t StencilWriteMask;

	FOpenGLDepthStencilStateData()
		: bZEnable(false)
		, bZWriteEnable(true)
		, ZFunc(GL_LESS)
		, bStencilEnable(false)
		, bTwoSidedStencilMode(false)
		, StencilFunc(GL_ALWAYS)
		, StencilFail(GL_KEEP)
		, StencilZFail(GL_KEEP)
		, StencilPass(GL_KEEP)
		, CCWStencilFunc(GL_ALWAYS)
		, CCWStencilFail(GL_KEEP)
		, CCWStencilZFail(GL_KEEP)
		, CCWStencilPass(GL_KEEP)
		, StencilReadMask(0xFFFFFFFF)
		, StencilWriteMask(0xFFFFFFFF)
	{
	}
};

class FOpenGLDepthStencilState : public FRHIDepthStencilState
{
public:
	virtual bool GetInitializer(FDepthStencilStateInitializerRHI& Init) override final;

	FOpenGLDepthStencilStateData Data;
};

struct FOpenGLBlendStateData
{
	struct FRenderTarget
	{
		bool bAlphaBlendEnable;
		GLenum ColorBlendOperation;
		GLenum ColorSourceBlendFactor;
		GLenum ColorDestBlendFactor;
		bool bSeparateAlphaBlendEnable;
		GLenum AlphaBlendOperation;
		GLenum AlphaSourceBlendFactor;
		GLenum AlphaDestBlendFactor;
		uint32_t ColorWriteMaskR : 1;
		uint32_t ColorWriteMaskG : 1;
		uint32_t ColorWriteMaskB : 1;
		uint32_t ColorWriteMaskA : 1;
	};

	TStaticArray<FRenderTarget, MaxSimultaneousRenderTargets> RenderTargets;

	bool bUseAlphaToCoverage;

	FOpenGLBlendStateData()
	{
		bUseAlphaToCoverage = false;
		for (int32_t i = 0; i < MaxSimultaneousRenderTargets; ++i)
		{
			FRenderTarget& Target = RenderTargets[i];
			Target.bAlphaBlendEnable = false;
			Target.ColorBlendOperation = GL_NONE;
			Target.ColorSourceBlendFactor = GL_NONE;
			Target.ColorDestBlendFactor = GL_NONE;
			Target.bSeparateAlphaBlendEnable = false;
			Target.AlphaBlendOperation = GL_NONE;
			Target.AlphaSourceBlendFactor = GL_NONE;
			Target.AlphaDestBlendFactor = GL_NONE;
			Target.ColorWriteMaskR = false;
			Target.ColorWriteMaskG = false;
			Target.ColorWriteMaskB = false;
			Target.ColorWriteMaskA = false;
		}
	}
};

class FOpenGLBlendState : public FRHIBlendState
{
	FBlendStateInitializerRHI RHIInitializer;
public:
	FOpenGLBlendState(const FBlendStateInitializerRHI& Initializer) : RHIInitializer(Initializer) {}
	virtual bool GetInitializer(FBlendStateInitializerRHI& Init) override final
	{
		Init = RHIInitializer;
		return true;
	}

	FOpenGLBlendStateData Data;
};

struct FTextureStage
{
	class FOpenGLTexture* Texture;
	class FOpenGLShaderResourceView* SRV;
	GLenum Target;
	GLuint Resource;
	int32_t LimitMip;
	bool bHasMips;
	int32_t NumMips;

	FTextureStage()
		: Texture(NULL)
		, SRV(NULL)
		, Target(GL_NONE)
		, Resource(0)
		, LimitMip(-1)
		, bHasMips(false)
		, NumMips(0)
	{
	}
};

struct FUAVStage
{
	GLenum Format;
	GLuint Resource;
	GLenum Access;
	GLint Layer;
	bool bLayered;

	FUAVStage()
		: Format(GL_NONE)
		, Resource(0)
		, Access(GL_READ_WRITE)
		, Layer(0)
		, bLayered(false)
	{
	}
};
#define FOpenGLCachedAttr_Invalid (void*)(UPTRINT)0xFFFFFFFF
#define FOpenGLCachedAttr_SingleVertex (void*)(UPTRINT)0xFFFFFFFE



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
		assert(NumCombinedTextures >= OpenGL::GetMaxCombinedTextureImageUnits());
		assert(NumCombinedUAVUnits >= OpenGL::GetMaxCombinedUAVUnits());
		assert(Textures.IsEmpty() && SamplerStates.IsEmpty() && UAVs.Num() == 0);
		Textures.resize(NumCombinedTextures);
		SamplerStates.SetNumZeroed(NumCombinedTextures);

		UAVs.Reserve(NumCombinedUAVUnits);
		UAVs.AddDefaulted(NumCombinedUAVUnits);
	}

	virtual void CleanupResources()
	{
		SamplerStates.clear();
		Textures.clear();
		UAVs.clear();
	}
};

struct FOpenGLContextState final : public FOpenGLCommonState
{
	FOpenGLRasterizerStateData		RasterizerState;
	FOpenGLDepthStencilStateData	DepthStencilState;
	uint32_t							StencilRef;
	FOpenGLBlendStateData			BlendState;
	GLuint							Framebuffer;
	uint32_t						RenderTargetWidth;
	uint32_t						RenderTargetHeight;
	GLuint							OcclusionQuery;
	GLuint							Program;
	GLuint 							UniformBuffers[ShaderType::All * OGL_MAX_UNIFORM_BUFFER_BINDINGS];
	GLuint 							UniformBufferOffsets[ShaderType::All * OGL_MAX_UNIFORM_BUFFER_BINDINGS];
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
		CachedSamplerStates.clear(NumCombinedTextures);
		CachedSamplerStates.AddZeroed(NumCombinedTextures);

		checkf(NumCombinedUAVUnits <= sizeof(ActiveUAVMask) * 8, TEXT("Not enough bits in ActiveUAVMask to store %d UAV units"), NumCombinedUAVUnits);
	}

	virtual void CleanupResources() override
	{
		CachedSamplerStates.clear();
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

//template<>
//struct TOpenGLResourceTraits<FRHISamplerState>
//{
//	typedef FOpenGLSamplerState TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIRasterizerState>
//{
//	typedef FOpenGLRasterizerState TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIDepthStencilState>
//{
//	typedef FOpenGLDepthStencilState TConcreteType;
//};
//template<>
//struct TOpenGLResourceTraits<FRHIBlendState>
//{
//	typedef FOpenGLBlendState TConcreteType;
//};
}