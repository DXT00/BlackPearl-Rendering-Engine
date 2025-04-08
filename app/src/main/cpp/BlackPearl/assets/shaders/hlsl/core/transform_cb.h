#ifndef TRANSFORM_CB_H
#define TRANSFORM_CB_H

#include "align.h"
struct TransformConstants
{
    ALIGN(16) float4x4 matModel; 
};
#endif // TRANSFORM_CB_H