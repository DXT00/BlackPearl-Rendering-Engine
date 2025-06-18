#ifndef BP_COMMON_DEFERRED_STRUCT_H
#define BP_COMMON_DEFERRED_STRUCT_H

#include <deferred_lighting_cb.h>

/* deferred_shading_bsdf_xx pass */

layout(std140, binding = 8) uniform DeferredShadingUBO {
    DeferredLightingConstants g_DeferredLight;
};


#if USE_GLES_PLS

// inout rather than out


__pixel_localEXT InPLS
{
//    layout(r11f_g11f_b10f) vec3 t_gSceneColor;
//    layout(rgb10_a2) vec4 t_gGbufferA;
//    layout(rgba8) vec4 t_gGbufferB;
//    layout(rgba8) vec4 t_gGbufferC;
// pls 必须是四个通道
    layout(rgb10_a2) vec4 t_gSceneColor;
    layout(rgba8) vec4 t_gGbufferA;
    layout(rgba8) vec4 t_gGbufferB;
    layout(rgba8) vec4 t_gGbufferC;
}pls;

//layout(binding = 0) uniform sampler2D t_gSceneColor;

#else
// SceneColor and Gbuffer Textures
layout(binding = 0) uniform sampler2D t_gSceneColor; //use for emissive color and fog
layout(binding = 1) uniform sampler2D t_gGbufferA;  //encode normal.xy + Encode IndirectIrradiance + reserve
layout(binding = 2) uniform sampler2D t_gGbufferB; // Metallic + Specular + Roughness + ShadingModelID / 255.0
layout(binding = 3) uniform sampler2D t_gGbufferC; // BaseColor.xyz + reserve(precompute shadow)
layout(binding = 4) uniform sampler2D t_gSceneDepth; //default depth texture

#endif


#if USE_POINTLIGHT_SHADOW
layout(binding = 5) uniform samplerCube t_gShadowCubeMap; //default depth texture
#endif

#endif