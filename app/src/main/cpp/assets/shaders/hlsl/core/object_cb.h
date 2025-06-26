
#ifndef OBJECT_CB_H
#define OBJECT_CB_H

#include "view_cb.h"
#include "align.h"

struct ObjectInfoConstants
{
   // vec2 mesh; //meshId = mesh.x; meshCnt = mesh.y
    ALIGN(16) vec3 boundingBox; // bbox.xyz = bbox.extend, 
    ALIGN(16) vec3 pos;
};

#endif // OBJECT_CB_H