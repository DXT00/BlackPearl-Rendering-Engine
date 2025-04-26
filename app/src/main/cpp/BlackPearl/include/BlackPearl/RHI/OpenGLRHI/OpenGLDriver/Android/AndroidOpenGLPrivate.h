//
// Created by DXT00 on 2025/4/11.
//
#pragma once
#include <string>
#include <vector>
#include "BlackPearl/Core.h"
#include "AndroidEGL.h"
//#include "Android/AndroidApplication.h"
//#include "libgpuinfo.hpp"
//#include "Internationalization/Regex.h"
//#include "Misc/CString.h"
namespace BlackPearl{


bool GAndroidGPUInfoReady = false;

// call out to JNI to see if the application was packaged for Oculus Mobile
extern bool AndroidThunkCpp_IsOculusMobileApplication();
extern bool ShouldUseGPUFencesToLimitLatency();


class FAndroidGPUInfo
{
public:
    static FAndroidGPUInfo& Get()
    {
        static FAndroidGPUInfo This;
        return This;
    }

    std::string GLVersion;
    std::string VendorName;
    bool bSupportsFloatingPointRenderTargets;
    bool bSupportsFrameBufferFetch;
    std::vector<std::string> TargetPlatformNames;

    void RemoveTargetPlatform(std::string PlatformName)
    {
        auto it = std::find(TargetPlatformNames.begin(), TargetPlatformNames.end(),PlatformName);
        if(it != TargetPlatformNames.end())
            TargetPlatformNames.erase(it);
    }

    // computing GPU family needs regex access, which might not be available early in init
    std::string& GetGPUFamily()
    {
        if (GPUFamily.empty())
            ReadGPUFamily();
        return GPUFamily;
    }

private:
    std::string GPUFamily;

    FAndroidGPUInfo()
    {
        // this is only valid in the game thread, make sure we are initialized there before being called on other threads!
       // GE_ASSERT(IsInGameThread())

        // make sure GL is started so we can get the supported formats
        AndroidEGL* EGL = AndroidEGL::GetInstance();

        if (!EGL->IsInitialized())
        {
            FAndroidAppEntry::PlatformInit();
        }

        // Do not create a window surface if the app is for Oculus Mobile (use small buffer)
        bool bCreateSurface = !AndroidThunkCpp_IsOculusMobileApplication();
        //FPlatformMisc::LowLevelOutputDebugString(TEXT("FAndroidGPUInfo"));
        EGL->InitSurface(false, bCreateSurface);
        EGL->SetCurrentSharedContext();

        // get extensions
        // Process the extension caps directly here, as FOpenGL might not yet be setup
        // Do not process extensions here, because extension pointers may not be setup
        const char* GlGetStringOutput = (const char*) glGetString(GL_EXTENSIONS);
        std::string ExtensionsString = GlGetStringOutput;

        GLVersion = (const char*)glGetString(GL_VERSION);

        // highest priority is the per-texture version
        if (ExtensionsString.find(("GL_KHR_texture_compression_astc_ldr")) != std::string::npos)
        {
            TargetPlatformNames.push_back(("Android_ASTC"));
        }
        if (ExtensionsString.find(("GL_NV_texture_compression_s3tc"))  != std::string::npos|| ExtensionsString.find(("GL_EXT_texture_compression_s3tc")) != std::string::npos)
        {
            TargetPlatformNames.push_back(("Android_DXT"));
        }

        TargetPlatformNames.push_back(("Android_ETC2"));

        // finally, generic Android
        TargetPlatformNames.push_back(("Android"));

        bSupportsFloatingPointRenderTargets =
                ExtensionsString.find(("GL_EXT_color_buffer_half_float")) != std::string::npos
                // According to https://www.khronos.org/registry/gles/extensions/EXT/EXT_color_buffer_float.txt
                || (ExtensionsString.find(("GL_EXT_color_buffer_float"))) != std::string::npos;

        bSupportsFrameBufferFetch = ExtensionsString.find(("GL_EXT_shader_framebuffer_fetch"))  != std::string::npos|| ExtensionsString.find(("GL_NV_shader_framebuffer_fetch")) != std::string::npos
                                    || ExtensionsString.find(("GL_ARM_shader_framebuffer_fetch ")) != std::string::npos; // has space at the end to exclude GL_ARM_shader_framebuffer_fetch_depth_stencil match

        GAndroidGPUInfoReady = true;

        VendorName = std::string(((const char*)glGetString(GL_VENDOR)));
    }

    void ReadGPUFamily()
    {
        GPUFamily = (const char*)glGetString(GL_RENDERER);
        GE_ASSERT(!GPUFamily.empty());

//        // thirdparty api requires std::unique_ptr
//        std::unique_ptr<libgpuinfo::instance> ArmGPUInfoInstance = libgpuinfo::instance::create();
//
//        if (ArmGPUInfoInstance)
//        {
//            const libgpuinfo::gpuinfo& ArmGPUInfo = ArmGPUInfoInstance->get_info();
//            // Note:
//            // if libgpuinfo is not upto date then the gpu may not appear in gpuinfo's internal list,
//            // To avoid this we ignore the name and use the lib to extract only the core count. (which does not use the list)
//            const FRegexPattern RegexPattern(("^Mali(?:.+[MC|MP]([0-9]+))?")); // find anything that starts with Mali and capture the number after the last M[CP]
//            FRegexMatcher RegexMatcher(RegexPattern, *GPUFamily);
//            if (RegexMatcher.FindNext() && ArmGPUInfo.num_shader_cores > 0)
//            {
//                std::string Capture = RegexMatcher.GetCaptureGroup(1);
//                if (Capture.empty())
//                {
//                    std::string ARMLibName = FString::Format(("{0} MP{1}"), { *GPUFamily, ArmGPUInfo.num_shader_cores });
//                    UE_LOG(LogAndroid, Log, ("FAndroidGPUInfo renaming GPUFamily: %s -> %s"), *GPUFamily, *ARMLibName);
//                    GPUFamily = ARMLibName;
//                }
//                else if((uint32)FCString::Atoi64(*Capture) != ArmGPUInfo.num_shader_cores)
//                {
//                    UE_LOG(LogAndroid, Warning, ("FAndroidGPUInfo GPUFamily core count mismatch: %s, expected MP%d"), *GPUFamily, ArmGPUInfo.num_shader_cores);
//                }
//            }
//        }
    }
};
}