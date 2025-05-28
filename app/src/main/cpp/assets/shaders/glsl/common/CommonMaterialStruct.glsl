#ifndef BP_COMMON_MATERIAL_STRUCT_H
#define BP_COMMON_MATERIAL_STRUCT_H

#include <material_cb.h>

// Material Textures Sampler
layout(binding = 0) uniform sampler2D t_BaseOrDiffuse;    // MATERIAL_DIFFUSE_SLOT -> 0
layout(binding = 1) uniform sampler2D t_MetalRoughOrSpecular;
layout(binding = 2) uniform sampler2D t_Normal;
layout(binding = 3) uniform sampler2D t_Emissive;
layout(binding = 4) uniform sampler2D t_Occlusion;
layout(binding = 5) uniform sampler2D t_Transmission;


#if COOK
layout(std140, binding = 7) uniform MaterialUBO  { // MATERIAL_CB_SLOT -> binding=7
   MaterialConstants g_Mat;
};
#elif (Disney)
layout(std140, binding = 7) uniform MaterialUBO { // MATERIAL_CB_SLOT -> binding=7
  DisneyMaterialConstant g_Mat;
};
#endif

#endif //BP_COMMON_MATERIAL_STRUCT_H