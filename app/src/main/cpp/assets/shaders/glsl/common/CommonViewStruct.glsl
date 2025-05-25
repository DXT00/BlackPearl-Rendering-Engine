
#ifndef  BP_COMMON_VIEW_STRUCT_H
#define BP_COMMON_VIEW_STRUCT_H

#include <forward_cb.h>

layout(std140, binding = 0) uniform ForwardShadingViewConstantsUBO {
   ForwardShadingViewConstants g_View;
};
#endif//BP_COMMON_VIEW_STRUCT_H