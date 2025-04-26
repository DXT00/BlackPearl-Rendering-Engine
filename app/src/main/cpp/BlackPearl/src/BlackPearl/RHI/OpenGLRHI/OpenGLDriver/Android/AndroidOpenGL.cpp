// Copyright Epic Games, Inc. All Rights Reserved.

//#include "CoreMinimal.h"
//#include "Android/AndroidPlatform.h"
#include "pch.h"

#ifdef GE_PLATFORM_ANDROID

#include "Android/AndroidOpenGL.h"
#include "Android/AndroidOpenGLFramePacer.h"
#include <jni.h>
#include "Android/AndroidEGL.h"
#include "Android/AndroidOpenGLPrivate.h"
#include "OpenGLDrvPrivate.h"
#include "Android/OpenGLDrvAndroid.h"
#include "OpenGLES.h"
#include "BlackPearl/ApplicationCore/Android/AndroidApplication.h"
#include "BlackPearl/Core/Android/AndroidPlatformMisc.h"
#include "RHI/RHIGlobals.h"
#include "RHI/OpenGLRHI/OpenGLViewport.h"


namespace BlackPearl {


    PFNeglPresentationTimeANDROID eglPresentationTimeANDROID_p = NULL;
    PFNeglGetNextFrameIdANDROID eglGetNextFrameIdANDROID_p = NULL;
    PFNeglGetCompositorTimingANDROID eglGetCompositorTimingANDROID_p = NULL;
    PFNeglGetFrameTimestampsANDROID eglGetFrameTimestampsANDROID_p = NULL;
    PFNeglQueryTimestampSupportedANDROID eglQueryTimestampSupportedANDROID_p = NULL;
    PFNeglQueryTimestampSupportedANDROID eglGetCompositorTimingSupportedANDROID_p = NULL;
    PFNeglQueryTimestampSupportedANDROID eglGetFrameTimestampsSupportedANDROID_p = NULL;

    namespace GLFuncPointers {
        PFNGLFRAMEBUFFERFETCHBARRIERQCOMPROC glFramebufferFetchBarrierQCOM = NULL;
    }

    int32_t FAndroidOpenGL::GLMajorVerion = 0;
    int32_t FAndroidOpenGL::GLMinorVersion = 0;

    bool FAndroidOpenGL::bSupportsImageExternal = false;
    bool FAndroidOpenGL::bRequiresAdrenoTilingHint = true;

//static TAutoConsoleVariable<int32_t> CVarEnableAdrenoTilingHint(
//	("r.Android.EnableAdrenoTilingHint"),
//	1,
//	("Whether Adreno-based Android devices should hint to the driver to use tiling mode for the mobile base pass.\n")
//	("  0 = hinting disabled\n")
//	("  1 = hinting enabled for Adreno devices running Andorid 8 or earlier [default]\n")
//	("  2 = hinting always enabled for Adreno devices\n"));
//
//static TAutoConsoleVariable<int32_t> CVarDisableEarlyFragmentTests(
//	("r.Android.DisableEarlyFragmentTests"),
//	0,
//	("Whether to disable early_fragment_tests if any \n"),
//	ECVF_ReadOnly);
//
//static TAutoConsoleVariable<int32_t> CVarDisableFBFNonCoherent(
//	("r.Android.DisableFBFNonCoherent"),
//	0,
//	("Whether to disable usage of QCOM_shader_framebuffer_fetch_noncoherent extension\n"),
//	ECVF_ReadOnly);



// call out to JNI to see if the application was packaged for Oculus Mobile
    extern bool AndroidThunkCpp_IsOculusMobileApplication();


// RenderDoc
#define GL_DEBUG_TOOL_EXT    0x6789
    static bool bRunningUnderRenderDoc = false;


#if UE_BUILD_SHIPPING
#define CHECK_JNI_EXCEPTIONS(env)  env->ExceptionClear();
#else
#define CHECK_JNI_EXCEPTIONS(env)  if (env->ExceptionCheck()) {env->ExceptionDescribe();env->ExceptionClear();}
#endif


    struct FOpenGLRemoteGLProgramCompileJNI
    {
        jclass OGLServiceAccessor = 0;
        jmethodID DispatchProgramLink = 0;
        jmethodID StartRemoteProgramLink = 0;
        jmethodID StopRemoteProgramLink = 0;
        jclass ProgramResponseClass = 0;
        jfieldID ProgramResponse_SuccessField = 0;
        jfieldID ProgramResponse_ErrorField = 0;
        jfieldID ProgramResponse_CompiledBinaryField = 0;
        bool bAllFound = false;

        void Init(JNIEnv* Env)
        {


            GE_ASSERT(OGLServiceAccessor == 0);
//            OGLServiceAccessor = AndroidJavaEnv::FindJavaClassGlobalRef("com/epicgames/unreal/psoservices/PSOProgramServiceAccessor");
//            CHECK_JNI_EXCEPTIONS(Env);
            if(OGLServiceAccessor)
            {
//                DispatchProgramLink = FJavaWrapper::FindStaticMethod(Env, OGLServiceAccessor, "AndroidThunkJava_OGLRemoteProgramLink", "([BLjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lcom/epicgames/unreal/psoservices/PSOProgramServiceAccessor$JNIProgramLinkResponse;", false);
//                CHECK_JNI_EXCEPTIONS(Env);
//                StartRemoteProgramLink = FJavaWrapper::FindStaticMethod(Env, OGLServiceAccessor, "AndroidThunkJava_StartRemoteProgramLink", "(IZ)Z", false);
//                CHECK_JNI_EXCEPTIONS(Env);
//                StopRemoteProgramLink = FJavaWrapper::FindStaticMethod(Env, OGLServiceAccessor, "AndroidThunkJava_StopRemoteProgramLink", "()V", false);
//                CHECK_JNI_EXCEPTIONS(Env);
//                ProgramResponseClass = AndroidJavaEnv::FindJavaClassGlobalRef("com/epicgames/unreal/psoservices/PSOProgramServiceAccessor$JNIProgramLinkResponse");
//                CHECK_JNI_EXCEPTIONS(Env);
//                ProgramResponse_SuccessField = FJavaWrapper::FindField(Env, ProgramResponseClass, "bCompileSuccess", "Z", true);
//                CHECK_JNI_EXCEPTIONS(Env);
//                ProgramResponse_CompiledBinaryField = FJavaWrapper::FindField(Env, ProgramResponseClass, "CompiledProgram", "[B", true);
//                CHECK_JNI_EXCEPTIONS(Env);
//                ProgramResponse_ErrorField = FJavaWrapper::FindField(Env, ProgramResponseClass, "ErrorMessage", "Ljava/lang/String;", true);
//                CHECK_JNI_EXCEPTIONS(Env);
            }

            bAllFound = OGLServiceAccessor && DispatchProgramLink && StartRemoteProgramLink && StopRemoteProgramLink && ProgramResponseClass && ProgramResponse_SuccessField && ProgramResponse_CompiledBinaryField && ProgramResponse_ErrorField;
            //UE_CLOG(!bAllFound, LogRHI, Fatal, TEXT("Failed to find JNI GL remote program compiler."));
        }
    }OpenGLRemoteGLProgramCompileJNI;

    static bool AreAndroidOpenGLRemoteCompileServicesAvailable() {
//        static int RemoteCompileService = -1;
//        if (RemoteCompileService == -1) {
//            const std::string *ConfigRulesDisableProgramCompileServices = FAndroidMisc::GetConfigRulesVariable(
//                    ("DisableProgramCompileServices"));
//            bool bConfigRulesDisableProgramCompileServices = ConfigRulesDisableProgramCompileServices &&
//                                                             ConfigRulesDisableProgramCompileServices->Equals("true",
//                                                                                                              ESearchCase::IgnoreCase);
//            static const auto CVarBinaryProgramCache = IConsoleManager::Get().FindConsoleVariable(
//                    ("r.ProgramBinaryCache.Enable"));
//            static const auto CVarNumRemoteProgramCompileServices = IConsoleManager::Get().FindConsoleVariable(
//                    ("Android.OpenGL.NumRemoteProgramCompileServices"));
//
//            RemoteCompileService =
//                    !bConfigRulesDisableProgramCompileServices && OpenGLRemoteGLProgramCompileJNI.bAllFound &&
//                    (CVarBinaryProgramCache->GetInt() != 0) && (CVarNumRemoteProgramCompileServices->GetInt() > 0);
//            FGenericCrashContext::SetEngineData(("Android.PSOService"),
//                                                RemoteCompileService == 0 ? ("disabled") : ("enabled"));
//            UE_CLOG(!RemoteCompileService, LogRHI, Log, ("Remote PSO services disabled: (%d, %d, %d, %d)"),
//                    bConfigRulesDisableProgramCompileServices, OpenGLRemoteGLProgramCompileJNI.bAllFound,
//                    CVarBinaryProgramCache->GetInt(), CVarNumRemoteProgramCompileServices->GetInt());
//        }
//        return RemoteCompileService;
        return false;
    }
    FPlatformOpenGLDevice::~FPlatformOpenGLDevice() {
        //TODO::
        // FPlatformRHIFramePacer::Destroy();

        FAndroidAppEntry::ReleaseEGL();
    }
    FPlatformOpenGLDevice::FPlatformOpenGLDevice() {
    }

    void FPlatformOpenGLDevice::Init() {
        // Initialize frame pacer
        FPlatformRHIFramePacer::Init(new AndroidOpenGLFramePacer());

        extern void InitDebugContext();

        bRunningUnderRenderDoc = glIsEnabled(GL_DEBUG_TOOL_EXT) != GL_FALSE;

        //FPlatformMisc::LowLevelOutputDebugString(("FPlatformOpenGLDevice:Init"));
        bool bCreateSurface = !AndroidThunkCpp_IsOculusMobileApplication();
        AndroidEGL::GetInstance()->InitSurface(false, bCreateSurface);

        LoadEXT();
        PlatformRenderingContextSetup(this);
        InitDefaultGLContextState();
        InitDebugContext();

        PlatformSharedContextSetup(this);
        InitDefaultGLContextState();
        InitDebugContext();

        AndroidEGL::GetInstance()->InitBackBuffer(); //can be done only after context is made current.

        OpenGLRemoteGLProgramCompileJNI.Init(AndroidApplication::GetJavaEnv());

        // AsyncPipelinePrecompile can be enabled on android GL, precompiles are compiled via separate processes and the result is stored in GL's LRU cache as an evicted binary.
        // The lru cache is a requirement as the precompile produces binary program data only.
        GRHISupportsAsyncPipelinePrecompile = AreAndroidOpenGLRemoteCompileServicesAvailable();
    }

    FPlatformOpenGLDevice *PlatformCreateOpenGLDevice() {
        FPlatformOpenGLDevice *Device = new FPlatformOpenGLDevice();
        Device->Init();
        return Device;
    }

    bool PlatformCanEnableGPUCapture() {
        return bRunningUnderRenderDoc;
    }

    void PlatformReleaseOpenGLContext(FPlatformOpenGLDevice *Device, FPlatformOpenGLContext *Context) {
    }

    void *PlatformGetWindow(FPlatformOpenGLContext *Context, void **AddParam) {
        GE_ASSERT(Context);

        return (void *) &Context->eglContext;
    }

    bool PlatformBlitToViewport(FPlatformOpenGLDevice *Device, const OpenGLViewport &Viewport, uint32_t BackbufferSizeX,
                           uint32_t BackbufferSizeY, bool bPresent, bool bLockToVsync) {
        //SCOPED_NAMED_EVENT(STAT_PlatformBlitToViewportTime, FColor::Red)

        FPlatformOpenGLContext *const Context = Viewport.GetGLContext();

        if (bPresent && AndroidEGL::GetInstance()->IsOfflineSurfaceRequired()) {
            if (Device->TargetDirty) {
                //VERIFY_GL_SCOPE();
                glBindFramebuffer(GL_FRAMEBUFFER, Context->ViewportFramebuffer);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, Context->BackBufferTarget,
                                       Context->BackBufferResource, 0);

                Device->TargetDirty = false;
            }

            {
                //VERIFY_GL_SCOPE();
                glDisable(GL_FRAMEBUFFER_SRGB);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
                FOpenGL::DrawBuffer(GL_BACK);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, Context->ViewportFramebuffer);
                FOpenGL::ReadBuffer(GL_COLOR_ATTACHMENT0);

                FOpenGL::BlitFramebuffer(
                        0, 0, BackbufferSizeX, BackbufferSizeY,
                        0, BackbufferSizeY, BackbufferSizeX, 0,
                        GL_COLOR_BUFFER_BIT,
                        GL_NEAREST
                );

                glEnable(GL_FRAMEBUFFER_SRGB);

                // Bind viewport FBO so driver knows we don't need backbuffer image anymore
                glBindFramebuffer(GL_FRAMEBUFFER, Context->ViewportFramebuffer);
            }
        }
//
//        if (bPresent && Viewport.GetCustomPresent()) {
//            QUICK_SCOPE_CYCLE_COUNTER(STAT_FAndroidOpenGL_PlatformBlitToViewport_CustomPresent);
//            int32_t SyncInterval = FAndroidPlatformRHIFramePacer::GetLegacySyncInterval();
//            bPresent = Viewport.GetCustomPresent()->Present(SyncInterval);
//        }
        if (bPresent) {
            AndroidEGL::GetInstance()->UpdateBuffersTransform();
            FPlatformRHIFramePacer::SwapBuffers(bLockToVsync);
        }
//        static IConsoleVariable *CVar = IConsoleManager::Get().FindConsoleVariable(
//                ("a.UseFrameTimeStampsForPacing"));
//        const bool bForceGPUFence = CVar ? CVar->GetInt() != 0 : false;

        return bPresent && ShouldUseGPUFencesToLimitLatency();
    }

    void PlatformRenderingContextSetup(FPlatformOpenGLDevice *Device) {
        Device->SetCurrentRenderingContext();
        Device->SetupCurrentContext();
    }

    void PlatformFlushIfNeeded() {
    }

    void PlatformRebindResources(FPlatformOpenGLDevice *Device) {
    }

    void PlatformSharedContextSetup(FPlatformOpenGLDevice *Device) {
        Device->SetCurrentSharedContext();
        Device->SetupCurrentContext();
    }

    void PlatformNULLContextSetup() {
        AndroidEGL::GetInstance()->ReleaseContextOwnership();
    }

    EOpenGLCurrentContext PlatformOpenGLCurrentContext(FPlatformOpenGLDevice *Device) {
        return (EOpenGLCurrentContext) AndroidEGL::GetInstance()->GetCurrentContextType();
    }

    void *PlatformOpenGLCurrentContextHandle(FPlatformOpenGLDevice *Device) {
        return AndroidEGL::GetInstance()->GetCurrentContext();
    }

    void PlatformRestoreDesktopDisplayMode() {
    }

    bool PlatformInitOpenGL() {
        //check(!FAndroidMisc::ShouldUseVulkan());

        {
            // determine ES version. PlatformInitOpenGL happens before ProcessExtensions and therefore FAndroidOpenGL::bES31Support.
            std::string FullVersionString, VersionString, SubVersionString;

            FullVersionString = CommonFunc::SplitString(FAndroidGPUInfo::Get().GLVersion,"OpenGL ES ").second;
            FullVersionString = CommonFunc::SplitString(FullVersionString," ").first;
            VersionString = CommonFunc::SplitString(FullVersionString,".").first;
            SubVersionString = CommonFunc::SplitString(FullVersionString,".").second;
//
//            FAndroidGPUInfo::Get().GLVersion.Split(("OpenGL ES "), nullptr, &FullVersionString);
//            FullVersionString.Split((" "), &FullVersionString, nullptr);
//            FullVersionString.Split(("."), &VersionString, &SubVersionString);
            FAndroidOpenGL::GLMajorVerion = atoi(VersionString.c_str());
            FAndroidOpenGL::GLMinorVersion = atoi(SubVersionString.c_str());

            bool bES32Supported = FAndroidOpenGL::GLMajorVerion == 3 && FAndroidOpenGL::GLMinorVersion >= 2;
//            static const auto CVarDisableES31 = IConsoleManager::Get().FindTConsoleVariableDataInt(
//                    ("r.Android.DisableOpenGLES31Support"));

            bool bBuildForES31 = false;
//            GConfig->GetBool(("/Script/AndroidRuntimeSettings.AndroidRuntimeSettings"), ("bBuildForES31"),
//                             bBuildForES31, GEngineIni);

            const bool bSupportsFloatingPointRTs = FAndroidMisc::SupportsFloatingPointRenderTargets();

            if (bBuildForES31 && bES32Supported) {
                FOpenGLES::CurrentFeatureLevelSupport =
                        FAndroidOpenGL::GLMinorVersion >= 2 ? FOpenGLES::EFeatureLevelSupport::ES32
                                                            : FOpenGLES::EFeatureLevelSupport::ES31;
                GE_CORE_INFO(
                       ("App is packaged for OpenGL ES 3.1 and an ES %d.%d-capable device was detected."),
                       FAndroidOpenGL::GLMajorVerion, FAndroidOpenGL::GLMinorVersion);
            } else {
                std::string Message = ("");

                if (bES32Supported) {
                    Message+=(
                            ("This device does not support Vulkan but the app was not packaged with ES 3.1 support."));
//                    if (FAndroidMisc::GetAndroidBuildVersion() < 26) {
//                        Message+=((" Updating to a newer Android version may resolve this issue."));
//                    }
                    GE_CORE_INFO("%s",Message.c_str());
//                    FPlatformMisc::LowLevelOutputDebugString(*Message);
//                    FAndroidMisc::MessageBoxExt(EAppMsgType::Ok, *Message, ("Unable to run on this device!"));
                } else {
                    Message+=(
                            ("This device only supports OpenGL ES 2/3/3.1 which is not supported, only supports ES 3.2+ "));
                    //FPlatformMisc::LowLevelOutputDebugString(*Message);
                   // FAndroidMisc::MessageBoxExt(EAppMsgType::Ok, *Message, ("Unable to run on this device!"));
                }
            }

            // Needs to initialize GPU vendor id before AndroidEGL::AcquireCurrentRenderingContext
            std::string &VendorName = FAndroidGPUInfo::Get().VendorName;
            if (VendorName.find(("ImgTec")) != std::string::npos|| VendorName.find(("Imagination"))!= std::string::npos) {
                GRHIVendorId = 0x1010;
            } else if (VendorName.find(("ARM"))!= std::string::npos) {
                GRHIVendorId = 0x13B5;
            } else if (VendorName.find(("Qualcomm"))!= std::string::npos) {
                GRHIVendorId = 0x5143;
            }
        }
        return true;
    }

    bool PlatformOpenGLContextValid() {
        return AndroidEGL::GetInstance()->IsCurrentContextValid();
    }

    void PlatformGetBackbufferDimensions(uint32_t &OutWidth, uint32_t &OutHeight) {
        AndroidEGL::GetInstance()->GetDimensions(OutWidth, OutHeight);
    }

// =============================================================

    void PlatformGetNewOcclusionQuery(GLuint *OutQuery, uint64_t *OutQueryContext) {
    }

    bool PlatformContextIsCurrent(uint64_t QueryContext) {
        return true;
    }

    void FPlatformOpenGLDevice::LoadEXT() {
        eglGetSystemTimeNV_p = (PFNEGLGETSYSTEMTIMENVPROC) ((void *) eglGetProcAddress("eglGetSystemTimeNV"));
        eglCreateSyncKHR_p = (PFNEGLCREATESYNCKHRPROC) ((void *) eglGetProcAddress("eglCreateSyncKHR"));
        eglDestroySyncKHR_p = (PFNEGLDESTROYSYNCKHRPROC) ((void *) eglGetProcAddress("eglDestroySyncKHR"));
        eglClientWaitSyncKHR_p = (PFNEGLCLIENTWAITSYNCKHRPROC) ((void *) eglGetProcAddress("eglClientWaitSyncKHR"));
        eglGetSyncAttribKHR_p = (PFNEGLGETSYNCATTRIBKHRPROC) ((void *) eglGetProcAddress("eglGetSyncAttribKHR"));

        eglPresentationTimeANDROID_p = (PFNeglPresentationTimeANDROID) (
                (void *) eglGetProcAddress("eglPresentationTimeANDROID"));
        eglGetNextFrameIdANDROID_p = (PFNeglGetNextFrameIdANDROID) (
                (void *) eglGetProcAddress("eglGetNextFrameIdANDROID"));
        eglGetCompositorTimingANDROID_p = (PFNeglGetCompositorTimingANDROID) (
                (void *) eglGetProcAddress("eglGetCompositorTimingANDROID"));
        eglGetFrameTimestampsANDROID_p = (PFNeglGetFrameTimestampsANDROID) (
                (void *) eglGetProcAddress("eglGetFrameTimestampsANDROID"));
        eglQueryTimestampSupportedANDROID_p = (PFNeglQueryTimestampSupportedANDROID) (
                (void *) eglGetProcAddress("eglQueryTimestampSupportedANDROID"));
        eglGetCompositorTimingSupportedANDROID_p = (PFNeglQueryTimestampSupportedANDROID) (
                (void *) eglGetProcAddress("eglGetCompositorTimingSupportedANDROID"));
        eglGetFrameTimestampsSupportedANDROID_p = (PFNeglQueryTimestampSupportedANDROID) (
                (void *) eglGetProcAddress("eglGetFrameTimestampsSupportedANDROID"));

        const char *NotAvailable = ("NOT Available");
        const char *Present = ("Present");

//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglPresentationTimeANDROID"),
//               eglPresentationTimeANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglGetNextFrameIdANDROID"),
//               eglGetNextFrameIdANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglGetCompositorTimingANDROID"),
//               eglGetCompositorTimingANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglGetFrameTimestampsANDROID"),
//               eglGetFrameTimestampsANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglQueryTimestampSupportedANDROID"),
//               eglQueryTimestampSupportedANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglGetCompositorTimingSupportedANDROID"),
//               eglGetCompositorTimingSupportedANDROID_p ? Present : NotAvailable);
//        UE_LOG(LogRHI, Log, ("Extension %s %s"), ("eglGetFrameTimestampsSupportedANDROID"),
//               eglGetFrameTimestampsSupportedANDROID_p ? Present : NotAvailable);

        glDebugMessageControlKHR = (PFNGLDEBUGMESSAGECONTROLKHRPROC) ((void *) eglGetProcAddress(
                "glDebugMessageControlKHR"));

        // Some PowerVR drivers (Rogue Han and Intel-based devices) are crashing using glDebugMessageControlKHR (causes signal 11 crash)
        if (glDebugMessageControlKHR != NULL && FAndroidMisc::GetGPUFamily().find(("PowerVR"))!=std::string::npos) {
            glDebugMessageControlKHR = NULL;
        }

        glDebugMessageInsertKHR = (PFNGLDEBUGMESSAGEINSERTKHRPROC) ((void *) eglGetProcAddress(
                "glDebugMessageInsertKHR"));
        glDebugMessageCallbackKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC) ((void *) eglGetProcAddress(
                "glDebugMessageCallbackKHR"));
        glDebugMessageLogKHR = (PFNGLGETDEBUGMESSAGELOGKHRPROC) ((void *) eglGetProcAddress("glDebugMessageLogKHR"));
        glGetPointervKHR = (PFNGLGETPOINTERVKHRPROC) ((void *) eglGetProcAddress("glGetPointervKHR"));
        glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC) ((void *) eglGetProcAddress("glPushDebugGroupKHR"));
        glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC) ((void *) eglGetProcAddress("glPopDebugGroupKHR"));
        glObjectLabelKHR = (PFNGLOBJECTLABELKHRPROC) ((void *) eglGetProcAddress("glObjectLabelKHR"));
        glGetObjectLabelKHR = (PFNGLGETOBJECTLABELKHRPROC) ((void *) eglGetProcAddress("glGetObjectLabelKHR"));
        glObjectPtrLabelKHR = (PFNGLOBJECTPTRLABELKHRPROC) ((void *) eglGetProcAddress("glObjectPtrLabelKHR"));
        glGetObjectPtrLabelKHR = (PFNGLGETOBJECTPTRLABELKHRPROC) ((void *) eglGetProcAddress("glGetObjectPtrLabelKHR"));
    }

    FPlatformOpenGLContext *PlatformGetOpenGLRenderingContext(FPlatformOpenGLDevice *Device) {
        return AndroidEGL::GetInstance()->GetRenderingContext();
    }

    FPlatformOpenGLContext *PlatformCreateOpenGLContext(FPlatformOpenGLDevice *Device, void *InWindowHandle) {
        //Assumes Device is already initialized and context already created.
        return AndroidEGL::GetInstance()->GetRenderingContext();
    }

    void PlatformDestroyOpenGLContext(FPlatformOpenGLDevice *Device, FPlatformOpenGLContext *Context) {
    }
//
//   Texture *PlatformCreateBuiltinBackBuffer(FOpenGLDynamicRHI *OpenGLRHI, uint32_t SizeX, uint32_t SizeY) {
//        GE_ASSERT(IsInRenderingThread());
//        // Create the built-in back buffer if we disable backbuffer sampling.
//        // Otherwise return null and we will create an off-screen surface afterward.
//        if (!AndroidEGL::GetInstance()->IsOfflineSurfaceRequired()) {
//            const FOpenGLTextureCreateDesc CreateDesc =
//                    FRHITextureCreateDesc::Create2D(("PlatformCreateBuiltinBackBuffer"), SizeX, SizeY, PF_B8G8R8A8)
//                            .SetFlags(ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::Presentable |
//                                      ETextureCreateFlags::ResolveTargetable)
//                            .DetermineInititialState();
//
//            return new FOpenGLTexture(CreateDesc);
//        } else {
//            return nullptr;
//        }
//    }

    void PlatformResizeGLContext(FPlatformOpenGLDevice *Device, FPlatformOpenGLContext *Context, uint32_t SizeX,
                                 uint32_t SizeY, bool bFullscreen, bool bWasFullscreen, GLenum BackBufferTarget,
                                 GLuint BackBufferResource) {
        GE_ASSERT(Context);
        //VERIFY_GL_SCOPE();

        Context->BackBufferResource = BackBufferResource;
        Context->BackBufferTarget = BackBufferTarget;

        if (AndroidEGL::GetInstance()->IsOfflineSurfaceRequired()) {
            Device->TargetDirty = true;
            glBindFramebuffer(GL_FRAMEBUFFER, Context->ViewportFramebuffer);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, BackBufferTarget, BackBufferResource, 0);
        }

        glViewport(0, 0, SizeX, SizeY);
        GE_ASSERT(glViewport);
    }

    void PlatformGetSupportedResolution(uint32_t &Width, uint32_t &Height) {
    }

    bool PlatformGetAvailableResolutions(std::vector<RHIScreenResolution> &Resolutions, bool bIgnoreRefreshRate) {
        return true;
    }

    int32_t PlatformGlGetError() {
        return glGetError();
    }

// =============================================================

    void PlatformReleaseOcclusionQuery(GLuint Query, uint64_t QueryContext) {
    }

    void FPlatformOpenGLDevice::SetCurrentSharedContext() {
        AndroidEGL::GetInstance()->SetCurrentSharedContext();
    }

    void PlatformDestroyOpenGLDevice(FPlatformOpenGLDevice *Device) {
        delete Device;
    }

    void FPlatformOpenGLDevice::SetCurrentRenderingContext() {
        AndroidEGL::GetInstance()->AcquireCurrentRenderingContext();
    }

    void FPlatformOpenGLDevice::SetupCurrentContext() {
        GLuint *DefaultVao = nullptr;
        EOpenGLCurrentContext ContextType = (EOpenGLCurrentContext) AndroidEGL::GetInstance()->GetCurrentContextType();

        if (ContextType == CONTEXT_Rendering) {
            DefaultVao = &AndroidEGL::GetInstance()->GetRenderingContext()->DefaultVertexArrayObject;
        } else if (ContextType == CONTEXT_Shared) {
            DefaultVao = &AndroidEGL::GetInstance()->GetSharedContext()->DefaultVertexArrayObject;
        } else {
            //Invalid or Other return
            return;
        }

        if (*DefaultVao == 0) {
            glGenVertexArrays(1, DefaultVao);
            glBindVertexArray(*DefaultVao);
        }
    }

    void PlatformLabelObjects() {
        GLuint FrameBuffer = AndroidEGL::GetInstance()->GetResolveFrameBuffer();
        if (FrameBuffer != 0) {
            FOpenGL::LabelObject(GL_FRAMEBUFFER, FrameBuffer, "ResolveFB");
        }
    }

//--------------------------------
#define VIRTUALIZE_QUERIES (1)

    static int32_t GMaxmimumOcclusionQueries = 4000;

#if VIRTUALIZE_QUERIES
// These data structures could be better, but it would be tricky. InFlightVirtualQueries.Remove(QueryId) is a drag
    std::vector<GLuint> UsableRealQueries;
    std::vector<int32_t> InFlightVirtualQueries;
    std::vector<GLuint> VirtualToRealMap;
    std::vector<GLuint64> VirtualResults;
    std::vector<int32_t> FreeVirtuals;
    std::vector<GLuint> QueriesBeganButNotEnded;
#endif

#define QUERY_CHECK(x) check(x)
//#define QUERY_CHECK(x) if (!(x)) { FPlatformMisc::LocalPrint(("Failed a check on line:\n")); FPlatformMisc::LocalPrint(( PREPROCESSOR_TO_STRING(__LINE__))); FPlatformMisc::LocalPrint(("\n")); *((int*)3) = 13; }

#define CHECK_QUERY_ERRORS (DO_CHECK)

    void PlatformGetNewRenderQuery(GLuint *OutQuery, uint64_t *OutQueryContext) {
#if CHECK_QUERY_ERRORS
        GLenum Err = glGetError();
        while (Err != GL_NO_ERROR)
        {
            Err = glGetError();
        }
#endif

        *OutQueryContext = 0;
        //VERIFY_GL_SCOPE();

#if !VIRTUALIZE_QUERIES
        FOpenGLES::GenQueries(1, OutQuery);
#if CHECK_QUERY_ERRORS
        Err = glGetError();
        if (Err != GL_NO_ERROR)
        {
            FPlatformMisc::LowLevelOutputDebugStringf(("GenQueries Failed, glError %d (0x%x)"), Err, Err);
            *(char*)3 = 0;
        }
#endif
#else


        if (!UsableRealQueries.size() && !InFlightVirtualQueries.size()) {
            GRHIMaximumReccommendedOustandingOcclusionQueries = GMaxmimumOcclusionQueries;
//            UE_LOG(LogRHI, Log, ("AndroidOpenGL: Using a maximum of %d occlusion queries."),
//                   GMaxmimumOcclusionQueries);

            UsableRealQueries.push_back(GMaxmimumOcclusionQueries);
            glGenQueries(GMaxmimumOcclusionQueries, &UsableRealQueries[0]);
#if CHECK_QUERY_ERRORS
            Err = glGetError();
            if (Err != GL_NO_ERROR)
            {
                FPlatformMisc::LowLevelOutputDebugStringf(("GenQueries Failed, glError %d (0x%x)"), Err, Err);
                *(char*)3 = 0;
            }
#endif
            VirtualToRealMap.push_back(0); // this is null, it is not a real query and never will be
            VirtualResults.push_back(0); // this is null, it is not a real query and never will be
        }

        if (FreeVirtuals.size()) {
            *OutQuery = FreeVirtuals.back();
            FreeVirtuals.pop_back();
            return;
        }
        *OutQuery = VirtualToRealMap.size();
        VirtualToRealMap.push_back(0);
        VirtualResults.push_back(0);
#endif
    }

    void PlatformReleaseRenderQuery(GLuint Query, uint64_t QueryContext) {
#if !VIRTUALIZE_QUERIES
        glDeleteQueries(1, &Query);
#else
        GLuint RealIndex = VirtualToRealMap[Query];
        if (RealIndex) {
            GLuint OutResult;
            // still in use, wait for it now.
            FAndroidOpenGL::GetQueryObject(Query, FAndroidOpenGL::QM_Result, &OutResult);
            GE_ASSERT(!VirtualToRealMap[Query]);
        }
        FreeVirtuals.push_back(Query);
#endif
    }

    void FAndroidOpenGL::GetQueryObject(GLuint QueryId, EQueryMode QueryMode, GLuint64 *OutResult) {
        GLuint Result;
        GetQueryObject(QueryId, QueryMode, &Result);
        *OutResult = Result;
    }

    void FAndroidOpenGL::GetQueryObject(GLuint QueryId, EQueryMode QueryMode, GLuint *OutResult) {
        GLenum QueryName = (QueryMode == QM_Result) ? GL_QUERY_RESULT : GL_QUERY_RESULT_AVAILABLE;
        //VERIFY_GL_SCOPE();

       // FRenderThreadIdleScope IdleScope(ERenderThreadIdleTypes::WaitingForGPUQuery, QueryName == GL_QUERY_RESULT);

#if !VIRTUALIZE_QUERIES
#if CHECK_QUERY_ERRORS
        GLenum Err = glGetError();
        while (Err != GL_NO_ERROR)
        {
            Err = glGetError();
        }
#endif

        glGetQueryObjectuiv(QueryId, QueryName, OutResult);
#else
        GLuint RealIndex = VirtualToRealMap[QueryId];
        if (!RealIndex) {
            if (QueryName == GL_QUERY_RESULT_AVAILABLE) {
                *OutResult = GL_TRUE;
            } else {
                *OutResult = VirtualResults[QueryId];
            }
            return;
        }

        if (QueryName == GL_QUERY_RESULT) {
            {
                //QUICK_SCOPE_CYCLE_COUNTER(STAT_FAndroidOpenGL_GetQueryObject_Remove);
                auto it = InFlightVirtualQueries.begin()+QueryId;
                InFlightVirtualQueries.erase(it);
                GE_ASSERT(InFlightVirtualQueries.size() == 1);
            }
            UsableRealQueries.push_back(RealIndex);
            VirtualToRealMap[QueryId] = 0;
        }

#if CHECK_QUERY_ERRORS
        GLenum Err = glGetError();
        while (Err != GL_NO_ERROR)
        {
            Err = glGetError();
        }
#endif

        glGetQueryObjectuiv(RealIndex, QueryName, OutResult);

        if (QueryName == GL_QUERY_RESULT) {
            VirtualResults[QueryId] = *OutResult;
        }
#endif

#if CHECK_QUERY_ERRORS
        Err = glGetError();
        QUERY_CHECK(Err == GL_NO_ERROR);
#endif
    }

    GLuint FAndroidOpenGL::MakeVirtualQueryReal(GLuint Query) {
#if !VIRTUALIZE_QUERIES
        return Query;
#else
        GLuint RealIndex = VirtualToRealMap[Query];
        if (RealIndex) {
            GLuint OutResult;
            //QUICK_SCOPE_CYCLE_COUNTER(STAT_FAndroidOpenGL_BeginQuery_RecycleWait);
            // still in use, wait for it now.
            FAndroidOpenGL::GetQueryObject(Query, QM_Result, &OutResult);
            GE_ASSERT(!VirtualToRealMap[Query]);
        }
        if (!UsableRealQueries.size()) {
            GE_ASSERT(InFlightVirtualQueries.size() + QueriesBeganButNotEnded.size() == GMaxmimumOcclusionQueries);
            GE_ASSERT(
                    InFlightVirtualQueries.size()); // if this fires, then it means the nesting of begins is too deep.
            GLuint OutResult;
           // QUICK_SCOPE_CYCLE_COUNTER(STAT_FAndroidOpenGL_BeginQuery_FreeWait);
            FAndroidOpenGL::GetQueryObject(InFlightVirtualQueries[0], QM_Result, &OutResult);
            GE_ASSERT(UsableRealQueries.size());
        }
        RealIndex = UsableRealQueries.back();
        UsableRealQueries.pop_back();
        VirtualToRealMap[Query] = RealIndex;
        VirtualResults[Query] = 0;

        return RealIndex;
#endif
    }

    bool FAndroidOpenGL::SupportsFramebufferSRGBEnable() {
        //bool MobileUseHWsRGBEncodingCVAR = false;// IConsoleManager::Get().FindTConsoleVariableDataInt(
              //  ("r.Mobile.UseHWsRGBEncoding"));
        const bool bMobileUseHWsRGBEncoding = (MobileUseHWsRGBEncodingCVAR &&
                                               MobileUseHWsRGBEncodingCVAR == 1);
        return bMobileUseHWsRGBEncoding;
    }

    void FAndroidOpenGL::BeginQuery(GLenum QueryType, GLuint Query) {
        GE_ASSERT(QueryType == UGL_ANY_SAMPLES_PASSED || SupportsDisjointTimeQueries());
#if CHECK_QUERY_ERRORS
        GLenum Err = glGetError();
        while (Err != GL_NO_ERROR)
        {
            Err = glGetError();
        }
#endif

        //VERIFY_GL_SCOPE();

#if !VIRTUALIZE_QUERIES
        glBeginQuery(QueryType, Query);
#else
        GLuint RealIndex = MakeVirtualQueryReal(Query);
        QueriesBeganButNotEnded.push_back(Query);
        glBeginQuery(QueryType, RealIndex);
#endif
#if CHECK_QUERY_ERRORS
        Err = glGetError();

        QUERY_CHECK(Err == GL_NO_ERROR);
#endif
    }

    void FAndroidOpenGL::EndQuery(GLenum QueryType) {
        GE_ASSERT(QueryType == UGL_ANY_SAMPLES_PASSED || SupportsDisjointTimeQueries());

#if CHECK_QUERY_ERRORS
        GLenum Err = glGetError();
        while (Err != GL_NO_ERROR)
        {
            Err = glGetError();
        }
#endif

        //VERIFY_GL_SCOPE();

        if (QueryType == UGL_ANY_SAMPLES_PASSED) {
            //return;
        }

#if VIRTUALIZE_QUERIES
        InFlightVirtualQueries.push_back(QueriesBeganButNotEnded.back());
        QueriesBeganButNotEnded.pop_back();
#endif
        glEndQuery(QueryType);

#if CHECK_QUERY_ERRORS
        Err = glGetError();

        QUERY_CHECK(Err == GL_NO_ERROR);
#endif
    }

    FAndroidOpenGL::EImageExternalType FAndroidOpenGL::ImageExternalType = FAndroidOpenGL::EImageExternalType::None;

    extern bool AndroidThunkCpp_GetMetaDataBoolean(const std::string &Key);

    extern std::string AndroidThunkCpp_GetMetaDataString(const std::string &Key);

    void FAndroidOpenGL::SetupDefaultGLContextState(const std::string &ExtensionsString) {
        // Enable QCOM non-coherent framebuffer fetch if supported
        if (CVarDisableFBFNonCoherent == 0 &&
            ExtensionsString.find(("GL_QCOM_shader_framebuffer_fetch_noncoherent")) !=std::string::npos &&
            ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch")) != std::string::npos) {
            glEnable(GL_FRAMEBUFFER_FETCH_NONCOHERENT_QCOM);
        }
    }

    bool FAndroidOpenGL::RequiresAdrenoTilingModeHint() {
        return bRequiresAdrenoTilingHint;
    }

    void FAndroidOpenGL::EnableAdrenoTilingModeHint(bool bEnable) {
        if (bEnable && CVarEnableAdrenoTilingHint!= 0) {
            glEnable(GL_BINNING_CONTROL_HINT_QCOM);
            glHint(GL_BINNING_CONTROL_HINT_QCOM, GL_GPU_OPTIMIZED_QCOM);
        } else {
            glDisable(GL_BINNING_CONTROL_HINT_QCOM);
        }
    }

    void FAndroidOpenGL::ProcessExtensions(const std::string &ExtensionsString) {
        std::string VersionString = std::string(((const char *) glGetString(GL_VERSION)));
        std::string SubVersionString;

        FOpenGLES::ProcessExtensions(ExtensionsString);

        std::string RendererString = std::string(((const char *) glGetString(GL_RENDERER)));

        // Common GPU types
        const bool bIsNvidiaBased = RendererString.find(("NVIDIA")) != std::string::npos;
        const bool bIsPoverVRBased = RendererString.find(("PowerVR"))!= std::string::npos;
        const bool bIsAdrenoBased = RendererString.find(("Adreno"))!= std::string::npos;
        const bool bIsMaliBased = RendererString.find(("Mali"))!= std::string::npos;

        if (bIsPoverVRBased) {
            bHasHardwareHiddenSurfaceRemoval = true;
            GE_CORE_INFO( ("Enabling support for Hidden Surface Removal on PowerVR"));
        }

        if (bIsAdrenoBased) {
            GMaxmimumOcclusionQueries = 510;
            // This is to avoid a bug in Adreno drivers that define GL_ARM_shader_framebuffer_fetch_depth_stencil even when device does not support this extension
            // OpenGL ES 3.1 V@127.0 (GIT@I1af360237c)
            bRequiresARMShaderFramebufferFetchDepthStencilUndef = !bSupportsShaderDepthStencilFetch;

            // FORT-221329's broken adreno driver not common on Android 9 and above. TODO: check adreno driver version instead.
            bRequiresAdrenoTilingHint = FAndroidMisc::GetAndroidBuildVersion() < 28 ||
                                        CVarEnableAdrenoTilingHint == 2;
            if(bRequiresAdrenoTilingHint)
            GE_CORE_INFO("Enabling Adreno tiling hint.");
          //  LOGI("Enabling Adreno tiling hint.")
        }

        if (bIsMaliBased) {
            //TODO restrict this to problematic drivers only
            bRequiresReadOnlyBuffersWorkaround = true;
        }

        // Disable ASTC if requested by device profile
      //  static const auto CVarDisableASTC = IConsoleManager::Get().FindTConsoleVariableDataInt(
       //         ("r.Android.DisableASTCSupport"));
        if (bSupportsASTC && CVarDisableASTC) {
            bSupportsASTC = false;
            bSupportsASTCHDR = false;
            FAndroidGPUInfo::Get().RemoveTargetPlatform(("Android_ASTC"));
            //UE_LOG(LogRHI, Log, ("ASTC was disabled via r.OpenGL.DisableASTCSupport"));
        }

        // Check for external image support for different ES versions
        ImageExternalType = EImageExternalType::None;

//        static const auto CVarOverrideExternalTextureSupport = IConsoleManager::Get().FindTConsoleVariableDataInt(
//                ("r.Android.OverrideExternalTextureSupport"));
        const int32_t OverrideExternalTextureSupport = CVarOverrideExternalTextureSupport;
        switch (OverrideExternalTextureSupport) {
            case 1:
                ImageExternalType = EImageExternalType::None;
                break;

            case 2:
                ImageExternalType = EImageExternalType::ImageExternal100;
                break;

            case 3:
                ImageExternalType = EImageExternalType::ImageExternal300;
                break;

            case 4:
                ImageExternalType = EImageExternalType::ImageExternalESSL300;
                break;

            case 0:
            default:
                // auto-detect by extensions (default)
                bool bHasImageExternal = ExtensionsString.find(("GL_OES_EGL_image_external "))!=std::string::npos ;//||
                                       //  ExtensionsString.EndsWith(("GL_OES_EGL_image_external"));
                bool bHasImageExternalESSL3 = ExtensionsString.find(("OES_EGL_image_external_essl3"))!=std::string::npos ;
                if (bHasImageExternal || bHasImageExternalESSL3) {
                    ImageExternalType = EImageExternalType::ImageExternal100;
                    if (bHasImageExternalESSL3) {
                        ImageExternalType = EImageExternalType::ImageExternalESSL300;
                    } else {
                        // Adreno 5xx can do essl3 even without extension in list
                        if (bIsAdrenoBased && RendererString.find(("(TM) 5"))!=std::string::npos) {
                            ImageExternalType = EImageExternalType::ImageExternalESSL300;
                        }
                    }

                    if (bIsNvidiaBased) {
                        // Nvidia needs version 100 even though it supports ES3
                        ImageExternalType = EImageExternalType::ImageExternal100;
                    }
                }
                break;
        }
        switch (ImageExternalType) {
            case EImageExternalType::None:
                GE_CORE_INFO("Image external disabled");
                break;

            case EImageExternalType::ImageExternal100:
            GE_CORE_INFO ("Image external enabled: ImageExternal100");
                break;

            case EImageExternalType::ImageExternal300:
            GE_CORE_INFO ("Image external enabled: ImageExternal300");
                break;

            case EImageExternalType::ImageExternalESSL300:
            GE_CORE_INFO("Image external enabled: ImageExternalESSL300");
                break;

            default:
                ImageExternalType = EImageExternalType::None;
                GE_CORE_INFO("Image external disabled; unknown type");
        }
        bSupportsImageExternal = ImageExternalType != EImageExternalType::None;

        // check for supported texture formats if enabled
        bool bCookOnTheFly = false;
#if !UE_BUILD_SHIPPING
        std::string FileHostIP;
        bCookOnTheFly = false;//FParse::Value(FCommandLine::Get(), ("filehostip"), FileHostIP);
#endif
        if (!bCookOnTheFly &&
            AndroidThunkCpp_GetMetaDataBoolean(("com.epicgames.unreal.GameActivity.bValidateTextureFormats"))) {
            std::string CookedFlavorsString = AndroidThunkCpp_GetMetaDataString(
                    ("com.epicgames.unreal.GameActivity.CookedFlavors"));
            if (!CookedFlavorsString.empty()) {
//                std::vector<std::string> CookedFlavors;
//                CookedFlavorsString.ParseIntoArray(CookedFlavors, (","), true);
//
//                // check each cooked flavor for support (only need one to be supported)
//                bool bFoundSupported = false;
//                for (std::string Flavor: CookedFlavors) {
//                    if (Flavor.Equals(("ETC2"))) {
//                        if (FOpenGL::SupportsETC2()) {
//                            bFoundSupported = true;
//                            break;
//                        }
//                    }
//                    if (Flavor.Equals(("DXT"))) {
//                        if (FOpenGL::SupportsDXT()) {
//                            bFoundSupported = true;
//                            break;
//                        }
//                    }
//                    if (Flavor.Equals(("ASTC"))) {
//                        if (FOpenGL::SupportsASTC()) {
//                            bFoundSupported = true;
//                            break;
//                        }
//                    }
//                }
//
//                if (!bFoundSupported) {
//                    std::string Message = ("Cooked Flavors: ") + CookedFlavorsString + ("\n\nSupported: ETC2") +
//                                          (FOpenGL::SupportsDXT() ? (",DXT") : ("")) +
//                                          (FOpenGL::SupportsASTC() ? (",ASTC") : (""));
//
//                    FPlatformMisc::LowLevelOutputDebugStringf(("Error: Unsupported Texture Format\n%s"), *Message);
//                    FAndroidMisc::MessageBoxExt(EAppMsgType::Ok, *Message, ("Unsupported Texture Format"));
//                }
            }
        }

        // Qualcomm non-coherent framebuffer_fetch
        if (CVarDisableFBFNonCoherent == 0 &&
            ExtensionsString.find(("GL_QCOM_shader_framebuffer_fetch_noncoherent"))!= std::string::npos &&
            ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch"))!= std::string::npos ) {
            glFramebufferFetchBarrierQCOM = (PFNGLFRAMEBUFFERFETCHBARRIERQCOMPROC) ((void *) eglGetProcAddress(
                    "glFramebufferFetchBarrierQCOM"));
            if (glFramebufferFetchBarrierQCOM != nullptr) {
              GE_CORE_INFO("Using QCOM_shader_framebuffer_fetch_noncoherent");
            }
        }

        if (CVarDisableEarlyFragmentTests != 0) {
            bRequiresDisabledEarlyFragmentTests = true;
            GE_CORE_INFO("Disabling early_fragment_tests");
        }
    }

    std::string FAndroidMisc::GetGPUFamily() {
        return FAndroidGPUInfo::Get().GetGPUFamily();
    }

    std::string FAndroidMisc::GetGLVersion() {
        return FAndroidGPUInfo::Get().GLVersion;
    }

    bool FAndroidMisc::SupportsFloatingPointRenderTargets() {
        return FAndroidGPUInfo::Get().bSupportsFloatingPointRenderTargets;
    }

    bool FAndroidMisc::SupportsShaderFramebufferFetch() {
        return FAndroidGPUInfo::Get().bSupportsFrameBufferFetch;
    }

    bool FAndroidMisc::SupportsES30() {
        return true;
    }

//    void FAndroidMisc::GetValidTargetPlatforms(std::vector<std::string> &TargetPlatformNames) {
//        TargetPlatformNames = FAndroidGPUInfo::Get().TargetPlatformNames;
//    }

    void FAndroidAppEntry::PlatformInit() {
        // Try to create an ES3.2 EGL here for gpu queries and don't have to recreate the GL context.
        AndroidEGL::GetInstance()->Init(AndroidEGL::AV_OpenGLES, 3, 2, false);
    }

    void FAndroidAppEntry::ReleaseEGL() {
        AndroidEGL *EGL = AndroidEGL::GetInstance();
        if (EGL->IsInitialized()) {
            EGL->DestroyBackBuffer();
            EGL->Terminate();
        }
    }

    static bool GRemoteCompileServicesActive = false;

    bool AreAndroidOpenGLRemoteCompileServicesActive() {
        return GRemoteCompileServicesActive && AreAndroidOpenGLRemoteCompileServicesAvailable();
    }

    bool FAndroidOpenGL::AreRemoteCompileServicesActive() {
        return AreAndroidOpenGLRemoteCompileServicesActive();
    }

    bool FAndroidOpenGL::StartAndWaitForRemoteCompileServices(int NumServices) {
        bool bResult = false;
        JNIEnv *Env = AndroidApplication::GetJavaEnv();

        if (Env && AreAndroidOpenGLRemoteCompileServicesAvailable()) {
            bResult = (bool) Env->CallStaticBooleanMethod(OpenGLRemoteGLProgramCompileJNI.OGLServiceAccessor,
                                                          OpenGLRemoteGLProgramCompileJNI.StartRemoteProgramLink,
                                                          (jint) NumServices, (jboolean) false);
            GRemoteCompileServicesActive = bResult;
        }

        return bResult;
    }

    void FAndroidOpenGL::StopRemoteCompileServices()
    {
        GRemoteCompileServicesActive = false;
        JNIEnv *Env = AndroidApplication::GetJavaEnv();

        if (Env && AreAndroidOpenGLRemoteCompileServicesAvailable())
        {
            Env->CallStaticVoidMethod(OpenGLRemoteGLProgramCompileJNI.OGLServiceAccessor,
                                      OpenGLRemoteGLProgramCompileJNI.StopRemoteProgramLink);
        }
    }

    namespace AndroidOGLService {
        std::atomic<bool> bOneTimeErrorEncountered = false;
    }

    std::vector<uint8_t>
    FAndroidOpenGL::DispatchAndWaitForRemoteGLProgramCompile(const std::vector<uint8_t> ContextData,
                                                             const std::vector<char> &VertexGlslCode,
                                                             const std::vector<char> &PixelGlslCode,
                                                             const std::vector<char> &ComputeGlslCode,
                                                             std::string &FailureMessageOUT) {
        bool bResult = false;
        JNIEnv *Env = AndroidApplication::GetJavaEnv();
        std::vector<uint8_t> CompiledProgramBinary;
        std::string ErrorMessage;

        if (Env && (GRemoteCompileServicesActive) && (AreAndroidOpenGLRemoteCompileServicesAvailable())) {
//            // todo: double conversion :(
//            auto jVS = NewScopedJavaObject(Env, Env->NewStringUTF(
//                    TCHAR_TO_UTF8(ANSI_TO_TCHAR(VertexGlslCode.empty() ? "" : VertexGlslCode.GetData()))));
//            auto jPS = NewScopedJavaObject(Env, Env->NewStringUTF(
//                    TCHAR_TO_UTF8(ANSI_TO_TCHAR(PixelGlslCode.empty() ? "" : PixelGlslCode.GetData()))));
//            auto jCS = NewScopedJavaObject(Env, Env->NewStringUTF(
//                    TCHAR_TO_UTF8(ANSI_TO_TCHAR(ComputeGlslCode.empty() ? "" : ComputeGlslCode.GetData()))));
//            auto ProgramKeyBuffer = NewScopedJavaObject(Env, Env->NewByteArray(ContextData.size()));
//            Env->SetByteArrayRegion(*ProgramKeyBuffer, 0, ContextData.size(),
//                                    reinterpret_cast<const jbyte *>(ContextData.GetData()));
//            auto ProgramResponseObj = NewScopedJavaObject(Env, Env->CallStaticObjectMethod(
//                    OpenGLRemoteGLProgramCompileJNI.OGLServiceAccessor,
//                    OpenGLRemoteGLProgramCompileJNI.DispatchProgramLink, *ProgramKeyBuffer, *jVS, *jPS, *jCS));
//            CHECK_JNI_EXCEPTIONS(Env);
//
//            if (*ProgramResponseObj) {
//                bool bSucceeded = (bool) Env->GetBooleanField(*ProgramResponseObj,
//                                                              OpenGLRemoteGLProgramCompileJNI.ProgramResponse_SuccessField);
//                if (bSucceeded) {
//                    auto ProgramResult = NewScopedJavaObject(Env, (jbyteArray) Env->GetObjectField(*ProgramResponseObj,
//                                                                                                   OpenGLRemoteGLProgramCompileJNI.ProgramResponse_CompiledBinaryField));
//                    int len = Env->GetArrayLength(*ProgramResult);
//                    CompiledProgramBinary.SetNumUninitialized(len);
//                    Env->GetByteArrayRegion(*ProgramResult, 0, len,
//                                            reinterpret_cast<jbyte *>(CompiledProgramBinary.GetData()));
//                } else {
//                    if (AndroidOGLService::bOneTimeErrorEncountered.exchange(true) == false) {
//                        FGenericCrashContext::SetEngineData(("Android.PSOService"), ("ec"));
//                    }
//
//                    FailureMessageOUT = FJavaHelper::FStringFromLocalRef(Env, (jstring) Env->GetObjectField(
//                            *ProgramResponseObj, OpenGLRemoteGLProgramCompileJNI.ProgramResponse_ErrorField));
//                    check(!FailureMessageOUT.empty());
//                }
//            } else {
//                if (AndroidOGLService::bOneTimeErrorEncountered.exchange(true) == false) {
//                    FGenericCrashContext::SetEngineData(("Android.PSOService"), ("es"));
//                }
//                FailureMessageOUT = ("Remote compiler failed.");
//            }
        }
        return CompiledProgramBinary;
    }

}
#endif
