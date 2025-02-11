#include "pch.h"
#include "OpenGLDevice.h"
#include "OpenGLTexture.h"
#include "OpenGLCubeMapTexture.h"
#include "OpenGLImageTexture2D.h"
#include "OpenGLBindingLayout.h"
#include "OpenGLCommandList.h"
#include "OpenGLShader.h"
#include "BlackPearl/RHI/RHIGlobals.h"
#include "BlackPearl/RHI/OpenGLRHI/OpenGLProgramBinaryFileCache.h"
#include "OpenGLState.h"
//#include "OpenGLFrameBuffer.h"
//
//
//#include "OpenGLUtil.h"
//#include "BlackPearl/Core.h"
//#include "BlackPearl/Log.h"
//
//
//#include "BlackPearl/RHI/DynamicRHI.h"
//#include "BlackPearl/RHI/RHIDefinitions.h"
//#include "BlackPearl/RHI/RHITexture.h"
//#include "BlackPearl/RHI/RHIBindingLayoutDesc.h"
//#include "OpenGLDriver/OpenGLDrv.h"


#include "OpenGLDriver/OpenGLDrvPrivate.h"


namespace BlackPearl 
{

	extern class Log* g_Log;

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
		return nullptr;
	}



	FramebufferHandle Device::createFramebuffer(const FramebufferDesc& desc)
	{
		return nullptr;
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
		BindingLayout* ret = new BindingLayout(m_Context, desc);
		return BindingLayoutHandle::Create(ret);

	}

	BindingLayoutHandle Device::createBindlessLayout(const RHIBindlessLayoutDesc& desc)
	{
		BindingLayout* ret = new BindingLayout(m_Context, desc);

		//ret->bake();

		return BindingLayoutHandle::Create(ret);
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

	

	ShaderHandle Device::createShader(const ShaderDesc& d, const void* binary, size_t binarySize)
	{
		Shader* shader = nullptr;
		if (binary == nullptr)
		{
			shader = new Shader(d, d.filePath);
		}
		else {
			shader = new Shader(d, binary, binarySize);

		}

		/*shader->desc = d;
		shader->stageFlagBits = VkUtil::convertShaderTypeToShaderStageFlagBits(d.shaderType);

		VkShaderModuleCreateInfo shaderInfo{};
		shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderInfo.codeSize = binarySize;
		shaderInfo.pCode = (const uint32_t*)binary;


		if (vkCreateShaderModule(m_Context.device, &shaderInfo, m_Context.allocationCallbacks, &shader->shaderModule) != VK_SUCCESS) {
			GE_CORE_ERROR("failed to create shader module!");
		}*/



		return ShaderHandle::Create(shader);
	}

	ShaderLibraryHandle Device::createShaderLibrary(const void* binary, size_t binarySize)
	{
		return ShaderLibraryHandle();
	}

	InputLayoutHandle Device::createInputLayout(const VertexAttributeDesc* d, uint32_t attributeCount)
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

	FOpenGLContextState& Device::GetContextStateForCurrentContext(bool bAssertIfInvalid)
	{
		// most common case
		if (BeginSceneContextType == CONTEXT_Rendering)
		{
			return RenderingContextState;
		}

		int32_t ContextType = (int32_t)PlatformOpenGLCurrentContext(m_Context.PlatformDevice);
		if (bAssertIfInvalid)
		{
			assert(ContextType >= 0);
		}
		else if (ContextType < 0)
		{
			return InvalidContextState;
		}

		if (ContextType == CONTEXT_Rendering)
		{
			return RenderingContextState;
		}
		else
		{
			return SharedContextState;
		}
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

	void Device::SetPendingBlendStateForActiveRenderTargets(FOpenGLContextState& ContextState)
	{
		bool bABlendWasSet = false;
		bool bMSAAEnabled = false;

		//
		// Need to expand setting for glBlendFunction and glBlendEquation

		for (uint32_t RenderTargetIndex = 0; RenderTargetIndex < c_MaxRenderTargets; ++RenderTargetIndex)
		{
			if (PendingState.RenderTargets[RenderTargetIndex] == 0)
			{
				// Even if on this stage blend states are incompatible with other stages, we can disregard it, as no render target is assigned to it.
				continue;
			}
			else if (RenderTargetIndex == 0)
			{
				Texture* RenderTarget2D = PendingState.RenderTargets[RenderTargetIndex];
				bMSAAEnabled = PendingState.NumRenderingSamples > 1 || RenderTarget2D->IsMultisampled();
			}

			const FOpenGLBlendStateData::FRenderTarget& RenderTargetBlendState = PendingState.BlendState.RenderTargets[RenderTargetIndex];
			FOpenGLBlendStateData::FRenderTarget& CachedRenderTargetBlendState = ContextState.BlendState.RenderTargets[RenderTargetIndex];

			if (CachedRenderTargetBlendState.bAlphaBlendEnable != RenderTargetBlendState.bAlphaBlendEnable)
			{
				if (RenderTargetBlendState.bAlphaBlendEnable)
				{
					FOpenGL::EnableIndexed(GL_BLEND, RenderTargetIndex);
				}
				else
				{
					FOpenGL::DisableIndexed(GL_BLEND, RenderTargetIndex);
				}
				CachedRenderTargetBlendState.bAlphaBlendEnable = RenderTargetBlendState.bAlphaBlendEnable;
			}

			if (RenderTargetBlendState.bAlphaBlendEnable)
			{
				if (FOpenGL::SupportsSeparateAlphaBlend())
				{
					// Set current blend per stage
					if (RenderTargetBlendState.bSeparateAlphaBlendEnable)
					{
						if (CachedRenderTargetBlendState.ColorSourceBlendFactor != RenderTargetBlendState.ColorSourceBlendFactor
							|| CachedRenderTargetBlendState.ColorDestBlendFactor != RenderTargetBlendState.ColorDestBlendFactor
							|| CachedRenderTargetBlendState.AlphaSourceBlendFactor != RenderTargetBlendState.AlphaSourceBlendFactor
							|| CachedRenderTargetBlendState.AlphaDestBlendFactor != RenderTargetBlendState.AlphaDestBlendFactor)
						{
							FOpenGL::BlendFuncSeparatei(
								RenderTargetIndex,
								RenderTargetBlendState.ColorSourceBlendFactor, RenderTargetBlendState.ColorDestBlendFactor,
								RenderTargetBlendState.AlphaSourceBlendFactor, RenderTargetBlendState.AlphaDestBlendFactor
							);
						}

						if (CachedRenderTargetBlendState.ColorBlendOperation != RenderTargetBlendState.ColorBlendOperation
							|| CachedRenderTargetBlendState.AlphaBlendOperation != RenderTargetBlendState.AlphaBlendOperation)
						{
							FOpenGL::BlendEquationSeparatei(
								RenderTargetIndex,
								RenderTargetBlendState.ColorBlendOperation,
								RenderTargetBlendState.AlphaBlendOperation
							);
						}
					}
					else
					{
						if (CachedRenderTargetBlendState.ColorSourceBlendFactor != RenderTargetBlendState.ColorSourceBlendFactor
							|| CachedRenderTargetBlendState.ColorDestBlendFactor != RenderTargetBlendState.ColorDestBlendFactor
							|| CachedRenderTargetBlendState.AlphaSourceBlendFactor != RenderTargetBlendState.AlphaSourceBlendFactor
							|| CachedRenderTargetBlendState.AlphaDestBlendFactor != RenderTargetBlendState.AlphaDestBlendFactor)
						{
							FOpenGL::BlendFunci(RenderTargetIndex, RenderTargetBlendState.ColorSourceBlendFactor, RenderTargetBlendState.ColorDestBlendFactor);
						}

						if (CachedRenderTargetBlendState.ColorBlendOperation != RenderTargetBlendState.ColorBlendOperation)
						{
							FOpenGL::BlendEquationi(RenderTargetIndex, RenderTargetBlendState.ColorBlendOperation);
						}
					}

					CachedRenderTargetBlendState.bSeparateAlphaBlendEnable = RenderTargetBlendState.bSeparateAlphaBlendEnable;
					CachedRenderTargetBlendState.ColorBlendOperation = RenderTargetBlendState.ColorBlendOperation;
					CachedRenderTargetBlendState.ColorSourceBlendFactor = RenderTargetBlendState.ColorSourceBlendFactor;
					CachedRenderTargetBlendState.ColorDestBlendFactor = RenderTargetBlendState.ColorDestBlendFactor;
					if (RenderTargetBlendState.bSeparateAlphaBlendEnable)
					{
						CachedRenderTargetBlendState.AlphaSourceBlendFactor = RenderTargetBlendState.AlphaSourceBlendFactor;
						CachedRenderTargetBlendState.AlphaDestBlendFactor = RenderTargetBlendState.AlphaDestBlendFactor;
					}
					else
					{
						CachedRenderTargetBlendState.AlphaSourceBlendFactor = RenderTargetBlendState.ColorSourceBlendFactor;
						CachedRenderTargetBlendState.AlphaDestBlendFactor = RenderTargetBlendState.ColorDestBlendFactor;
					}
				}
				else
				{
					if (bABlendWasSet)
					{
						// Detect the case of subsequent render target needing different blend setup than one already set in this call.
						if (CachedRenderTargetBlendState.bSeparateAlphaBlendEnable != RenderTargetBlendState.bSeparateAlphaBlendEnable
							|| CachedRenderTargetBlendState.ColorBlendOperation != RenderTargetBlendState.ColorBlendOperation
							|| CachedRenderTargetBlendState.ColorSourceBlendFactor != RenderTargetBlendState.ColorSourceBlendFactor
							|| CachedRenderTargetBlendState.ColorDestBlendFactor != RenderTargetBlendState.ColorDestBlendFactor
							|| (RenderTargetBlendState.bSeparateAlphaBlendEnable &&
								(CachedRenderTargetBlendState.AlphaSourceBlendFactor != RenderTargetBlendState.AlphaSourceBlendFactor
									|| CachedRenderTargetBlendState.AlphaDestBlendFactor != RenderTargetBlendState.AlphaDestBlendFactor
									)
								)
							)
							GE_CORE_INFO("OpenGL state on draw requires setting different blend operation or factors to different render targets. This is not supported on Mac OS X!");
					}
					else
					{
						// Set current blend to all stages
						if (RenderTargetBlendState.bSeparateAlphaBlendEnable)
						{
							if (CachedRenderTargetBlendState.ColorSourceBlendFactor != RenderTargetBlendState.ColorSourceBlendFactor
								|| CachedRenderTargetBlendState.ColorDestBlendFactor != RenderTargetBlendState.ColorDestBlendFactor
								|| CachedRenderTargetBlendState.AlphaSourceBlendFactor != RenderTargetBlendState.AlphaSourceBlendFactor
								|| CachedRenderTargetBlendState.AlphaDestBlendFactor != RenderTargetBlendState.AlphaDestBlendFactor)
							{
								glBlendFuncSeparate(
									RenderTargetBlendState.ColorSourceBlendFactor, RenderTargetBlendState.ColorDestBlendFactor,
									RenderTargetBlendState.AlphaSourceBlendFactor, RenderTargetBlendState.AlphaDestBlendFactor
								);
							}

							if (CachedRenderTargetBlendState.ColorBlendOperation != RenderTargetBlendState.ColorBlendOperation
								|| CachedRenderTargetBlendState.AlphaBlendOperation != RenderTargetBlendState.AlphaBlendOperation)
							{
								glBlendEquationSeparate(
									RenderTargetBlendState.ColorBlendOperation,
									RenderTargetBlendState.AlphaBlendOperation
								);
							}
						}
						else
						{
							if (CachedRenderTargetBlendState.ColorSourceBlendFactor != RenderTargetBlendState.ColorSourceBlendFactor
								|| CachedRenderTargetBlendState.ColorDestBlendFactor != RenderTargetBlendState.ColorDestBlendFactor
								|| CachedRenderTargetBlendState.AlphaSourceBlendFactor != RenderTargetBlendState.AlphaSourceBlendFactor
								|| CachedRenderTargetBlendState.AlphaDestBlendFactor != RenderTargetBlendState.AlphaDestBlendFactor)
							{
								glBlendFunc(RenderTargetBlendState.ColorSourceBlendFactor, RenderTargetBlendState.ColorDestBlendFactor);
							}

							if (CachedRenderTargetBlendState.ColorBlendOperation != RenderTargetBlendState.ColorBlendOperation
								|| CachedRenderTargetBlendState.AlphaBlendOperation != RenderTargetBlendState.ColorBlendOperation)
							{
								glBlendEquation(RenderTargetBlendState.ColorBlendOperation);
							}
						}

						// Set cached values of all stages to what they were set by global calls, common to all stages
						for (uint32_t RenderTargetIndex2 = 0; RenderTargetIndex2 < c_MaxRenderTargets; ++RenderTargetIndex2)
						{
							FOpenGLBlendStateData::FRenderTarget& CachedRenderTargetBlendState2 = ContextState.BlendState.RenderTargets[RenderTargetIndex2];
							CachedRenderTargetBlendState2.bSeparateAlphaBlendEnable = RenderTargetBlendState.bSeparateAlphaBlendEnable;
							CachedRenderTargetBlendState2.ColorBlendOperation = RenderTargetBlendState.ColorBlendOperation;
							CachedRenderTargetBlendState2.ColorSourceBlendFactor = RenderTargetBlendState.ColorSourceBlendFactor;
							CachedRenderTargetBlendState2.ColorDestBlendFactor = RenderTargetBlendState.ColorDestBlendFactor;
							if (RenderTargetBlendState.bSeparateAlphaBlendEnable)
							{
								CachedRenderTargetBlendState2.AlphaBlendOperation = RenderTargetBlendState.AlphaBlendOperation;
								CachedRenderTargetBlendState2.AlphaSourceBlendFactor = RenderTargetBlendState.AlphaSourceBlendFactor;
								CachedRenderTargetBlendState2.AlphaDestBlendFactor = RenderTargetBlendState.AlphaDestBlendFactor;
								CachedRenderTargetBlendState2.AlphaBlendOperation = RenderTargetBlendState.AlphaBlendOperation;
							}
							else
							{
								CachedRenderTargetBlendState2.AlphaBlendOperation = RenderTargetBlendState.ColorBlendOperation;
								CachedRenderTargetBlendState2.AlphaSourceBlendFactor = RenderTargetBlendState.ColorSourceBlendFactor;
								CachedRenderTargetBlendState2.AlphaDestBlendFactor = RenderTargetBlendState.ColorDestBlendFactor;
								CachedRenderTargetBlendState2.AlphaBlendOperation = RenderTargetBlendState.ColorBlendOperation;
							}
						}

						bABlendWasSet = true;
					}
				}
			}

			CachedRenderTargetBlendState.bSeparateAlphaBlendEnable = RenderTargetBlendState.bSeparateAlphaBlendEnable;

			if (CachedRenderTargetBlendState.ColorWriteMaskR != RenderTargetBlendState.ColorWriteMaskR
				|| CachedRenderTargetBlendState.ColorWriteMaskG != RenderTargetBlendState.ColorWriteMaskG
				|| CachedRenderTargetBlendState.ColorWriteMaskB != RenderTargetBlendState.ColorWriteMaskB
				|| CachedRenderTargetBlendState.ColorWriteMaskA != RenderTargetBlendState.ColorWriteMaskA)
			{
				FOpenGL::ColorMaskIndexed(
					RenderTargetIndex,
					RenderTargetBlendState.ColorWriteMaskR,
					RenderTargetBlendState.ColorWriteMaskG,
					RenderTargetBlendState.ColorWriteMaskB,
					RenderTargetBlendState.ColorWriteMaskA
				);

				CachedRenderTargetBlendState.ColorWriteMaskR = RenderTargetBlendState.ColorWriteMaskR;
				CachedRenderTargetBlendState.ColorWriteMaskG = RenderTargetBlendState.ColorWriteMaskG;
				CachedRenderTargetBlendState.ColorWriteMaskB = RenderTargetBlendState.ColorWriteMaskB;
				CachedRenderTargetBlendState.ColorWriteMaskA = RenderTargetBlendState.ColorWriteMaskA;
			}
		}

		PendingState.bAlphaToCoverageEnabled = bMSAAEnabled && PendingState.BlendState.bUseAlphaToCoverage;
		if (PendingState.bAlphaToCoverageEnabled != ContextState.bAlphaToCoverageEnabled)
		{
			if (PendingState.bAlphaToCoverageEnabled)
			{
				glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
			else
			{
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}

			ContextState.bAlphaToCoverageEnabled = PendingState.bAlphaToCoverageEnabled;
		}
	}

	void Device::CachedBindArrayBuffer(FOpenGLContextState& ContextState, GLuint Buffer)
	{
		
			//	VERIFY_GL_SCOPE();
			if (ContextState.ArrayBufferBound != Buffer)
			{
				glBindBuffer(GL_ARRAY_BUFFER, Buffer);
				ContextState.ArrayBufferBound = Buffer;
			}
		

	}
	void Device::CachedBindElementArrayBuffer(FOpenGLContextState& ContextState, GLuint Buffer)
	{
		
			//VERIFY_GL_SCOPE();
			if (ContextState.ElementArrayBufferBound != Buffer)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer);
				ContextState.ElementArrayBufferBound = Buffer;
			}
		
	}
	void Device::SetupTexturesForDraw(FOpenGLContextState& ContextState)
	{
		SetupTexturesForDraw(ContextState, PendingState.BoundShaderState, FOpenGL::GetMaxCombinedTextureImageUnits());

	}


	void Device::CachedSetupUAVStage(FOpenGLContextState& ContextState, GLint UAVIndex, GLenum Format, GLuint Resource, bool bLayered, GLint Layer, GLenum Access)
	{
		//VERIFY_GL_SCOPE();

		FUAVStage& UAVStage = ContextState.UAVs[UAVIndex];

		if (UAVStage.Format == Format &&
			UAVStage.Resource == Resource &&
			UAVStage.Access == Access &&
			UAVStage.Layer == Layer &&
			UAVStage.bLayered == bLayered)
		{
			// Nothing's changed, no need to update
			return;
		}

		// unbind any SSBO or Image in this slot
		if (Resource == 0)
		{
			if (UAVStage.Resource != 0)
			{
				// SSBO
				if (UAVStage.Format == 0)
				{
					FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, UAVIndex, 0);
					ContextState.StorageBufferBound = 0;
				}
				else // Image
				{
					FOpenGL::BindImageTexture(UAVIndex, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
				}

				UAVStage.Format = 0;
				UAVStage.Resource = 0;
				UAVStage.Access = GL_READ_WRITE;
				UAVStage.Layer = 0;
				UAVStage.bLayered = false;
			}
		}
		else
		{
			// SSBO
			if (Format == 0)
			{
				// make sure we dont end up binding both SSBO and Image to the same UAV slot
				if (UAVStage.Resource != 0 && UAVStage.Format != 0)
				{
					FOpenGL::BindImageTexture(UAVIndex, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
				}

				FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, UAVIndex, Resource);

				UAVStage.Format = 0;
				UAVStage.Resource = Resource;
				UAVStage.Access = GL_READ_WRITE;
				UAVStage.Layer = 0;
				UAVStage.bLayered = false;
				ContextState.StorageBufferBound = Resource;
			}
			else // Image
			{
				// make sure we dont end up binding both SSBO and Image to the same UAV slot
				if (UAVStage.Resource != 0 && UAVStage.Format == 0)
				{
					FOpenGL::BindBufferBase(GL_SHADER_STORAGE_BUFFER, UAVIndex, 0);
					ContextState.StorageBufferBound = 0;
				}

				//assert(IsImageTextureFormatSupported(Format));

				FOpenGL::BindImageTexture(UAVIndex, Resource, 0, bLayered ? GL_TRUE : GL_FALSE, Layer, Access, Format);

				UAVStage.Format = Format;
				UAVStage.Resource = Resource;
				UAVStage.Access = Access;
				UAVStage.Layer = Layer;
				UAVStage.bLayered = bLayered;
			}
		}

		uint32_t UAVBit = 1 << UAVIndex;
		if (Resource != 0)
		{
			ContextState.ActiveUAVMask |= UAVBit;
		}
		else
		{
			ContextState.ActiveUAVMask &= ~UAVBit;
		}
	}

	void Device::SetupUAVsForProgram(FOpenGLContextState& ContextState, const TBitArray& NeededBits, int32_t MaxUAVUnitUsed)
	{
		if (MaxUAVUnitUsed < 0 && ContextState.ActiveUAVMask == 0)
		{
			// Quit early if program does not use UAVs and context has no active UAV units
			return;
		}

		for (int32_t UAVStageIndex = 0; UAVStageIndex <= MaxUAVUnitUsed; ++UAVStageIndex)
		{
			if (!NeededBits[UAVStageIndex])
			{
				CachedSetupUAVStage(ContextState, UAVStageIndex, 0, 0, false, 0, GL_READ_WRITE);
			}
			else
			{
				const FUAVStage& UAVStage = PendingState.UAVs[UAVStageIndex];
				CachedSetupUAVStage(ContextState, UAVStageIndex, UAVStage.Format, UAVStage.Resource, UAVStage.bLayered, UAVStage.Layer, UAVStage.Access);
			}
		}

		// clear rest of the units
		int32_t UAVStageIndex = (MaxUAVUnitUsed + 1);
		if ((ContextState.ActiveUAVMask >> UAVStageIndex) != 0)
		{
			const int32_t NumUAVs = ContextState.UAVs.size();
			for (; UAVStageIndex < NumUAVs; ++UAVStageIndex)
			{
				CachedSetupUAVStage(ContextState, UAVStageIndex, 0, 0, false, 0, GL_READ_WRITE);
			}
		}
	}
	void Device::SetupUAVsForDraw(FOpenGLContextState& ContextState)
	{
		int32_t MaxUAVUnitUsed = 0;
		const TBitArray& NeededBits = PendingState.BoundShaderState->GetUAVNeeds(MaxUAVUnitUsed);
		SetupUAVsForProgram(ContextState, NeededBits, MaxUAVUnitUsed);
	}
	void Device::SetupUAVsForCompute(FOpenGLContextState& ContextState, const Shader* ComputeShader)
	{
	}
	void Device::RHIClearMRT(const bool* bClearColorArray, int32_t NumClearColors, const Color* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32_t Stencil)
	{
	}

  


//	template <typename StateType>
//	void Device::SetupTexturesForDraw(FOpenGLContextState& ContextState, const StateType& ShaderState, int32_t MaxTexturesNeeded)
//	{
//		//VERIFY_GL_SCOPE();
//		//SCOPE_CYCLE_COUNTER_DETAILED(STAT_OpenGLTextureBindTime);
//
//		int32_t MaxProgramTexture = 0;
//		const TBitArray<>& NeededBits = ShaderState->GetTextureNeeds(MaxProgramTexture);
//
//		for (int32_t TextureStageIndex = 0; TextureStageIndex <= MaxProgramTexture; ++TextureStageIndex)
//		{
//			if (!NeededBits[TextureStageIndex])
//			{
//				// Current program doesn't make use of this texture stage. No matter what UnrealEditor wants to have on in,
//				// it won't be useful for this draw, so telling OpenGL we don't really need it to give the driver
//				// more leeway in memory management, and avoid false alarms about same texture being set on
//				// texture stage and in framebuffer.
//				//CachedSetupTextureStage(ContextState, TextureStageIndex, GL_NONE, 0, -1, 1);
//			}
//			else
//			{
//				const FTextureStage& TextureStage = PendingState.Textures[TextureStageIndex];
//
//				//CachedSetupTextureStage(ContextState, TextureStageIndex, TextureStage.Target, TextureStage.Resource, TextureStage.LimitMip, TextureStage.NumMips);
//
//				bool bExternalTexture = (TextureStage.Target == GL_TEXTURE_EXTERNAL_OES);
//				if (!bExternalTexture)
//				{
//					FOpenGLSamplerState* PendingSampler = PendingState.SamplerStates[TextureStageIndex];
//
//					if (ContextState.SamplerStates[TextureStageIndex] != PendingSampler)
//					{
//						FOpenGL::BindSampler(TextureStageIndex, PendingSampler ? PendingSampler->Resource : 0);
//						ContextState.SamplerStates[TextureStageIndex] = PendingSampler;
//					}
//				}
//				else if (TextureStage.Target != GL_TEXTURE_BUFFER)
//				{
//					FOpenGL::BindSampler(TextureStageIndex, 0);
//					ContextState.SamplerStates[TextureStageIndex] = nullptr;
//					ApplyTextureStage(ContextState, TextureStageIndex, TextureStage, PendingState.SamplerStates[TextureStageIndex]);
//				}
//			}
//		}
//
//		// For now, continue to clear unused stages
//		for (int32_t TextureStageIndex = MaxProgramTexture + 1; TextureStageIndex < MaxTexturesNeeded; ++TextureStageIndex)
//		{
//			CachedSetupTextureStage(ContextState, TextureStageIndex, GL_NONE, 0, -1, 1);
//		}
//	}
//
//

	void Device::SetupVertexArrays(FOpenGLContextState& ContextState, uint32_t BaseVertexIndex, FOpenGLStream* Streams, uint32_t NumStreams, uint32_t MaxVertices)
	{
		//VERIFY_GL_SCOPE();
		bool KnowsDivisor[NUM_OPENGL_VERTEX_STREAMS] = { 0 };
		uint32_t Divisor[NUM_OPENGL_VERTEX_STREAMS] = { 0 };
		bool UpdateDivisors = false;
		uint32_t StreamMask = ContextState.ActiveStreamMask;

		//check(IsValidRef(PendingState.BoundShaderState));
		InputLayout* VertexDeclaration = PendingState.BoundShaderState->VertexDeclaration;
		const CrossCompiler::FShaderBindingInOutMask& AttributeMask = PendingState.BoundShaderState->GetVertexShader()->Bindings.InOutMask;

		if (ContextState.VertexDecl != VertexDeclaration || AttributeMask.Bitmask != ContextState.VertexAttrs_EnabledBits)
		{
			StreamMask = 0;
			UpdateDivisors = true;

			check(VertexDeclaration->VertexElements.Num() <= 32);

			for (int32_t ElementIndex = 0; ElementIndex < VertexDeclaration->VertexElements.Num(); ElementIndex++)
			{
				FOpenGLVertexElement& VertexElement = VertexDeclaration->VertexElements[ElementIndex];
				uint32_t AttributeIndex = VertexElement.AttributeIndex;
				const uint32_t StreamIndex = VertexElement.StreamIndex;

				//only setup/track attributes actually in use
				FOpenGLCachedAttr& Attr = ContextState.VertexAttrs[AttributeIndex];
				if (AttributeMask.IsFieldEnabled(AttributeIndex))
				{
					if (VertexElement.StreamIndex < NumStreams)
					{
						// Track the actively used streams, to limit the updates to those in use
						StreamMask |= 0x1 << VertexElement.StreamIndex;

						// Verify that the Divisor is consistent across the stream
						check(!KnowsDivisor[StreamIndex] || Divisor[StreamIndex] == VertexElement.Divisor);
						KnowsDivisor[StreamIndex] = true;
						Divisor[StreamIndex] = VertexElement.Divisor;

						if ((Attr.StreamOffset != VertexElement.Offset) || //-V1013
							(Attr.Size != VertexElement.Size) ||
							(Attr.Type != VertexElement.Type) ||
							(Attr.bNormalized != VertexElement.bNormalized) ||
							(Attr.bShouldConvertToFloat != VertexElement.bShouldConvertToFloat))
						{
							if (!VertexElement.bShouldConvertToFloat)
							{
								FOpenGL::VertexAttribIFormat(AttributeIndex, VertexElement.Size, VertexElement.Type, VertexElement.Offset);
							}
							else
							{
								FOpenGL::VertexAttribFormat(AttributeIndex, VertexElement.Size, VertexElement.Type, VertexElement.bNormalized, VertexElement.Offset);
							}

							Attr.StreamOffset = VertexElement.Offset;
							Attr.Size = VertexElement.Size;
							Attr.Type = VertexElement.Type;
							Attr.bNormalized = VertexElement.bNormalized;
							Attr.bShouldConvertToFloat = VertexElement.bShouldConvertToFloat;
						}

						if (Attr.StreamIndex != StreamIndex)
						{
							FOpenGL::VertexAttribBinding(AttributeIndex, VertexElement.StreamIndex);
							Attr.StreamIndex = StreamIndex;
						}

						if (!ContextState.GetVertexAttrEnabled(AttributeIndex))
						{
							ContextState.SetVertexAttrEnabled(AttributeIndex, true);
							glEnableVertexAttribArray(AttributeIndex);
						}
					}
					else
					{
						//workaround attributes with no streams
						VERIFY_GL_SCOPE();

						if (ContextState.GetVertexAttrEnabled(AttributeIndex))
						{
							ContextState.SetVertexAttrEnabled(AttributeIndex, false);
							glDisableVertexAttribArray(AttributeIndex);
						}
						static float data[4] = { 0.0f };
						glVertexAttrib4fv(AttributeIndex, data);
					}
				}
				else
				{
					if (Attr.StreamIndex != StreamIndex)
					{
						FOpenGL::VertexAttribBinding(AttributeIndex, VertexElement.StreamIndex);
						Attr.StreamIndex = StreamIndex;
					}
				}
			}
			ContextState.VertexDecl = VertexDeclaration;
		}

		// Disable remaining vertex arrays
		uint32_t NotUsedButEnabledAttrMask = (ContextState.VertexAttrs_EnabledBits & ~(AttributeMask.Bitmask));
		for (GLuint AttribIndex = 0; AttribIndex < NUM_OPENGL_VERTEX_STREAMS && NotUsedButEnabledAttrMask; AttribIndex++)
		{
			if (NotUsedButEnabledAttrMask & 1)
			{
				glDisableVertexAttribArray(AttribIndex);
				ContextState.SetVertexAttrEnabled(AttribIndex, false);
			}
			NotUsedButEnabledAttrMask >>= 1;
		}

		// Active streams that are no used by this draw
		uint32_t NotUsedButActiveStreamMask = (ContextState.ActiveStreamMask & ~(StreamMask));

		// Update the stream mask
		ContextState.ActiveStreamMask = StreamMask;

		// Enable used streams
		for (uint32_t StreamIndex = 0; StreamIndex < NumStreams && StreamMask; StreamIndex++)
		{
			if (StreamMask & 0x1)
			{
				FOpenGLStream& CachedStream = ContextState.VertexStreams[StreamIndex];
				FOpenGLStream& Stream = Streams[StreamIndex];
				if (Stream.VertexBufferResource)
				{
					uint32_t Offset = BaseVertexIndex * Stream.Stride + Stream.Offset;
					bool bAnyDifferent = //bitwise ors to get rid of the branches
						(CachedStream.VertexBufferResource != Stream.VertexBufferResource) ||
						(CachedStream.Stride != Stream.Stride) ||
						(CachedStream.Offset != Offset);

					if (bAnyDifferent)
					{
						assert(Stream.VertexBufferResource != 0);
						FOpenGL::BindVertexBuffer(StreamIndex, Stream.VertexBufferResource, Offset, Stream.Stride);
						CachedStream.VertexBufferResource = Stream.VertexBufferResource;
						CachedStream.Offset = Offset;
						CachedStream.Stride = Stream.Stride;
					}
					if (UpdateDivisors && CachedStream.Divisor != Divisor[StreamIndex])
					{
						FOpenGL::VertexBindingDivisor(StreamIndex, Divisor[StreamIndex]);
						CachedStream.Divisor = Divisor[StreamIndex];
					}
				}
				else
				{
					UE_LOG(LogRHI, Error, TEXT("Stream %d marked as in use, but vertex buffer provided is NULL (Mask = %x)"), StreamIndex, StreamMask);

					FOpenGL::BindVertexBuffer(StreamIndex, 0, 0, 0);
					CachedStream.VertexBufferResource = 0;
					CachedStream.Offset = 0;
					CachedStream.Stride = 0;
				}
			}
			StreamMask >>= 1;
		}
		//Ensure that all requested streams were set
		assert(StreamMask == 0);

		// Disable active unused streams
		for (uint32_t StreamIndex = 0; StreamIndex < NUM_OPENGL_VERTEX_STREAMS && NotUsedButActiveStreamMask; StreamIndex++)
		{
			if (NotUsedButActiveStreamMask & 0x1)
			{
				FOpenGL::BindVertexBuffer(StreamIndex, 0, 0, 0);
				ContextState.VertexStreams[StreamIndex].VertexBufferResource = 0;
				ContextState.VertexStreams[StreamIndex].Offset = 0;
				ContextState.VertexStreams[StreamIndex].Stride = 0;
			}
			NotUsedButActiveStreamMask >>= 1;
		}
		assert(NotUsedButActiveStreamMask == 0);
	}


    void Device::BindPendingComputeShaderState(FOpenGLContextState& ContextState, IShader* ComputeShader)
    {
    }

    void Device::UpdateRasterizerStateInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (FOpenGL::SupportsPolygonMode() && ContextState.RasterizerState.fillMode != PendingState.RasterizerState.fillMode)
        {
            FOpenGL::PolygonMode(GL_FRONT_AND_BACK, PendingState.RasterizerState.fillMode);
            ContextState.RasterizerState.fillMode = PendingState.RasterizerState.fillMode;
        }

        if (ContextState.RasterizerState.cullMode != PendingState.RasterizerState.cullMode)
        {
            if (OpenGLUtil::convertCullMode(PendingState.RasterizerState.cullMode) != GL_NONE)
            {
                // Only call glEnable if needed
                if (OpenGLUtil::convertCullMode(ContextState.RasterizerState.cullMode) == GL_NONE)
                {
                    glEnable(GL_CULL_FACE);
                }
                glCullFace(OpenGLUtil::convertCullMode(PendingState.RasterizerState.cullMode);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }
            ContextState.RasterizerState.cullMode = PendingState.RasterizerState.cullMode;
        }

        if (FOpenGL::SupportsDepthClamp() && ContextState.RasterizerState.depthClipEnable != PendingState.RasterizerState.depthClipEnable)
        {
            if (PendingState.RasterizerState.depthClipEnable == true)
            {
                glEnable(GL_DEPTH_CLAMP);
            }
            else
            {
                glDisable(GL_DEPTH_CLAMP);
            }
            ContextState.RasterizerState.depthClipEnable = PendingState.RasterizerState.depthClipEnable;
        }

        // Convert our platform independent depth bias into an OpenGL depth bias.
        const float BiasScale = float((1<<24)-1);	// Warning: this assumes depth bits == 24, and won't be correct with 32.
        float DepthBias = PendingState.RasterizerState.depthBias * BiasScale;
        if (ContextState.RasterizerState.depthBias != PendingState.RasterizerState.depthBias
            || ContextState.RasterizerState.slopeScaledDepthBias != PendingState.RasterizerState.slopeScaledDepthBias)
        {
            if ((DepthBias == 0.0f) && (PendingState.RasterizerState.slopeScaledDepthBias == 0.0f))
            {
                // If we're here, both previous 2 'if' conditions are true, and this implies that cached state was not all zeroes, so we need to glDisable.
                glDisable(GL_POLYGON_OFFSET_FILL);
                if ( FOpenGL::SupportsPolygonMode() )
                {
                    glDisable(GL_POLYGON_OFFSET_LINE);
                    glDisable(GL_POLYGON_OFFSET_POINT);
                }
            }
            else
            {
                if (ContextState.RasterizerState.depthBias == 0.0f && ContextState.RasterizerState.slopeScaledDepthBias == 0.0f)
                {
                    glEnable(GL_POLYGON_OFFSET_FILL);
                    if ( FOpenGL::SupportsPolygonMode() )
                    {
                        glEnable(GL_POLYGON_OFFSET_LINE);
                        glEnable(GL_POLYGON_OFFSET_POINT);
                    }
                }
                glPolygonOffset(PendingState.RasterizerState.slopeScaledDepthBias, DepthBias);
            }

            ContextState.RasterizerState.depthBias = PendingState.RasterizerState.depthBias;
            ContextState.RasterizerState.slopeScaledDepthBias = PendingState.RasterizerState.slopeScaledDepthBias;
        }
    }
    void Device::UpdateDepthStencilStateInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (ContextState.DepthStencilState.depthTestEnable != PendingState.DepthStencilState.depthTestEnable)
        {
            if (PendingState.DepthStencilState.depthTestEnable)
            {
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }
            ContextState.DepthStencilState.depthTestEnable = PendingState.DepthStencilState.depthTestEnable;
        }

        if (ContextState.DepthStencilState.depthWriteEnable != PendingState.DepthStencilState.depthWriteEnable)
        {
            glDepthMask((GLboolean)PendingState.DepthStencilState.depthWriteEnable);
            ContextState.DepthStencilState.depthWriteEnable = PendingState.DepthStencilState.depthWriteEnable;
        }

        if (PendingState.DepthStencilState.depthTestEnable)
        {
            if (ContextState.DepthStencilState.depthFunc != PendingState.DepthStencilState.depthFunc)
            {
                glDepthFunc(OpenGLUtil::convertCompareOp(PendingState.DepthStencilState.depthFunc));
                ContextState.DepthStencilState.depthFunc = PendingState.DepthStencilState.depthFunc;
            }
        }

        if (ContextState.DepthStencilState.stencilEnable != PendingState.DepthStencilState.stencilEnable)
        {
            if (PendingState.DepthStencilState.stencilEnable)
            {
                glEnable(GL_STENCIL_TEST);
            }
            else
            {
                glDisable(GL_STENCIL_TEST);
            }
            ContextState.DepthStencilState.stencilEnable = PendingState.DepthStencilState.stencilEnable;
        }

        // If only two-sided <-> one-sided stencil mode changes, and nothing else, we need to call full set of functions
        // to ensure all drivers handle this correctly - some of them might keep those states in different variables.
        if (ContextState.DepthStencilState.bTwoSidedStencilMode != PendingState.DepthStencilState.bTwoSidedStencilMode)
        {
            // Invalidate cache to enforce update of part of stencil state that needs to be set with different functions, when needed next
            // Values below are all invalid, but they'll never be used, only compared to new values to be set.
//            ContextState.DepthStencilState.frontFaceStencil.stencilFunc = 0xFFFF;
//            ContextState.DepthStencilState.frontFaceStencil.failOp = 0xFFFF;
//            ContextState.DepthStencilState.StencilZFail = 0xFFFF;
//            ContextState.DepthStencilState.StencilPass = 0xFFFF;
//            ContextState.DepthStencilState.CCWStencilFunc = 0xFFFF;
//            ContextState.DepthStencilState.CCWStencilFail = 0xFFFF;
//            ContextState.DepthStencilState.CCWStencilZFail = 0xFFFF;
//            ContextState.DepthStencilState.CCWStencilPass = 0xFFFF;
//            ContextState.DepthStencilState.StencilReadMask = 0xFFFF;

            ContextState.DepthStencilState.bTwoSidedStencilMode = PendingState.DepthStencilState.bTwoSidedStencilMode;
        }

        if (PendingState.DepthStencilState.stencilEnable)
        {
            /*
             *  // 为正面多边形设置模板测试
                glStencilFuncSeparate(GL_FRONT, GL_EQUAL, 1, 0xFF);
                // 为背面多边形设置模板测试
                glStencilFuncSeparate(GL_BACK, GL_NOTEQUAL, 2, 0xFF);
             *
             * */
            if (PendingState.DepthStencilState.bTwoSidedStencilMode)
            {
                if (ContextState.DepthStencilState.backFaceStencil.stencilFunc != PendingState.DepthStencilState.backFaceStencil.stencilFunc
                    || ContextState.StencilRef != PendingState.StencilRef
                    || ContextState.DepthStencilState.stencilReadMask != PendingState.DepthStencilState.stencilReadMask)
                {
                    glStencilFuncSeparate(GL_BACK, OpenGLUtil::convertCompareOp(PendingState.DepthStencilState.backFaceStencil.stencilFunc), PendingState.StencilRef, PendingState.DepthStencilState.stencilReadMask);
                    ContextState.DepthStencilState.backFaceStencil.stencilFunc = PendingState.DepthStencilState.backFaceStencil.stencilFunc;
                }

                if (ContextState.DepthStencilState.backFaceStencil.failOp != PendingState.DepthStencilState.backFaceStencil.failOp
                    || ContextState.DepthStencilState.backFaceStencil.depthFailOp != PendingState.DepthStencilState.backFaceStencil.depthFailOp
                    || ContextState.DepthStencilState.backFaceStencil.passOp != PendingState.DepthStencilState.backFaceStencil.passOp)
                {
                    glStencilOpSeparate(GL_BACK, OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.backFaceStencil.failOp),OpenGLUtil::convertStencilOp( PendingState.DepthStencilState.backFaceStencil.depthFailOp), OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.backFaceStencil.passOp));
                    ContextState.DepthStencilState.backFaceStencil.failOp = PendingState.DepthStencilState.backFaceStencil.failOp;
                    ContextState.DepthStencilState.backFaceStencil.depthFailOp = PendingState.DepthStencilState.backFaceStencil.depthFailOp;
                    ContextState.DepthStencilState.backFaceStencil.passOp = PendingState.DepthStencilState.backFaceStencil.passOp;
                }

                if (ContextState.DepthStencilState.frontFaceStencil.stencilFunc != PendingState.DepthStencilState.frontFaceStencil.stencilFunc
                    || ContextState.StencilRef != PendingState.StencilRef
                    || ContextState.DepthStencilState.stencilReadMask != PendingState.DepthStencilState.stencilReadMask)
                {
                    glStencilFuncSeparate(GL_FRONT, OpenGLUtil::convertCompareOp(PendingState.DepthStencilState.frontFaceStencil.stencilFunc), PendingState.StencilRef, PendingState.DepthStencilState.stencilReadMask);
                    ContextState.DepthStencilState.frontFaceStencil.stencilFunc = PendingState.DepthStencilState.frontFaceStencil.stencilFunc;
                }

                if (ContextState.DepthStencilState.frontFaceStencil.failOp != PendingState.DepthStencilState.frontFaceStencil.failOp
                    || ContextState.DepthStencilState.frontFaceStencil.depthFailOp != PendingState.DepthStencilState.frontFaceStencil.depthFailOp
                    || ContextState.DepthStencilState.frontFaceStencil.passOp != PendingState.DepthStencilState.frontFaceStencil.passOp)
                {
                    glStencilOpSeparate(GL_FRONT, OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.frontFaceStencil.failOp), OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.frontFaceStencil.depthFailOp), OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.frontFaceStencil.passOp));
                    ContextState.DepthStencilState.frontFaceStencil.failOp = PendingState.DepthStencilState.frontFaceStencil.failOp;
                    ContextState.DepthStencilState.frontFaceStencil.depthFailOp = PendingState.DepthStencilState.frontFaceStencil.depthFailOp;
                    ContextState.DepthStencilState.frontFaceStencil.passOp = PendingState.DepthStencilState.frontFaceStencil.passOp;
                }

                ContextState.DepthStencilState.stencilReadMask = PendingState.DepthStencilState.stencilReadMask;
                ContextState.StencilRef = PendingState.StencilRef;
            }
            else
            {
                if (ContextState.DepthStencilState.backFaceStencil.stencilFunc != PendingState.DepthStencilState.backFaceStencil.stencilFunc
                    || ContextState.StencilRef != PendingState.StencilRef
                    || ContextState.DepthStencilState.stencilReadMask != PendingState.DepthStencilState.stencilReadMask)
                {
                    glStencilFunc(PendingState.DepthStencilState.stencilReadMask, PendingState.StencilRef, PendingState.DepthStencilState.stencilReadMask);
                    ContextState.DepthStencilState.stencilReadMask = PendingState.DepthStencilState.stencilReadMask;
                    ContextState.DepthStencilState.stencilReadMask = PendingState.DepthStencilState.stencilReadMask;
                    ContextState.StencilRef = PendingState.StencilRef;
                }

                if (ContextState.DepthStencilState.backFaceStencil.failOp != PendingState.DepthStencilState.backFaceStencil.failOp
                    || ContextState.DepthStencilState.backFaceStencil.depthFailOp != PendingState.DepthStencilState.backFaceStencil.depthFailOp
                    || ContextState.DepthStencilState.backFaceStencil.passOp != PendingState.DepthStencilState.backFaceStencil.passOp)
                {
                    glStencilOp(OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.backFaceStencil.failOp), OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.backFaceStencil.depthFailOp), OpenGLUtil::convertStencilOp(PendingState.DepthStencilState.backFaceStencil.passOp));
                    ContextState.DepthStencilState.backFaceStencil.failOp = PendingState.DepthStencilState.backFaceStencil.failOp;
                    ContextState.DepthStencilState.backFaceStencil.depthFailOp = PendingState.DepthStencilState.backFaceStencil.depthFailOp;
                    ContextState.DepthStencilState.backFaceStencil.passOp = PendingState.DepthStencilState.backFaceStencil.passOp;
                }
            }
            /*
             *  void glStencilMask(GLuint mask);
                参数说明
                mask: 指定一个掩码，用于控制模板缓冲区的写入操作。掩码的每一位对应模板缓冲区中的一个位。
                 如果掩码中的某一位为 1，则模板缓冲区中的相应位可以被修改；
                 如果掩码中的某一位为 0，则模板缓冲区中的相应位保持不变。
             *
             * */
            if (ContextState.DepthStencilState.stencilWriteMask != PendingState.DepthStencilState.stencilWriteMask)
            {
                glStencilMask(PendingState.DepthStencilState.stencilWriteMask);
                ContextState.DepthStencilState.stencilWriteMask = PendingState.DepthStencilState.stencilWriteMask;
            }
        }
    }
    void Device::UpdateScissorRectInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (ContextState.bScissorEnabled != PendingState.bScissorEnabled)
        {
            if (PendingState.bScissorEnabled)
            {
                glEnable(GL_SCISSOR_TEST);
            }
            else
            {
                glDisable(GL_SCISSOR_TEST);
            }
            ContextState.bScissorEnabled = PendingState.bScissorEnabled;
        }

        if (PendingState.bScissorEnabled &&
            ContextState.Scissor != PendingState.Scissor)
        {
            assert(PendingState.Scissor.minX <= PendingState.Scissor.maxX);
            assert(PendingState.Scissor.minY <= PendingState.Scissor.maxY);
            glScissor(PendingState.Scissor.minX, PendingState.Scissor.minY, PendingState.Scissor.maxX - PendingState.Scissor.minX, PendingState.Scissor.maxY - PendingState.Scissor.minY);
            ContextState.Scissor = PendingState.Scissor;
        }
    }
    void Device::UpdateViewportInOpenGLContext(FOpenGLContextState& ContextState)
    {
        if (ContextState.Viewport != PendingState.Viewport)
        {
            //@todo the viewport defined by glViewport does not clip, unlike the viewport in d3d
            // Set the scissor rect to the viewport unless it is explicitly set smaller to emulate d3d.
            glViewport(
                    PendingState.Viewport.minX,
                    PendingState.Viewport.minY,
                    PendingState.Viewport.maxX - PendingState.Viewport.minX,
                    PendingState.Viewport.maxY - PendingState.Viewport.minY);

            ContextState.Viewport = PendingState.Viewport;
        }

        if (ContextState.DepthMinZ != PendingState.DepthMinZ || ContextState.DepthMaxZ != PendingState.DepthMaxZ)
        {
            FOpenGL::DepthRange(PendingState.DepthMinZ, PendingState.DepthMaxZ);
            ContextState.DepthMinZ = PendingState.DepthMinZ;
            ContextState.DepthMaxZ = PendingState.DepthMaxZ;
        }
    }




}
