// 2. 替换 HLSL 的 Texture2D 和 SamplerState 为 GLSL 的组合采样器
layout(binding = 0) uniform sampler2D t_BaseOrDiffuse;    // MATERIAL_DIFFUSE_SLOT -> 0
layout(binding = 1) uniform sampler2D t_MetalRoughOrSpecular;
layout(binding = 2) uniform sampler2D t_Normal;
layout(binding = 3) uniform sampler2D t_Emissive;
layout(binding = 4) uniform sampler2D t_Occlusion;
layout(binding = 5) uniform sampler2D t_Transmission;


// 3. 定义材质采样结构体
struct MaterialTextureSample {
    vec4 baseOrDiffuse;
    vec4 metalRoughOrSpecular;
    vec4 emissive;
    vec4 normal;
    vec4 occlusion;
    vec4 transmission;
};



#ifdef COOK
#include <assets/shaders/glsl/CookBSDF.glsl>

// 1. 替换 HLSL 的 cbuffer 为 GLSL 的 uniform buffer
layout(std140, binding = 7) uniform MaterialUBO  { // MATERIAL_CB_SLOT -> binding=7

   MaterialConstants g_Material

};


MaterialConstant CreateCookBSDFMaterial(){

}


#endif

#ifdef Disney
#include <assets/shaders/glsl/DisneyBSDF.glsl>

// 1. 替换 HLSL 的 cbuffer 为 GLSL 的 uniform buffer
layout(std140, binding = 7) uniform MaterialUBO { // MATERIAL_CB_SLOT -> binding=7

  DisneyMaterialConstant g_Material

} ;


DisneyMaterialConstant CreateDisneyBSDFMaterial(){

}

#endif