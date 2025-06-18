//
// Created by DXT00 on 2025/5/16.
//
#include "pch.h"
#include "Renderer/GbufferInfo.h"
#include "RHI/RHIGlobals.h"
#include "RHI/Common/FormatInfo.h"
#include "Renderer/RenderGraph/RenderGraph.h"
namespace BlackPearl
{
    int32_t TargetLighting = 0; //IndirectLight*ao + Emissive + Fog.rgb
    int32_t TargetGBufferA = 1; //encode normal.xy
    int32_t TargetGBufferB = 2; //Metallic	Specular Roughness	ShadingModelID / 255.0
    int32_t TargetGBufferC = 3; //BaseColor	PrecomputedShadow


    GbufferTarget GetTargetByName(const GbufferInfo& info, const std::string& name) {
        if (name == "GBufferA") {
            if (TargetGBufferA < info.numTargets) {
                return info.targets[TargetGBufferA];
            }
        }
        else if (name == "GBufferB") {
            if (TargetGBufferB < info.numTargets) {
                return info.targets[TargetGBufferB];
            }
        }
        else if (name == "GBufferC") {
            if (TargetGBufferC < info.numTargets) {
                return info.targets[TargetGBufferC];
            }
        }
        GE_CORE_ERROR("unknown Gbuffer name %s", name.c_str());
        return GbufferTarget();
    }


    static GbufferInfo GetPCGbuffer(const PlatformGbufferConfig& config)
    {
        GbufferInfo info;
        
        info.targets[TargetLighting].format = Format::RGBA8_UNORM;//Format::R11G11B10_FLOAT;
        if (config.bHighPresision) {
            info.targets[TargetGBufferA].format = Format::RGBA16_FLOAT;
            info.targets[TargetGBufferB].format = Format::RGBA16_FLOAT;
            info.targets[TargetGBufferC].format = Format::RGBA16_FLOAT;



        }
        else {
            info.targets[TargetGBufferA].format = Format::RGBA8_UNORM;//R10G10B10A2_UNORM;
            info.targets[TargetGBufferB].format = Format::RGBA8_UNORM;
            info.targets[TargetGBufferC].format = Format::RGBA8_UNORM;

        }
        info.targets[TargetGBufferA].name = "GBufferA";
        info.targets[TargetGBufferB].name = "GBufferB";
        info.targets[TargetGBufferC].name = "GBufferC";
        info.targets[TargetLighting].name = "SceneColor";
        info.numTargets = 4;
        return info;
    }
    /*
     *
     *  1. Pixel Local Storage 是 OpenGL ES（通常是 ES 3.1 及以上）中一个扩展功能（比如 GL_EXT_shader_pixel_local_storage），允许你在渲染过程中直接在片元（pixel）级别的本地内存中读写数据，而不需要写回到 framebuffer。这在做 deferred shading（延迟着色）时非常有用，因为它能显著提高性能和带宽效率。
        2. 128 bits 限制
        当使用 PLS 时，硬件通常会限制你每个像素最多可以使用的本地存储空间为 128位（即16字节）。这16字节可以被用于存储多个不同的渲染属性,

        如：
        Albedo（RGB）: 3 × float16 → 48 bit
        Normal（XYZ）: 3 × float16 → 48 bit
        Depth or Roughness: 1 × float16 → 16 bit
        Specular or Metalness: 1 × float16 → 16 bit
        总计：48 + 48 + 16 + 16 = 128 bit → 正好在限制内
     *
     * */
    static GbufferInfo GetMobileGbuffer(const PlatformGbufferConfig& config)
    {
        //GbufferInfo info;
        GbufferInfo Info = GetPCGbuffer(config);
        bool bUsingPixelLocalStorage = RenderGraph::SupportPLS();
        if (bUsingPixelLocalStorage)
        {
            Info.numTargets = 1;
        }
        else
        {
            Info.numTargets = 4;
//            if (MobileUsesExtenedGBuffer(Params.ShaderPlatform))
//            {
//                info.numTargets++;
//            }
//
//            if (MobileRequiresSceneDepthAux(Params.ShaderPlatform))
//            {
//                // if used, mobile deferred is always F32.
//                info.numTargets[Info.NumTargets].Init(GBT_Float_32, TEXT("DepthAux"), false, true, true, false);
//                info.numTargets++;
//            }
        }
        return Info;
    }


    GbufferInfo GetGbufferInfo(const PlatformGbufferConfig& config)
    {
#ifdef GE_PLATFORM_ANDROID
        return GetMobileGbuffer(config);
#elif defined(GE_PLATFORM_WINDOWS)
        return GetPCGbuffer(config);
#endif
        GE_CORE_ERROR("Unknown platform");
        return GbufferInfo();

    }



}

