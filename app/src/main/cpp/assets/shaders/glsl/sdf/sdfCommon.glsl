#ifndef BP_SDF_COMMON_H
#define BP_SDF_COMMON_H

#define GLOBAL_SDF_WORLD_SIZE 60000.0f
#define GLOBAL_SDF_RASTERIZE_CHUNK_SIZE 32
#define GLOBAL_SDF_RASTERIZE_CHUNK_MARGIN 4

#include <sdf_cb.h>

struct GlobalSDFTrace
{
    float3 worldPosition;
    float minDistance;
    float3 worldDirection;
    float maxDistance;
    float stepScale;
    bool needsHitNormal;
};


struct GlobalSDFHit
{
    float3 hitNormal;
    float hitTime;
    uint hitCascade;
    uint stepsCount;
    float hitSDF;
};


void getGlobalSDFCascadeUV(in GlobalSDFTraceConstants data, uint clip, vec3 worldPosition, out float clipExtend, out vec3 clipUV)
{
    vec4 clipPosDistance = data.clipPosDistance[clip];
    vec3 posInCascade = worldPosition - clipPosDistance.xyz;
    clipExtend = clipPosDistance.w * 2;
    clipUV = clamp( posInCascade / clipExtend + 0.5f, vec3(0) , vec3(1) ); //pos 在clipmap 中的位置 [0,1]
    //textureUV = vec3((clip + cascadeUV.x) / float(data.clipCount), clipUV.y, clipUV.z); //TODO:: clip atlas
}

bool isHit(in GlobalSDFHit hit)
{
    return hit.hitTime >= 0.0f;
}

vec3 getHitPosition(in GlobalSDFHit hit, in GlobalSDFTrace trace)
{
    return trace.worldPosition + trace.worldDirection * hit.hitTime;
}

//(x - closest, y - furthest).
vec2 lineHitAABB(vec3 lineStart, vec3 lineEnd, vec3 boxMin, vec3 boxMax)
{
    vec3 invDirection = 1.0f / (lineEnd - lineStart);
    vec3 enterIntersection = (boxMin - lineStart) * invDirection;
    vec3 exitIntersection = (boxMax - lineStart) * invDirection;
    vec3 minIntersections = min(enterIntersection, exitIntersection);
    vec3 maxIntersections = max(enterIntersection, exitIntersection);
    vec2 intersections;
    intersections.x = max(minIntersections.x, max(minIntersections.y, minIntersections.z));
    intersections.y = min(maxIntersections.x, min(maxIntersections.y, maxIntersections.z));
    return clamp(intersections,0,1);
}


int GetClipmapLevel(vec3 worldPos, vec3 cameraPos, in GlobalSDFTraceConstants data){
    int level = 0;
    for(int i = 1; i < int(data.clipCount); i++){

        if (worldPos.y > cameraPos.y + data.clipPosDistance[i-1].w || worldPos.y <  cameraPos.y - data.clipPosDistance[i-1].w) 
            level = i;

    }
    return level;
}
#endif