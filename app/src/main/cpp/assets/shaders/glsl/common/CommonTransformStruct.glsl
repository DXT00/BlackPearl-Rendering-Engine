
#ifndef  BP_COMMON_TRAANSFORM_STRUCT_H
#define  BP_COMMON_TRAANSFORM_STRUCT_H

#include <transform_cb.h>



layout(std140, binding = 1) uniform TransformConstantsUBO {
    TransformConstants g_Transform;
};

#endif