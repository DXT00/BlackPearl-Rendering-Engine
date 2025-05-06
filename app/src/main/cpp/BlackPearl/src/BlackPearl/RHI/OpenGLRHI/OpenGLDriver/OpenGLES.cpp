// Copyright Epic Games, Inc. All Rights Reserved.

/*=============================================================================
	OpenGLES.cpp: OpenGL ES implementation.
=============================================================================*/

//#include "CoreMinimal.h"
//#include "HAL/IConsoleManager.h"
#include "pch.h"
#include "OpenGLDrv.h"
#include "OpenGLDrvPrivate.h"
#ifndef GE_PLATFORM_WINDOWS

#if OPENGL_ES


PFNEGLGETSYSTEMTIMENVPROC eglGetSystemTimeNV_p = NULL;
PFNEGLCREATESYNCKHRPROC eglCreateSyncKHR_p = NULL;
PFNEGLDESTROYSYNCKHRPROC eglDestroySyncKHR_p = NULL;
PFNEGLCLIENTWAITSYNCKHRPROC eglClientWaitSyncKHR_p = NULL;
PFNEGLGETSYNCATTRIBKHRPROC eglGetSyncAttribKHR_p = NULL;

namespace BlackPearl {

    namespace GLFuncPointers {
        // Offscreen MSAA rendering
        PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT = NULL;
        PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT = NULL;

        PFNGLPUSHGROUPMARKEREXTPROC glPushGroupMarkerEXT = NULL;
        PFNGLPOPGROUPMARKEREXTPROC glPopGroupMarkerEXT = NULL;
        PFNGLLABELOBJECTEXTPROC glLabelObjectEXT = NULL;
        PFNGLGETOBJECTLABELEXTPROC glGetObjectLabelEXT = NULL;

        PFNGLBUFFERSTORAGEEXTPROC glBufferStorageEXT = NULL;
        // KHR_debug
        PFNGLDEBUGMESSAGECONTROLKHRPROC glDebugMessageControlKHR = NULL;
        PFNGLDEBUGMESSAGEINSERTKHRPROC glDebugMessageInsertKHR = NULL;
        PFNGLDEBUGMESSAGECALLBACKKHRPROC glDebugMessageCallbackKHR = NULL;
        PFNGLGETDEBUGMESSAGELOGKHRPROC glDebugMessageLogKHR = NULL;
        PFNGLGETPOINTERVKHRPROC glGetPointervKHR = NULL;
        PFNGLPUSHDEBUGGROUPKHRPROC glPushDebugGroupKHR = NULL;
        PFNGLPOPDEBUGGROUPKHRPROC glPopDebugGroupKHR = NULL;
        PFNGLOBJECTLABELKHRPROC glObjectLabelKHR = NULL;
        PFNGLGETOBJECTLABELKHRPROC glGetObjectLabelKHR = NULL;
        PFNGLOBJECTPTRLABELKHRPROC glObjectPtrLabelKHR = NULL;
        PFNGLGETOBJECTPTRLABELKHRPROC glGetObjectPtrLabelKHR = NULL;

        // ES 3.2
        PFNGLTEXBUFFEREXTPROC glTexBufferEXT = nullptr;
        PFNGLTEXBUFFERRANGEEXTPROC glTexBufferRangeEXT = nullptr;
        PFNGLCOPYIMAGESUBDATAEXTPROC glCopyImageSubData = nullptr;
        PFNGLENABLEIEXTPROC glEnableiEXT = nullptr;
        PFNGLDISABLEIEXTPROC glDisableiEXT = nullptr;
        PFNGLBLENDEQUATIONIEXTPROC glBlendEquationiEXT = nullptr;
        PFNGLBLENDEQUATIONSEPARATEIEXTPROC glBlendEquationSeparateiEXT = nullptr;
        PFNGLBLENDFUNCIEXTPROC glBlendFunciEXT = nullptr;
        PFNGLBLENDFUNCSEPARATEIEXTPROC glBlendFuncSeparateiEXT = nullptr;
        PFNGLCOLORMASKIEXTPROC glColorMaskiEXT = nullptr;
        PFNGLFRAMEBUFFERTEXTUREPROC glFramebufferTexture = nullptr;

        PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC glFramebufferTextureMultiviewOVR = NULL;
        PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC glFramebufferTextureMultisampleMultiviewOVR = NULL;
    };

/** GL_EXT_disjoint_timer_query */
    bool FOpenGLES::bSupportsDisjointTimeQueries = false;

//    static TAutoConsoleVariable <int32_t> CVarDisjointTimerQueries(
//            TEXT("r.DisjointTimerQueries"),
//            0,
//            TEXT("If set to 1, allows GPU time to be measured (e.g. STAT UNIT). It defaults to 0 because some devices supports it but very slowly."),
//            ECVF_ReadOnly);

/** Some timer query implementations are never disjoint */
    bool FOpenGLES::bTimerQueryCanBeDisjoint = true;

/** GL_APPLE_texture_format_BGRA8888 */
    bool FOpenGLES::bSupportsBGRA8888 = false;

/** GL_EXT_color_buffer_half_float */
    bool FOpenGLES::bSupportsColorBufferHalfFloat = false;

/** GL_EXT_color_buffer_float */
    bool FOpenGLES::bSupportsColorBufferFloat = false;

/** GL_EXT_shader_framebuffer_fetch */
    bool FOpenGLES::bSupportsShaderFramebufferFetch = false;

/** GL_EXT_shader_framebuffer_fetch (MRT's) */
    bool FOpenGLES::bSupportsShaderMRTFramebufferFetch = false;


/** GL_ARM_shader_framebuffer_fetch_depth_stencil */
    bool FOpenGLES::bSupportsShaderDepthStencilFetch = false;

/** GL_EXT_multisampled_render_to_texture */
    bool FOpenGLES::bSupportsMultisampledRenderToTexture = false;

/** GL_NV_texture_compression_s3tc, GL_EXT_texture_compression_s3tc */
    bool FOpenGLES::bSupportsDXT = false;

/** OpenGL ES 3.0 profile */
    bool FOpenGLES::bSupportsETC2 = false;

/** GL_EXT_shader_pixel_local_storage */
    bool FOpenGLES::bSupportsPixelLocalStorage = false;

/** GL_FRAGMENT_SHADER, GL_LOW_FLOAT */
    int FOpenGLES::ShaderLowPrecision = 0;

/** GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT */
    int FOpenGLES::ShaderMediumPrecision = 0;

/** GL_FRAGMENT_SHADER, GL_HIGH_FLOAT */
    int FOpenGLES::ShaderHighPrecision = 0;

/** GL_NV_framebuffer_blit */
    bool FOpenGLES::bSupportsNVFrameBufferBlit = false;

/* This indicates failure when attempting to retrieve driver's binary representation of the hack program  */
    bool FOpenGLES::bBinaryProgramRetrievalFailed = false;

/* Some Mali devices do not work correctly with early_fragment_test enabled */
    bool FOpenGLES::bRequiresDisabledEarlyFragmentTests = false;

/* This is a workaround for a Mali bug where read-only buffers do not work when passed to functions*/
    bool FOpenGLES::bRequiresReadOnlyBuffersWorkaround = false;

/* This is to avoid a bug in Adreno drivers that define GL_ARM_shader_framebuffer_fetch_depth_stencil even when device does not support this extension  */
    bool FOpenGLES::bRequiresARMShaderFramebufferFetchDepthStencilUndef = false;

/** GL_EXT_buffer_storage */
    bool FOpenGLES::bSupportsBufferStorage = false;

/** GL_EXT_depth_clamp */
    bool FOpenGLES::bSupportsDepthClamp = false;

    bool FOpenGLES::bHasHardwareHiddenSurfaceRemoval = false;
    bool FOpenGLES::bSupportsMobileMultiView = false;
    GLint FOpenGLES::MaxMSAASamplesTileMem = 1;

    GLint FOpenGLES::MaxComputeUniformComponents = -1;

    GLint FOpenGLES::MaxComputeUAVUnits = -1;
    GLint FOpenGLES::MaxPixelUAVUnits = -1;
    GLint FOpenGLES::MaxCombinedUAVUnits = 0;

/** GL_EXT_texture_compression_astc_decode_mode */
    bool FOpenGLES::bSupportsASTCDecodeMode = false;

    FOpenGLES::EFeatureLevelSupport FOpenGLES::CurrentFeatureLevelSupport = FOpenGLES::EFeatureLevelSupport::ES31;

    bool FOpenGLES::SupportsDisjointTimeQueries() {
        bool bAllowDisjointTimerQueries = false;
        bAllowDisjointTimerQueries = (CVarDisjointTimerQueries == 1);
        return bSupportsDisjointTimeQueries && bAllowDisjointTimerQueries;
    }

    void FOpenGLES::ProcessQueryGLInt() {
        GLint MaxVertexAttribs;
        LOG_AND_GET_GL_INT(GL_MAX_VERTEX_ATTRIBS, 0, MaxVertexAttribs);
        if (MaxVertexAttribs < 16) {
            GE_CORE_ERROR("Device reports support for {:d} vertex attributes, UnrealEditor requires 16. Rendering artifacts may occur.",
                   MaxVertexAttribs
            );
        }

        LOG_AND_GET_GL_INT(GL_MAX_VARYING_VECTORS, 0, MaxVaryingVectors);
        LOG_AND_GET_GL_INT(GL_MAX_VERTEX_UNIFORM_VECTORS, 0, MaxVertexUniformComponents);
        LOG_AND_GET_GL_INT(GL_MAX_FRAGMENT_UNIFORM_VECTORS, 0, MaxPixelUniformComponents);
        LOG_AND_GET_GL_INT(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, 0, TextureBufferAlignment);

        LOG_AND_GET_GL_INT(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, 0, MaxComputeUniformComponents);
        LOG_AND_GET_GL_INT(GL_MAX_COMBINED_IMAGE_UNIFORMS, 0, MaxCombinedUAVUnits);
        LOG_AND_GET_GL_INT(GL_MAX_COMPUTE_IMAGE_UNIFORMS, 0, MaxComputeUAVUnits);
        LOG_AND_GET_GL_INT(GL_MAX_FRAGMENT_IMAGE_UNIFORMS, 0, MaxPixelUAVUnits);

        GLint MaxCombinedSSBOUnits = 0;
        GET_GL_INT(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, 0, MaxCombinedSSBOUnits);
        // UAVs slots in UE are shared between Images and SSBO, so this should be max(GL_MAX_COMBINED_IMAGE_UNIFORMS, GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS)
        MaxCombinedUAVUnits = math::max(MaxCombinedUAVUnits, MaxCombinedSSBOUnits);

        // clamp UAV units to a sensible limit
        MaxCombinedUAVUnits = math::min(MaxCombinedUAVUnits, 16);
        MaxComputeUAVUnits = math::min(MaxComputeUAVUnits, 16);
        // this is split between VS and PS, 4 to each stage
        MaxPixelUAVUnits = math::min(MaxPixelUAVUnits, 4);

        const GLint RequiredMaxVertexUniformComponents = 256;
        if (MaxVertexUniformComponents < RequiredMaxVertexUniformComponents) {
           GE_CORE_WARN("Device reports support for {:d} vertex uniform vectors, UnrealEditor requires  {:d}. Rendering artifacts may occur, especially with skeletal meshes. Some drivers, e.g. iOS, report a smaller number than is actually supported.",
                   MaxVertexUniformComponents,
                   RequiredMaxVertexUniformComponents
            );
        }
        MaxVertexUniformComponents = math::max<GLint>(MaxVertexUniformComponents, RequiredMaxVertexUniformComponents);
        MaxGeometryUniformComponents = 0;
        MaxGeometryTextureImageUnits = 0;

        // Set lowest possible limits for texture units, to avoid extra work in GL RHI
        MaxTextureImageUnits = math::min(MaxTextureImageUnits, 16);
        MaxVertexTextureImageUnits = math::min(MaxVertexTextureImageUnits, 16);
        MaxCombinedTextureImageUnits = math::min(MaxCombinedTextureImageUnits, 32);
    }

    void FOpenGLES::ProcessExtensions(const std::string &ExtensionsString) {
        ProcessQueryGLInt();
        FOpenGLBase::ProcessExtensions(ExtensionsString);

        bSupportsDisjointTimeQueries = ExtensionsString.find(("GL_EXT_disjoint_timer_query")) != std::string::npos ||
                                       ExtensionsString.find(("GL_NV_timer_query")) != std::string::npos;
        bTimerQueryCanBeDisjoint = !(ExtensionsString.find(("GL_NV_timer_query"))!= std::string::npos);
        bSupportsBGRA8888 = ExtensionsString.find(("GL_APPLE_texture_format_BGRA8888"))  != std::string::npos||
                            ExtensionsString.find(("GL_IMG_texture_format_BGRA8888")) != std::string::npos ||
                            ExtensionsString.find(("GL_EXT_texture_format_BGRA8888")) != std::string::npos;
        bSupportsColorBufferFloat = ExtensionsString.find(("GL_EXT_color_buffer_float")) != std::string::npos;
        bSupportsColorBufferHalfFloat = ExtensionsString.find(("GL_EXT_color_buffer_half_float")) != std::string::npos;
        bSupportsShaderFramebufferFetch = ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch"))  != std::string::npos||
                                          ExtensionsString.find(("GL_NV_shader_framebuffer_fetch")) != std::string::npos
                                          || ExtensionsString.find(
                ("GL_ARM_shader_framebuffer_fetch ")) != std::string::npos; // has space at the end to exclude GL_ARM_shader_framebuffer_fetch_depth_stencil match
        bSupportsShaderMRTFramebufferFetch = ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch"))  != std::string::npos||
                                             ExtensionsString.find(("GL_NV_shader_framebuffer_fetch")) != std::string::npos;
        bSupportsPixelLocalStorage = ExtensionsString.find(("GL_EXT_shader_pixel_local_storage")) != std::string::npos;
        bSupportsShaderDepthStencilFetch = ExtensionsString.find(
                ("GL_ARM_shader_framebuffer_fetch_depth_stencil")) != std::string::npos;
        bSupportsMultisampledRenderToTexture = ExtensionsString.find(("GL_EXT_multisampled_render_to_texture")) != std::string::npos;
        bSupportsDXT = ExtensionsString.find(("GL_NV_texture_compression_s3tc")) != std::string::npos ||
                       ExtensionsString.find(("GL_EXT_texture_compression_s3tc")) != std::string::npos;
        bSupportsNVFrameBufferBlit = ExtensionsString.find(("GL_NV_framebuffer_blit")) != std::string::npos;
        bSupportsBufferStorage = ExtensionsString.find(("GL_EXT_buffer_storage")) != std::string::npos;
        bSupportsDepthClamp = ExtensionsString.find(("GL_EXT_depth_clamp")) != std::string::npos;
        bSupportsASTCDecodeMode = ExtensionsString.find(("GL_EXT_texture_compression_astc_decode_mode")) != std::string::npos;

        // Report shader precision
        int Range[2];
        glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_LOW_FLOAT, Range, &ShaderLowPrecision);
        glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_MEDIUM_FLOAT, Range, &ShaderMediumPrecision);
        glGetShaderPrecisionFormat(GL_FRAGMENT_SHADER, GL_HIGH_FLOAT, Range, &ShaderHighPrecision);
        GE_CORE_INFO("Fragment shader lowp precision: {:d}", ShaderLowPrecision);
        GE_CORE_INFO("Fragment shader mediump precision: {:d}", ShaderMediumPrecision);
        GE_CORE_INFO("Fragment shader highp precision: {:d}", ShaderHighPrecision);

//        if (FPlatformMisc::IsDebuggerPresent() && UE_BUILD_DEBUG) {
//            // Enable GL debug markers if we're running in Xcode
//            extern int32_t GEmitMeshDrawEvent;
//            GEmitMeshDrawEvent = 1;
//            SetEmitDrawEvents(true);
//        }

        glPushGroupMarkerEXT = (PFNGLPUSHGROUPMARKEREXTPROC) ((void *) eglGetProcAddress("glPushGroupMarkerEXT"));
        glPopGroupMarkerEXT = (PFNGLPOPGROUPMARKEREXTPROC) ((void *) eglGetProcAddress("glPopGroupMarkerEXT"));

        if (ExtensionsString.find(("GL_EXT_DEBUG_LABEL")) != std::string::npos) {
            glLabelObjectEXT = (PFNGLLABELOBJECTEXTPROC) ((void *) eglGetProcAddress("glLabelObjectEXT"));
            glGetObjectLabelEXT = (PFNGLGETOBJECTLABELEXTPROC) ((void *) eglGetProcAddress("glGetObjectLabelEXT"));
        }

        if (bSupportsBufferStorage) {
            glBufferStorageEXT = (PFNGLBUFFERSTORAGEEXTPROC) ((void *) eglGetProcAddress("glBufferStorageEXT"));
        }

        if (ExtensionsString.find(("GL_EXT_multisampled_render_to_texture2")) != std::string::npos) {
            glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) ((void *) eglGetProcAddress(
                    "glFramebufferTexture2DMultisampleEXT"));
            glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) ((void *) eglGetProcAddress(
                    "glRenderbufferStorageMultisampleEXT"));
            glGetIntegerv(GL_MAX_SAMPLES_EXT, &MaxMSAASamplesTileMem);
            MaxMSAASamplesTileMem = math::max<GLint>(MaxMSAASamplesTileMem, 1);
            GE_CORE_INFO("Support for {:x} MSAA detected", MaxMSAASamplesTileMem);
        } else {
            // indicates RHI supports on-chip MSAA but this device does not.
            MaxMSAASamplesTileMem = 1;
        }

        bSupportsETC2 = true;
        // According to https://www.khronos.org/registry/gles/extensions/EXT/EXT_color_buffer_float.txt
        bSupportsColorBufferHalfFloat = (bSupportsColorBufferHalfFloat || bSupportsColorBufferFloat);

        // Mobile multi-view setup
        const bool bMultiViewSupport = ExtensionsString.find(("GL_OVR_multiview")) != std::string::npos;
        const bool bMultiView2Support = ExtensionsString.find(("GL_OVR_multiview2")) != std::string::npos;
        const bool bMultiViewMultiSampleSupport = ExtensionsString.find(
                ("GL_OVR_multiview_multisampled_render_to_texture")) != std::string::npos;
        if (bMultiViewSupport && bMultiView2Support && bMultiViewMultiSampleSupport) {
            glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC) ((void *) eglGetProcAddress(
                    "glFramebufferTextureMultiviewOVR"));
            glFramebufferTextureMultisampleMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTISAMPLEMULTIVIEWOVRPROC) ((void *) eglGetProcAddress(
                    "glFramebufferTextureMultisampleMultiviewOVR"));

            bSupportsMobileMultiView =
                    (glFramebufferTextureMultiviewOVR != NULL) && (glFramebufferTextureMultisampleMultiviewOVR != NULL);

            // Just because the driver declares multi-view support and hands us valid function pointers doesn't actually guarantee the feature works...
            if (bSupportsMobileMultiView) {
               GE_CORE_INFO("Device supports mobile multi-view.");
            }
        }

        if (IsES32Usable()) {
            glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC) ((void *) eglGetProcAddress("glTexBuffer"));
            glTexBufferRangeEXT = (PFNGLTEXBUFFERRANGEEXTPROC) ((void *) eglGetProcAddress("glTexBufferRange"));
            glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAEXTPROC) ((void *) eglGetProcAddress("glCopyImageSubData"));
            glEnableiEXT = (PFNGLENABLEIEXTPROC) ((void *) eglGetProcAddress("glEnablei"));
            glDisableiEXT = (PFNGLDISABLEIEXTPROC) ((void *) eglGetProcAddress("glDisablei"));
            glBlendEquationiEXT = (PFNGLBLENDEQUATIONIEXTPROC) ((void *) eglGetProcAddress("glBlendEquationi"));
            glBlendEquationSeparateiEXT = (PFNGLBLENDEQUATIONSEPARATEIEXTPROC) ((void *) eglGetProcAddress(
                    "glBlendEquationSeparatei"));
            glBlendFunciEXT = (PFNGLBLENDFUNCIEXTPROC) ((void *) eglGetProcAddress("glBlendFunci"));
            glBlendFuncSeparateiEXT = (PFNGLBLENDFUNCSEPARATEIEXTPROC) ((void *) eglGetProcAddress(
                    "glBlendFuncSeparatei"));
            glColorMaskiEXT = (PFNGLCOLORMASKIEXTPROC) ((void *) eglGetProcAddress("glColorMaski"));
            glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)((void *) eglGetProcAddress("glFramebufferTexture"));
        }

        if (!glEnableiEXT && ExtensionsString.find(("GL_EXT_draw_buffers_indexed")) != std::string::npos) {
            // GL_EXT_draw_buffers_indexed
            glEnableiEXT = (PFNGLENABLEIEXTPROC) ((void *) eglGetProcAddress("glEnableiEXT"));
            glDisableiEXT = (PFNGLDISABLEIEXTPROC) ((void *) eglGetProcAddress("glDisableiEXT"));
            glBlendEquationiEXT = (PFNGLBLENDEQUATIONIEXTPROC) ((void *) eglGetProcAddress("glBlendEquationiEXT"));
            glBlendEquationSeparateiEXT = (PFNGLBLENDEQUATIONSEPARATEIEXTPROC) ((void *) eglGetProcAddress(
                    "glBlendEquationSeparateiEXT"));
            glBlendFunciEXT = (PFNGLBLENDFUNCIEXTPROC) ((void *) eglGetProcAddress("glBlendFunciEXT"));
            glBlendFuncSeparateiEXT = (PFNGLBLENDFUNCSEPARATEIEXTPROC) ((void *) eglGetProcAddress(
                    "glBlendFuncSeparateiEXT"));
            glColorMaskiEXT = (PFNGLCOLORMASKIEXTPROC) ((void *) eglGetProcAddress("glColorMaskiEXT"));
        }
        bSupportsDrawBuffersBlend = (glEnableiEXT != nullptr);

        if (!glTexBufferEXT && ExtensionsString.find(("GL_EXT_texture_buffer")) != std::string::npos) {
            // GL_EXT_texture_buffer
            glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC) ((void *) eglGetProcAddress("glTexBufferEXT"));
            glTexBufferRangeEXT = (PFNGLTEXBUFFERRANGEEXTPROC) ((void *) eglGetProcAddress("glTexBufferRangeEXT"));
        }
    }

#endif


}
#endif //desktop