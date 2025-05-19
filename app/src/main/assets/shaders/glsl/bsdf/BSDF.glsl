// 2. 替换 HLSL 的 Texture2D 和 SamplerState 为 GLSL 的组合采样器
layout(binding = 0) uniform sampler2D t_BaseOrDiffuse;    // MATERIAL_DIFFUSE_SLOT -> 0
layout(binding = 1) uniform sampler2D t_MetalRoughOrSpecular;
layout(binding = 2) uniform sampler2D t_Normal;
layout(binding = 3) uniform sampler2D t_Emissive;
layout(binding = 4) uniform sampler2D t_Occlusion;
layout(binding = 5) uniform sampler2D t_Transmission;


#include <assets/shaders/glsl/common/TextureSample.glsl>

#ifdef COOK

#include <assets/shaders/glsl/bsdf/CookBSDF.glsl>
layout(std140, binding = 7) uniform MaterialUBO  { // MATERIAL_CB_SLOT -> binding=7
   MaterialConstants g_Mat;
};

#elif defined (Disney)

#include <assets/shaders/glsl/bsdf/DisneyBSDF.glsl>
layout(std140, binding = 7) uniform MaterialUBO { // MATERIAL_CB_SLOT -> binding=7
  DisneyMaterialConstant g_Mat;
} ;


#endif