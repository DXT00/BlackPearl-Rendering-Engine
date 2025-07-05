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


void GetClipmap(int id, out sampler3D clip){


}


GlobalSDFHit tracyGlobalSDF(in GlobalSDFTraceConstants data, 
                            sampler3D clip0, 
                            sampler3D clip1, 
                            sampler3D clip2, 
                            sampler3D clip3, 
                            in GlobalSDFTrace trace, 
                            float cascadeTraceStartBias)
{
    GlobalSDFHit hit;
    hit.stepsCount = 0;
    hit.hitTime = -1.0f;
    hit.hitNormal = vec3(0);
    hit.hitCascade = 0;
    hit.hitSDF = 0.0;

    float traceMaxDistance = min(trace.maxDistance, data.clipPosDistance[ data.clipCount - 1 ].w * 2);
    
    float chunkSizeDistance     = float(GLOBAL_SDF_RASTERIZE_CHUNK_SIZE) /  data.dimension;  // Size of the chunk in SDF distance (0-1)
    float chunkMarginDistance   = float(GLOBAL_SDF_RASTERIZE_CHUNK_MARGIN) / data.dimension; // Size of the chunk margin in SDF distance (0-1)
    
    float nextIntersectionStart = 0.0f;
    vec3  traceEndPosition = trace.worldPosition + trace.worldDirection * traceMaxDistance;

    for (uint clip = 0; clip < data.clipCount && hit.hitTime < 0.0f; clip++)
    {
        vec4  clipPosDistance    = data.clipPosDistance[clip];
        float voxelSize          = data.clipVoxelSize[clip];
        float voxelHalf          = voxelSize * 0.5f;
        vec3  worldPosition      = trace.worldPosition + trace.worldDirection * (voxelSize * cascadeTraceStartBias);

        vec2 intersections  = lineHitAABB(worldPosition, traceEndPosition, clipPosDistance.xyz - clipPosDistance.www, clipPosDistance.xyz + clipPosDistance.www);
        intersections.xy    *= traceMaxDistance;
        intersections.x     = max(intersections.x, nextIntersectionStart);

        float stepTime      = intersections.x;
//        if (intersections.x >= intersections.y)// closest is greater than furthest
//        {
//            stepTime = intersections.y;
//        }
//        else
//        {
//            // Skip the current clip, tracing on the next clip
//            nextIntersectionStart = intersections.y;
//        }

        if (intersections.x < intersections.y)
        {

            nextIntersectionStart = intersections.y;

            //Raymarching
            uint step = 0;
            for (; step < 250 && stepTime < intersections.y; step++)
            {
                vec3 stepPosition = worldPosition + trace.worldDirection * stepTime;

                float clipExtend;
                vec3  clipUV;//, textureUV;
                getGlobalSDFCascadeUV(data, clip, stepPosition, clipExtend, clipUV);
                 float stepDistance;
                if(clip == 0){
                    stepDistance = texture(clip0,clipUV).r;
                }
                else if(clip == 1){
                    stepDistance = texture(clip1,clipUV).r;
                }
                else if(clip == 2){
                    stepDistance = texture(clip2,clipUV).r;
                }
                else if(clip == 3){
                    stepDistance = texture(clip3,clipUV).r;
                }

//                if (stepDistance < chunkSizeDistance)
//                {
//                    float stepDistanceTex = texture(tex,textureUV).r;
//                    if (stepDistanceTex < chunkMarginDistance * 2)
//                    {
//                        stepDistance = stepDistanceTex;
//                    }
//                }
//                else
//                {
//                    stepDistance = chunkSizeDistance;
//                }
//
                //stepDistance *= clipExtend;

                float minSurfaceThickness = voxelHalf * clamp(stepTime / voxelSize, 0.0, 1.0);
                if (stepDistance < minSurfaceThickness)
                {
                    hit.hitTime = max(stepTime + stepDistance - minSurfaceThickness, 0.0f);
                    hit.hitCascade = clip;
                    hit.hitSDF = stepDistance;
                    if (trace.needsHitNormal)
                    {
                        // Calculate hit normal from SDF gradient
                        //dU/dX, dU/dY
//                        float texelOffset = 1.0f / data.dimension;
//                        float xp = texture(tex, vec3(textureUV.x + texelOffset, textureUV.y, textureUV.z)).r;
//                        float xn = texture(tex, vec3(textureUV.x - texelOffset, textureUV.y, textureUV.z)).r;
//                        float yp = texture(tex, vec3(textureUV.x, textureUV.y + texelOffset, textureUV.z)).r;
//                        float yn = texture(tex, vec3(textureUV.x, textureUV.y - texelOffset, textureUV.z)).r;
//                        float zp = texture(tex, vec3(textureUV.x, textureUV.y, textureUV.z + texelOffset)).r;
//                        float zn = texture(tex, vec3(textureUV.x, textureUV.y, textureUV.z - texelOffset)).r;
//                        hit.hitNormal = normalize(vec3(xp - xn, yp - yn, zp - zn));
                    }
                    break;
                }

                // Move 
                stepTime += max(stepDistance * trace.stepScale, voxelSize);
            }

            hit.stepsCount += step;

        
        }
       
    }
    return hit;
}


#endif