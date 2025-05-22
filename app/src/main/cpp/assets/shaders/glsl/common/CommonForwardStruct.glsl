#include <forward_cb.h>

/* deferred_shading_bsdf_xx pass */

layout(std140, binding = 8) uniform ForwardShadingUBO {
    ForwardShadingLightConstants g_ForwardLight;
} ;


