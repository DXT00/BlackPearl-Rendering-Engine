
/***************************** default pc macro **************************************/
#ifndef COMPILE_SHADER
#define COMPILE_SHADER 1
#endif

//platform
#ifndef SHADING_PATH_MOBILE
#define SHADING_PATH_MOBILE 0
#endif

//lighting
#ifndef ALLOW_STATIC_LIGHTING
#define ALLOW_STATIC_LIGHTING 1
#endif

// normal
#ifndef USE_TBN
#define USE_TBN 0
#endif
//platform
#ifndef VULKAN_PROFILE
#define VULKAN_PROFILE 0
#endif

#ifndef METAL_PROFILE
#define METAL_PROFILE 0
#endif

#ifndef USE_GLES_FBF_DEFERRED
#define USE_GLES_FBF_DEFERRED 0
#endif

#ifndef USE_GLES_PLS
#define USE_GLES_PLS 0
#endif

#ifndef MOBILE_SHADINGMODEL_SUPPORT
#define MOBILE_SHADINGMODEL_SUPPORT 0
#endif

//deferred shading
#ifndef DEFERRED_SHADING_PASS
#define DEFERRED_SHADING_PASS 0
#endif

// shadow
#ifndef USE_POINTLIGHT_SHADOW
#define USE_POINTLIGHT_SHADOW 0
#endif

// shading model
#ifndef COOK
#define COOK 1
#endif

#ifndef PHONG
#define PHONG 0
#endif

#ifndef BLINN
#define BLINN 0
#endif

#ifndef Disney
#define Disney 0
#endif

//gbuffer
#ifndef NONMETAL
#define NONMETAL 0
#endif

// brdf specular part: D
#ifndef COOK_BLINN
#define COOK_BLINN 0
#endif

#ifndef COOK_BECKMANN
#define COOK_BECKMANN 0
#endif

#ifndef COOK_GGX
#define COOK_GGX 1
#endif


//texture
#ifndef USE_ALBEDO_MAP
#define USE_ALBEDO_MAP 0
#endif

#ifndef USE_NORMAL_MAP
#define USE_NORMAL_MAP 0
#endif

#ifndef USE_ROUGHNESS_MAP
#define USE_ROUGHNESS_MAP 0
#endif

#ifndef USE_EMISSIVE_MAP
#define USE_EMISSIVE_MAP 0
#endif

#ifndef USE_TRANSMISSION_MAP
#define USE_TRANSMISSION_MAP 0
#endif

#ifndef USE_AO_MAP
#define USE_AO_MAP 0
#endif
//end if macro.glsl
/***************************** end of default pc macro **************************************/


