
#ifndef BP_COMMON_FORWARD_STRUCT_H
#define BP_COMMON_FORWARD_STRUCT_H

#include <forward_cb.h>
#include <material_cb.h>

layout(std140, binding = 8) uniform ForwardShadingUBO {
    ForwardShadingLightConstants g_ForwardLight;
};


/* fowardshading_pass */
#include <assets/shaders/glsl/common/CommonMaterialStruct.glsl>




#endif //BP_COMMON_FORWARD_STRUCT_H