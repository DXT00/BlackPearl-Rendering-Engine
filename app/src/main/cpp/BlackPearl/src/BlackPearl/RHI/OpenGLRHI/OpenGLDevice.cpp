#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLTexture.h"
#include "OpenGLCubeMapTexture.h"
#include "OpenGLImageTexture2D.h"
#include "OpenGLFrameBuffer.h"
#include "OpenGLCommandList.h"
#include "BlackPearl/Core.h"
#include "BlackPearl/Log.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLDrvPrivate.h"
#include "BlackPearl/RHI/RHIGlobals.h"
#include "BlackPearl/RHI/DynamicRHI.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
namespace BlackPearl 
{
	extern GLint GMaxOpenGLColorSamples;
	extern GLint GMaxOpenGLDepthSamples;
	extern GLint GMaxOpenGLIntegerSamples;
	extern GLint GMaxOpenGLTextureFilterAnisotropic;
	extern GLint GMaxOpenGLDrawBuffers;

	bool GUseEmulatedUniformBuffers;

#if GE_PLATFORM_WINDOWS
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT_ProcAddress = NULL;
#endif


	TextureHandle Device::createTexture(TextureDesc& d)
	{
		Texture* texture = nullptr;
		if (d.type == TextureType::CubeMap) {
			texture = DBG_NEW CubeMapTexture(d);
		}
		else if(d.type == TextureType::Image2DMap){
			
			texture = DBG_NEW ImageTexture2D(d, d.data);

		}
		else {
			texture = DBG_NEW Texture(d);
		}

		GE_ASSERT(texture, "texture is nullptr");

		texture->Init(d, d.data);

		return TextureHandle::Create(texture);
	}

	TextureHandle Device::createHandleForNativeTexture(uint32_t objectType, RHIObject texture, const TextureDesc& desc)
	{
		return TextureHandle();
	}


	BufferHandle Device::createBuffer(const BufferDesc& d)
	{
		return BufferHandle();
	}

	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		return FramebufferHandle();
	}

	void* Device::mapBuffer(IBuffer* b, CpuAccessMode mapFlags)
	{
		return nullptr;
	}

	void Device::unmapBuffer(IBuffer* b)
	{
	}

	MemoryRequirements Device::getBufferMemoryRequirements(IBuffer* buffer)
	{
		return MemoryRequirements();
	}

	GraphicsPipelineHandle Device::createGraphicsPipeline(const GraphicsPipelineDesc& desc, IFramebuffer* fb)
	{
		return GraphicsPipelineHandle();
	}

	ComputePipelineHandle Device::createComputePipeline(const ComputePipelineDesc& desc)
	{
		return ComputePipelineHandle();
	}

	MeshletPipelineHandle Device::createMeshletPipeline(const MeshletPipelineDesc& desc, IFramebuffer* fb)
	{
		return MeshletPipelineHandle();
	}

	RayTracingPipelineHandle Device::createRayTracingPipeline(const RayTracingPipelineDesc& desc)
	{
		return RayTracingPipelineHandle();
	}

	BindingLayoutHandle Device::createBindingLayout(const RHIBindingLayoutDesc& desc)
	{
		return BindingLayoutHandle();
	}

	BindingLayoutHandle Device::createBindlessLayout(const RHIBindlessLayoutDesc& desc)
	{
		return BindingLayoutHandle();
	}

	BindingSetHandle Device::createBindingSet(const BindingSetDesc& desc, IBindingLayout* layout)
	{
		return BindingSetHandle();
	}

	CommandListHandle Device::createCommandList(const CommandListParameters& params)
	{

		CommandList* cmdList = new CommandList(this, m_Context, params);

		return CommandListHandle::Create(cmdList);

	}

	uint64_t Device::executeCommandLists(ICommandList* const* pCommandLists, size_t numCommandLists, CommandQueue executionQueue)
	{
		return 0;
	}

	IMessageCallback* Device::getMessageCallback()
	{
		return nullptr;
	}

	SamplerHandle Device::createSampler(const SamplerDesc& d)
	{
		return SamplerHandle();
	}

	ShaderHandle Device::createShader(const ShaderDesc& d, const void* binary, size_t binarySize)
	{
		return ShaderHandle();
	}

	ShaderLibraryHandle Device::createShaderLibrary(const void* binary, size_t binarySize)
	{
		return ShaderLibraryHandle();
	}

	InputLayoutHandle Device::createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount, IShader* vertexShader)
	{
		return InputLayoutHandle();
	}

	bool Device::queryFeatureSupport(Feature feature, void* pInfo, size_t infoSize)
	{
		return false;
	}

	void Device::resizeDescriptorTable(IDescriptorTable* descriptorTable, uint32_t newSize, bool keepContents)
	{
	}

	bool Device::writeDescriptorTable(IDescriptorTable* descriptorTable, const BindingSetItem& item)
	{
		return false;
	}

	FormatSupport Device::queryFormatSupport(Format format)
	{
		return FormatSupport();
	}

	EventQueryHandle Device::createEventQuery()
	{
		return EventQueryHandle();
	}

	void Device::setEventQuery(IEventQuery* query, CommandQueue queue)
	{
	}

	bool Device::pollEventQuery(IEventQuery* query)
	{
		return false;
	}

	void Device::waitEventQuery(IEventQuery* query)
	{
	}

	void Device::resetEventQuery(IEventQuery* query)
	{
	}


	DeviceHandle Device::createDevice()
	{
		Device* device = new Device();

		return DeviceHandle(device);
	}

	void Device::InitializeStateResources()
	{
		SharedContextState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), FOpenGL::GetMaxCombinedUAVUnits());
		RenderingContextState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), FOpenGL::GetMaxCombinedUAVUnits());
		PendingState.InitializeResources(FOpenGL::GetMaxCombinedTextureImageUnits(), FOpenGL::GetMaxCombinedUAVUnits());
	}

	static void InitRHICapabilitiesForGL() {

//		GTexturePoolSize = 0;
//		GPoolSizeVRAMPercentage = 0;
//#if PLATFORM_WINDOWS || PLATFORM_LINUX
//		GConfig->GetInt(TEXT("TextureStreaming"), TEXT("PoolSizeVRAMPercentage"), GPoolSizeVRAMPercentage, GEngineIni);
//#endif

		// GL vendor and version information.

#define LOG_GL_STRING(StringEnum) GE_CORE_INFO("AverageFPS = " + std::string((const char*)glGetString(StringEnum)));

		GE_CORE_INFO(("Initializing OpenGL RHI"));
		LOG_GL_STRING(GL_VENDOR);
		LOG_GL_STRING(GL_RENDERER);
		LOG_GL_STRING(GL_VERSION);
		LOG_GL_STRING(GL_SHADING_LANGUAGE_VERSION);
#undef LOG_GL_STRING

		GRHIAdapterName = FOpenGL::GetAdapterName();
		GRHIAdapterInternalDriverVersion = std::string((const char*)glGetString(GL_VERSION));

		// Shader platform & RHI feature level
		GMaxRHIFeatureLevel = FOpenGL::GetFeatureLevel();
		GMaxRHIShaderPlatform = FOpenGL::GetShaderPlatform();

		// Log all supported extensions.
#if GE_PLATFORM_WINDOWS
		bool bWindowsSwapControlExtensionPresent = false;
#endif
		{
			extern void GetExtensionsString(std::string & ExtensionsString);
			std::string ExtensionsString;

			GetExtensionsString(ExtensionsString);

#if GE_PLATFORM_WINDOWS
			if (ExtensionsString.find("WGL_EXT_swap_control")!= std::string::npos)
			{
				bWindowsSwapControlExtensionPresent = true;
			}
#endif

			// Log supported GL extensions
			GE_CORE_INFO("OpenGL Extensions:" + ExtensionsString);
	/*		std::vector<std::string> GLExtensionArray;
			ExtensionsString.ParseIntoArray(GLExtensionArray, TEXT(" "), true);
			for (int ExtIndex = 0; ExtIndex < GLExtensionArray.Num(); ExtIndex++)
			{
				UE_LOG(LogRHI, Log, TEXT("  %s"), *GLExtensionArray[ExtIndex]);
			}*/

			//ApplyExtensionsOverrides(ExtensionsString);

			FOpenGL::ProcessExtensions(ExtensionsString);
		}

#if GE_PLATFORM_WINDOWS
#pragma warning(push)
#pragma warning(disable:4191)
		if (!bWindowsSwapControlExtensionPresent)
		{
			// Disable warning C4191: 'type cast' : unsafe conversion from 'PROC' to 'XXX' while getting GL entry points.
			PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT_ProcAddress =
				(PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

			if (strstr(wglGetExtensionsStringEXT_ProcAddress(), "WGL_EXT_swap_control") != NULL)
			{
				bWindowsSwapControlExtensionPresent = true;
			}
		}

		if (bWindowsSwapControlExtensionPresent)
		{
			wglSwapIntervalEXT_ProcAddress = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		}
#pragma warning(pop)
#endif

		// Set debug flag if context was setup with debugging
		FOpenGL::InitDebugContext();


#define LOG_AND_GET_GL_INT_TEMP(IntEnum,Default) GLint Value_##IntEnum = Default; if (IntEnum) {glGetIntegerv(IntEnum, &Value_##IntEnum); glGetError();} else {Value_##IntEnum = Default;} GE_CORE_INFO("  " #IntEnum + std::to_string(Value_##IntEnum))

		LOG_AND_GET_GL_INT_TEMP(GL_MAX_TEXTURE_SIZE, 0);
#if defined(GL_MAX_TEXTURE_BUFFER_SIZE)
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_TEXTURE_BUFFER_SIZE, 0);
#endif
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_CUBE_MAP_TEXTURE_SIZE, 0);
#if defined(GL_MAX_ARRAY_TEXTURE_LAYERS) && GL_MAX_ARRAY_TEXTURE_LAYERS
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_ARRAY_TEXTURE_LAYERS, 0);
#endif
#if GL_MAX_3D_TEXTURE_SIZE
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_3D_TEXTURE_SIZE, 0);
#endif
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_RENDERBUFFER_SIZE, 0);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_TEXTURE_IMAGE_UNITS, 0);

		LOG_AND_GET_GL_INT_TEMP(GL_MAX_DRAW_BUFFERS, 1);
		GMaxOpenGLDrawBuffers = Math::Min(Value_GL_MAX_DRAW_BUFFERS, (GLint)c_MaxRenderTargets);

		LOG_AND_GET_GL_INT_TEMP(GL_MAX_COLOR_ATTACHMENTS, 1);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_SAMPLES, 1);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_COLOR_TEXTURE_SAMPLES, 1);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_DEPTH_TEXTURE_SAMPLES, 1);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_INTEGER_SAMPLES, 1);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_VERTEX_ATTRIBS, 0);

		LOG_AND_GET_GL_INT_TEMP(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0);
		LOG_AND_GET_GL_INT_TEMP(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE, 0);


		//if (FParse::Param(FCommandLine::Get(), TEXT("quad_buffer_stereo")))
		//{
		//	GLboolean Result = GL_FALSE;
		//	glGetBooleanv(GL_STEREO, &Result);
		//	// Skip any errors if any were generated
		//	glGetError();
		//	GSupportsQuadBufferStereo = (Result == GL_TRUE);
		//}

		if (FOpenGL::SupportsTextureFilterAnisotropic())
		{
			LOG_AND_GET_GL_INT_TEMP(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, 0);
			GMaxOpenGLTextureFilterAnisotropic = Value_GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT;
		}

		if (FOpenGL::SupportsPixelLocalStorage())
		{
			LOG_AND_GET_GL_INT_TEMP(GL_MAX_SHADER_PIXEL_LOCAL_STORAGE_SIZE_EXT, 0);
		}
#undef LOG_AND_GET_GL_INT_TEMP

		GMaxOpenGLColorSamples = Value_GL_MAX_COLOR_TEXTURE_SAMPLES;
		GMaxOpenGLDepthSamples = Value_GL_MAX_DEPTH_TEXTURE_SAMPLES;
		GMaxOpenGLIntegerSamples = Value_GL_MAX_INTEGER_SAMPLES;

		// Verify some assumptions.
		// Android seems like reports one color attachment even when it supports MRT
#if !GE_PLATFORM_ANDROID
		assert(Value_GL_MAX_COLOR_ATTACHMENTS >= c_MaxRenderTargets);
#endif

	
		// Set capabilities.
		const GLint MajorVersion = FOpenGL::GetMajorVersion();
		const GLint MinorVersion = FOpenGL::GetMinorVersion();

		// Enable the OGL rhi thread if explicitly requested.
		GRHISupportsRHIThread = (GMaxRHIFeatureLevel <= ERHIFeatureLevel::ES3_1 );

		GRHISupportsMultithreadedResources = GRHISupportsRHIThread;

		// OpenGL ES does not support glTextureView
		GRHISupportsTextureViews = false;

		// By default use emulated UBs on mobile
#if !GE_PLATFORM_ANDROID
		GUseEmulatedUniformBuffers = true;//IsUsingEmulatedUniformBuffers(GMaxRHIShaderPlatform);
#endif
//		std::string FeatureLevelName;
//		GetFeatureLevelName(GMaxRHIFeatureLevel, FeatureLevelName);
//		std::string ShaderPlatformName = LegacyShaderPlatformToShaderFormat(GMaxRHIShaderPlatform).ToString();
//
//		printf("OpenGL MajorVersion = %d, MinorVersion = %d, ShaderPlatform = %s, FeatureLevel = %s\n", MajorVersion, MinorVersion, ShaderPlatformName.c_str(), FeatureLevelName.c_str());
//#if GE_PLATFORM_ANDROID
//		printf("GE_PLATFORM_ANDROID");
//#endif

		GMaxTextureSamplers = Value_GL_MAX_TEXTURE_IMAGE_UNITS;
		GMaxTextureMipCount = (int)math::log2f(Value_GL_MAX_TEXTURE_SIZE) + 1;
		GMaxTextureMipCount = Math::Min<int32_t>(MAX_TEXTURE_MIP_COUNT, GMaxTextureMipCount);
		GMaxTextureDimensions = Value_GL_MAX_TEXTURE_SIZE;

#if defined(GL_MAX_TEXTURE_BUFFER_SIZE)
		GMaxBufferDimensions = Value_GL_MAX_TEXTURE_BUFFER_SIZE;
#endif

		GMaxCubeTextureDimensions = Value_GL_MAX_CUBE_MAP_TEXTURE_SIZE;
#if defined(GL_MAX_ARRAY_TEXTURE_LAYERS) && GL_MAX_ARRAY_TEXTURE_LAYERS
		GMaxTextureArrayLayers = Value_GL_MAX_ARRAY_TEXTURE_LAYERS;
#endif

#if defined(GL_MAX_COMPUTE_SHARED_MEMORY_SIZE)
		GMaxComputeSharedMemory = Value_GL_MAX_COMPUTE_SHARED_MEMORY_SIZE;
#endif

		GMaxWorkGroupInvocations = Value_GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS;

		GSupportsParallelRenderingTasksWithSeparateRHIThread = false; // hopefully this is just temporary
		GRHIThreadNeedsKicking = true; // GL is SLOW
		GRHISupportsExactOcclusionQueries = FOpenGL::SupportsExactOcclusionQueries();

		GSupportsVolumeTextureRendering = FOpenGL::SupportsVolumeTextureRendering();
		GSupportsRenderDepthTargetableShaderResources = true;
		GSupportsRenderTargetFormat_PF_G8 = true;
		GSupportsSeparateRenderTargetBlendState = FOpenGL::SupportsSeparateAlphaBlend();
		GSupportsDepthBoundsTest = FOpenGL::SupportsDepthBoundsTest();

		GSupportsRenderTargetFormat_PF_FloatRGBA = FOpenGL::SupportsColorBufferHalfFloat();

		GSupportsWideMRT = FOpenGL::SupportsWideMRT();
		GSupportsTexture3D = FOpenGL::SupportsTexture3D();
		GSupportsMobileMultiView = FOpenGL::SupportsMobileMultiView();
		GSupportsImageExternal = FOpenGL::SupportsImageExternal();

		GSupportsShaderFramebufferFetch = FOpenGL::SupportsShaderFramebufferFetch();
		GSupportsShaderMRTFramebufferFetch = FOpenGL::SupportsShaderMRTFramebufferFetch();
		GSupportsShaderDepthStencilFetch = FOpenGL::SupportsShaderDepthStencilFetch();
		GSupportsPixelLocalStorage = FOpenGL::SupportsPixelLocalStorage();

		GMaxShadowDepthBufferSizeX = Math::Min<int32_t>(Value_GL_MAX_RENDERBUFFER_SIZE, 4096); // Limit to the D3D11 max.
		GMaxShadowDepthBufferSizeY = Math::Min<int32_t>(Value_GL_MAX_RENDERBUFFER_SIZE, 4096);
		GHardwareHiddenSurfaceRemoval = FOpenGL::HasHardwareHiddenSurfaceRemoval();
		GSupportsTimestampRenderQueries = FOpenGL::SupportsTimestampQueries();

		//GRHIMaxDispatchThreadGroupsPerDimension.X = MAX_uint16;
		//GRHIMaxDispatchThreadGroupsPerDimension.Y = MAX_uint16;
		//GRHIMaxDispatchThreadGroupsPerDimension.Z = MAX_uint16;

		// It's not possible to create a framebuffer with the backbuffer as the color attachment and a custom renderbuffer as the depth/stencil surface.
		GRHISupportsBackBufferWithCustomDepthStencil = false;

		GRHISupportsLazyShaderCodeLoading = true;

		GShaderPlatformForFeatureLevel[ERHIFeatureLevel::ES3_1] = (GMaxRHIFeatureLevel == ERHIFeatureLevel::ES3_1) ? GMaxRHIShaderPlatform : SP_OPENGL_PCES3_1;
		GShaderPlatformForFeatureLevel[ERHIFeatureLevel::SM5] = SP_NumPlatforms;


		GRHISupportsTextureStreaming = true;

		//// Disable texture streaming if forced off by r.OpenGL.DisableTextureStreamingSupport
		//static const auto CVarDisableOpenGLTextureStreamingSupport = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.OpenGL.DisableTextureStreamingSupport"));
		//if (CVarDisableOpenGLTextureStreamingSupport->GetValueOnAnyThread())
		//{
		//	GRHISupportsTextureStreaming = false;
		//}

//		for (int32_t PF = 0; PF < Format::MAX; ++PF)
//		{
//			SetupTextureFormat(EPixelFormat(PF), FOpenGLTextureFormat());
//		}
//
//		GLenum DepthFormat = FOpenGL::GetDepthFormat();
//		GLenum ShadowDepthFormat = FOpenGL::GetShadowDepthFormat();
//
//		// Initialize the platform pixel format map.					InternalFormat				InternalFormatSRGB		Format				Type							bCompressed		bBGRA
//		SetupTextureFormat(Format::UNKNOWN, FOpenGLTextureFormat());
//		SetupTextureFormat(Format::A32B32G32R32F, FOpenGLTextureFormat(GL_RGBA32F, GL_RGBA32F, GL_RGBA, GL_FLOAT, false, false));
//		SetupTextureFormat(Format::UYVY, FOpenGLTextureFormat());
//		if (CVarGLDepth24Bit.GetValueOnAnyThread() != 0)
//		{
//			SetupTextureFormat(Format::DepthStencil, FOpenGLTextureFormat(GL_DEPTH24_STENCIL8, GL_NONE, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, false, false));
//			GPixelFormats[Format::DepthStencil].BlockBytes = 4;
//		}
//		else
//		{
//			SetupTextureFormat(Format::DepthStencil, FOpenGLTextureFormat(GL_DEPTH32F_STENCIL8, GL_NONE, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, false, false));
//			GPixelFormats[Format::DepthStencil].BlockBytes = 8;
//		}
//		GPixelFormats[Format::X24_G8].Supported = true;
//		SetupTextureFormat(Format::ShadowDepth, FOpenGLTextureFormat(ShadowDepthFormat, ShadowDepthFormat, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
//		SetupTextureFormat(Format::D24, FOpenGLTextureFormat(DepthFormat, DepthFormat, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, false, false));
//		SetupTextureFormat(Format::A16B16G16R16, FOpenGLTextureFormat(GL_RGBA16F, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false, false));
//		SetupTextureFormat(Format::A1, FOpenGLTextureFormat());
//		SetupTextureFormat(Format::R16G16B16A16_UINT, FOpenGLTextureFormat(GL_RGBA16UI, GL_RGBA16UI, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, false, false));
//		SetupTextureFormat(Format::R16G16B16A16_SINT, FOpenGLTextureFormat(GL_RGBA16I, GL_RGBA16I, GL_RGBA_INTEGER, GL_SHORT, false, false));
//		SetupTextureFormat(Format::R32G32B32A32_UINT, FOpenGLTextureFormat(GL_RGBA32UI, GL_RGBA32UI, GL_RGBA_INTEGER, GL_UNSIGNED_INT, false, false));
//		SetupTextureFormat(Format::R16G16B16A16_UNORM, FOpenGLTextureFormat(GL_RGBA16, GL_RGBA16, GL_RGBA, GL_UNSIGNED_SHORT, false, false));
//		SetupTextureFormat(Format::R16G16B16A16_SNORM, FOpenGLTextureFormat(GL_RGBA16, GL_RGBA16, GL_RGBA, GL_SHORT, false, false));
//
//		SetupTextureFormat(Format::R16G16_UINT, FOpenGLTextureFormat(GL_RG16UI, GL_RG16UI, GL_RG_INTEGER, GL_UNSIGNED_SHORT, false, false));
//		SetupTextureFormat(Format::R8, FOpenGLTextureFormat(GL_R8, GL_R8, GL_RED, GL_UNSIGNED_BYTE, false, false));
//
//		SetupTextureFormat(Format::R5G6B5_UNORM, FOpenGLTextureFormat(GL_RGB565, GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, false, false));
//
//		//GL_UNSIGNED_SHORT_1_5_5_5_REV is not defined in gles
//		GLenum GL5551Format = FOpenGL::GetPlatfrom5551Format();
//		//the last 5 bits of GL_UNSIGNED_SHORT_1_5_5_5_REV is Red, while the last 5 bits of DXGI_FORMAT_B5G5R5A1_UNORM is Blue
//		bool bNeedsToSwizzleRedBlue = GL5551Format != GL_UNSIGNED_SHORT_5_5_5_1;
//		SetupTextureFormat(Format::B5G5R5A1_UNORM, FOpenGLTextureFormat(GL_RGB5_A1, GL_RGB5_A1, GL_RGBA, GL5551Format, false, bNeedsToSwizzleRedBlue));
//
//#if GE_PLATFORM_ANDROID
//		// Unfortunatelly most OpenGLES devices do not support R16Unorm pixel format, fallback to a less precise R16F
//		SetupTextureFormat(Format::G16, FOpenGLTextureFormat(GL_R16F, GL_R16F, GL_RED, GL_HALF_FLOAT, false, false));
//#else
//		SetupTextureFormat(Format::G16, FOpenGLTextureFormat(GL_R16, GL_R16, GL_RED, GL_UNSIGNED_SHORT, false, false));
//#endif
//		SetupTextureFormat(Format::R32_FLOAT, FOpenGLTextureFormat(GL_R32F, GL_R32F, GL_RED, GL_FLOAT, false, false));
//		SetupTextureFormat(Format::G16R16F, FOpenGLTextureFormat(GL_RG16F, GL_RG16F, GL_RG, GL_HALF_FLOAT, false, false));
//		SetupTextureFormat(Format::G16R16F_FILTER, FOpenGLTextureFormat(GL_RG16F, GL_RG16F, GL_RG, GL_HALF_FLOAT, false, false));
//		SetupTextureFormat(Format::G32R32F, FOpenGLTextureFormat(GL_RG32F, GL_RG32F, GL_RG, GL_FLOAT, false, false));
//		SetupTextureFormat(Format::A2B10G10R10, FOpenGLTextureFormat(GL_RGB10_A2, GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV, false, false));
//		SetupTextureFormat(Format::R16F, FOpenGLTextureFormat(GL_R16F, GL_R16F, GL_RED, GL_HALF_FLOAT, false, false));
//		SetupTextureFormat(Format::R16F_FILTER, FOpenGLTextureFormat(GL_R16F, GL_R16F, GL_RED, GL_HALF_FLOAT, false, false));
//		SetupTextureFormat(Format::A8, FOpenGLTextureFormat(GL_R8, GL_R8, GL_RED, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::R32_UINT, FOpenGLTextureFormat(GL_R32UI, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, false, false));
//		SetupTextureFormat(Format::R32_SINT, FOpenGLTextureFormat(GL_R32I, GL_R32I, GL_RED_INTEGER, GL_INT, false, false));
//		SetupTextureFormat(Format::R16_UINT, FOpenGLTextureFormat(GL_R16UI, GL_R16UI, GL_RED_INTEGER, GL_UNSIGNED_SHORT, false, false));
//		SetupTextureFormat(Format::R16_SINT, FOpenGLTextureFormat(GL_R16I, GL_R16I, GL_RED_INTEGER, GL_SHORT, false, false));
//		SetupTextureFormat(Format::R8_UINT, FOpenGLTextureFormat(GL_R8UI, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::R8G8, FOpenGLTextureFormat(GL_RG8, GL_RG8, GL_RG, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::R32G32_UINT, FOpenGLTextureFormat(GL_RG32UI, GL_RG32UI, GL_RG_INTEGER, GL_UNSIGNED_INT, false, false));
//		// Should be GL_RGBA8_SNORM but it doesn't work with glTexBuffer -> mapping to Unorm and unpacking in the shader
//		SetupTextureFormat(Format::R8G8B8A8_SNORM, FOpenGLTextureFormat(GL_RGBA8, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, false, false));
//
//		SetupTextureFormat(Format::FloatRGB, FOpenGLTextureFormat(GL_R11F_G11F_B10F, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false, false));
//		SetupTextureFormat(Format::FloatR11G11B10, FOpenGLTextureFormat(GL_R11F_G11F_B10F, GL_R11F_G11F_B10F, GL_RGB, GL_UNSIGNED_INT_10F_11F_11F_REV, false, false));
//		SetupTextureFormat(Format::FloatRGBA, FOpenGLTextureFormat(GL_RGBA16F, GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT, false, false));
//
//		SetupTextureFormat(Format::R8G8_UINT, FOpenGLTextureFormat(GL_RG8UI, GL_RG8UI, GL_RG_INTEGER, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::R32G32B32_UINT, FOpenGLTextureFormat(GL_RGB32UI, GL_RGB32UI, GL_RGB_INTEGER, GL_UNSIGNED_INT, false, false));
//		SetupTextureFormat(Format::R32G32B32_SINT, FOpenGLTextureFormat(GL_RGB32I, GL_RGB32I, GL_RGB_INTEGER, GL_INT, false, false));
//		SetupTextureFormat(Format::R32G32B32F, FOpenGLTextureFormat(GL_RGB32F, GL_RGB32F, GL_RGB, GL_FLOAT, false, false));
//		SetupTextureFormat(Format::R8_SINT, FOpenGLTextureFormat(GL_R8I, GL_R8I, GL_RED_INTEGER, GL_BYTE, false, false));
//
//		SetupTextureFormat(Format::B8G8R8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, false, true));
//		SetupTextureFormat(Format::R8G8B8A8, FOpenGLTextureFormat(GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::R8G8B8A8_UINT, FOpenGLTextureFormat(GL_RGBA8UI, GL_RGBA8UI, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, false, false));
//		SetupTextureFormat(Format::G8, FOpenGLTextureFormat(GL_R8, GL_R8, GL_RED, GL_UNSIGNED_BYTE, false, false));
//
//#if PLATFORM_DESKTOP
//		CA_SUPPRESS(6286);
//		if (PLATFORM_DESKTOP || FOpenGL::GetFeatureLevel() >= ERHIFeatureLevel::SM5)
//		{
//			SetupTextureFormat(Format::V8U8, FOpenGLTextureFormat(GL_RG8_SNORM, GL_NONE, GL_RG, GL_BYTE, false, false));
//			SetupTextureFormat(Format::BC5, FOpenGLTextureFormat(GL_COMPRESSED_RG_RGTC2, GL_COMPRESSED_RG_RGTC2, GL_RG, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::BC4, FOpenGLTextureFormat(GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_RED_RGTC1, GL_RED, GL_UNSIGNED_BYTE, true, false));
//
//			SetupTextureFormat(Format::G16R16, FOpenGLTextureFormat(GL_RG16, GL_RG16, GL_RG, GL_UNSIGNED_SHORT, false, false));
//			SetupTextureFormat(Format::G16R16_SNORM, FOpenGLTextureFormat(GL_RG16_SNORM, GL_RG16_SNORM, GL_RG, GL_SHORT, false, false));
//		}
//		else
//#endif // PLATFORM_DESKTOP
//		{
//#if !PLATFORM_DESKTOP
//			FOpenGL::PE_SetupTextureFormat(&SetupTextureFormat); // platform extension
//#endif // !PLATFORM_DESKTOP
//		}
//
//		if (FOpenGL::SupportsDXT())
//		{
//			SetupTextureFormat(Format::DXT1, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::DXT3, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::DXT5, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//		}
//#if GE_PLATFORM_ANDROID
//		if (FOpenGL::SupportsETC2())
//		{
//			SetupTextureFormat(Format::ETC2_RGB, FOpenGLTextureFormat(GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_SRGB8_ETC2, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ETC2_RGBA, FOpenGLTextureFormat(GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ETC2_R11_EAC, FOpenGLTextureFormat(GL_COMPRESSED_R11_EAC, GL_COMPRESSED_R11_EAC, GL_RED, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ETC2_RG11_EAC, FOpenGLTextureFormat(GL_COMPRESSED_RG11_EAC, GL_COMPRESSED_RG11_EAC, GL_RG, GL_UNSIGNED_BYTE, true, false));
//		}
//#endif
//		if (FOpenGL::SupportsASTC())
//		{
//			SetupTextureFormat(Format::ASTC_4x4, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ASTC_6x6, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ASTC_8x8, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ASTC_10x10, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//			SetupTextureFormat(Format::ASTC_12x12, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_RGBA, GL_UNSIGNED_BYTE, true, false));
//		}
//		if (FOpenGL::SupportsASTCHDR())
//		{
//			SetupTextureFormat(Format::ASTC_4x4_HDR, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_4x4_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, GL_RGBA, GL_HALF_FLOAT, true, false));
//			SetupTextureFormat(Format::ASTC_6x6_HDR, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_6x6_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, GL_RGBA, GL_HALF_FLOAT, true, false));
//			SetupTextureFormat(Format::ASTC_8x8_HDR, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_8x8_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, GL_RGBA, GL_HALF_FLOAT, true, false));
//			SetupTextureFormat(Format::ASTC_10x10_HDR, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_10x10_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, GL_RGBA, GL_HALF_FLOAT, true, false));
//			SetupTextureFormat(Format::ASTC_12x12_HDR, FOpenGLTextureFormat(GL_COMPRESSED_RGBA_ASTC_12x12_KHR, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, GL_RGBA, GL_HALF_FLOAT, true, false));
//		}
//		// Some formats need to know how large a block is.
//		GPixelFormats[Format::FloatRGB].BlockBytes = 4;
//		GPixelFormats[Format::FloatRGBA].BlockBytes = 8;

		// Temporary fix for nvidia driver issue with non-power-of-two shadowmaps (9/8/2016) UE-35312
		// @TODO revisit this with newer drivers
		GRHINeedsUnatlasedCSMDepthsWorkaround = true;

	/*	static const auto CVarPSOPrecaching = IConsoleManager::Get().FindConsoleVariable(TEXT("r.PSOPrecaching"));
		if (CVarPSOPrecaching && CVarPSOPrecaching->GetInt() != 0)*/
		{
			GRHISupportsPSOPrecaching = true;
		}

		GRHISupportsPipelineFileCache = !GRHISupportsPSOPrecaching;// || CVarEnablePSOFileCacheWhenPrecachingActive.GetValueOnAnyThread();

	}

	Device::Device()
	{

		m_Context.PlatformDevice = PlatformCreateOpenGLDevice();

		InitRHICapabilitiesForGL();

		assert(PlatformOpenGLCurrentContext(m_Context.PlatformDevice) == CONTEXT_Shared);


		assert(!GIsRHIInitialized);

		FOpenGLProgramBinaryCache::Initialize();

		InitializeStateResources();

		// Create a default point sampler state for internal use.
		/*FSamplerStateInitializerRHI PointSamplerStateParams(SF_Point, AM_Clamp, AM_Clamp, AM_Clamp);
		PointSamplerState = this->RHICreateSamplerState(PointSamplerStateParams);*/

#if GE_PLATFORM_WINDOWS

		extern int64_t GOpenGLDedicatedVideoMemory;
		extern int64_t GOpenGLTotalGraphicsMemory;

		GOpenGLDedicatedVideoMemory = FOpenGL::GetVideoMemorySize();

		if (GOpenGLDedicatedVideoMemory != 0)
		{
			GOpenGLTotalGraphicsMemory = GOpenGLDedicatedVideoMemory;

			if (GPoolSizeVRAMPercentage > 0)
			{
				float PoolSize = float(GPoolSizeVRAMPercentage) * 0.01f * float(GOpenGLTotalGraphicsMemory);

				// Truncate GTexturePoolSize to MB (but still counted in bytes)
				GTexturePoolSize = (int64_t)((int)(PoolSize / 1024.0f / 1024.0f)) * 1024 * 1024;

				printf("Texture pool is %llu MB (%d%% of %llu MB)",
					GTexturePoolSize / 1024 / 1024,
					GPoolSizeVRAMPercentage,
					GOpenGLTotalGraphicsMemory / 1024 / 1024);
			}
		}

#else

		static const auto CVarStreamingTexturePoolSize = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.Streaming.PoolSize"));
		GTexturePoolSize = (int64_t)CVarStreamingTexturePoolSize->GetValueOnAnyThread() * 1024 * 1024;

		printf("Texture pool is %llu MB (of %llu MB total graphics mem)",
			GTexturePoolSize / 1024 / 1024,
			FOpenGL::GetVideoMemorySize());

#endif

		// Flush here since we might be switching to a different context/thread for rendering
		FOpenGL::Flush();

	/*	FHardwareInfo::RegisterHardwareInfo(NAME_RHI, TEXT("OpenGL"));

		GRHICommandList.GetImmediateCommandList().InitializeImmediateContexts();

		FRenderResource::InitPreRHIResources();*/
		GIsRHIInitialized = true;
	}
}