#type compute
#version 450 core

//#include "DDGICommon.glsl"
//#include "../Common/Math.glsl"
//#include "../SDF/SDFCommon.glsl"
//#include "../SDF/AtlasCommon.glsl"
//
#include <sdf_cb.h>
#include <voxel_cb.h>
#include <ddgi_cb.h>
#include <sky_cb.h>
#include <common/CommonMath.glsl>
#include <common/CommonViewStruct.glsl>
#include <ddgi/ddgiCommon.glsl>
#include <sdf/sdfCommon.glsl>
#include <voxelization/voxelCommon.glsl>

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

//-----------------------------------------------
// Global SDF sets
//-----------------------------------------------
layout(binding = 0, r16f) uniform readonly image3D sdfClip0;  
layout(binding = 1, r16f) uniform readonly image3D sdfClip1;  
layout(binding = 2, r16f) uniform readonly image3D sdfClip2;  
layout(binding = 3, r16f) uniform readonly image3D sdfClip3;  

layout(binding = 1, std140) uniform GDFUbo
{
    //GlobalSurfaceAtlasData data;
    GlobalSDFTraceConstants g_sdfData;
};

//-----------------------------------------------
// Voxel sets
//-----------------------------------------------

layout(binding = 5) uniform sampler3D voxelTexture3D;
layout(binding = 2, std140)  uniform VoxelUBO {
    VoxelConstants g_voxel;
};

//-----------------------------------------------
// Skybox sets
//-----------------------------------------------


layout(binding = 7) uniform samplerCube u_Skybox0;
layout(binding = 8) uniform samplerCube u_Skybox1;
layout(binding = 9) uniform samplerCube u_Skybox2;

layout(binding = 3, std140) uniform SkyConstantsUbo {

   SkyConstants g_SkyConstants;

};

//-----------------------------------------------
// DDGI sets
//-----------------------------------------------

layout (binding = 4, std140) uniform DDGIUBO
{
    DDGIConstants g_ddgi;
};
layout (binding = 5, std140) uniform DDGIRayUBO
{
    DDGIRayConstants g_pushConsts;
};


//-----------------------------------------------
// SDFTrace Output sets
//-----------------------------------------------


//rgba16f,
//rgba16f,
layout(binding = 4, rgba16f) writeonly uniform image2D iRadiance;
layout(binding = 5, rgba16f) writeonly uniform image2D iDirectionDistance;




float GetVoxelMipLevel(vec3 worldPos, in VoxelConstants voxelConst){
        
        float dist = length(worldPos - voxelConst.center);
        float voxelSize = (voxelConst.areaExtent / voxelConst.dimension).x;
		float l = (1+  dist/voxelSize); //跨过了多少Voxel
		float level = log2(l);

        return level;
}


vec4 GetSurfaceColorFromVoxel(in vec3 worldPos, vec3  direction , in sampler3D voxelTex, in VoxelConstants voxelConst)
{



// Trace.
    uint numberOfSteps = 128;
   vec3 originInVoxel = (worldPos - voxelConst.center);
   float voxelSize = (voxelConst.areaExtent / voxelConst.dimension).x;

	vec4 color = vec4(0.0f);
    vec3 bias = direction*5;
    originInVoxel -= bias;
	for(uint step_ = 0; step_ < numberOfSteps && color.a < 0.99f; ++step_) {
        
        vec3 pos = (originInVoxel + voxelSize * float(step_) * direction);
        float mipLevel = GetVoxelMipLevel(pos, voxelConst);

		vec3 currentPoint =  pos / voxelConst.areaExtent; //[-1,1]

		if(!isInsideCube(currentPoint,0))break;
		
        vec4 currentSample = textureLod(voxelTex, scaleAndBias(currentPoint), mipLevel);///u_CubeSize.x
		//if(currentSample.a!=-1)//if voxel is not empty
		color += (1.0f - color.a) * currentSample;
	} 


//
//	vec3 currentPoint =  (originInVoxel) / g_voxel.areaExtent; //[-1,1]
//	if(!isInsideCube(currentPoint,2.0))
//        return vec4(0.0);
//
//	vec4 currentSample = textureLod(voxelTex, scaleAndBias(currentPoint), mipLevel);

    return color;

}



struct ClipmapSample {
    float distance;
    int level;
};

// 从clipmap层级采样 wordPos的SDF
//worldPos : 某一点的世界坐标
ClipmapSample sampleClipmap(
     vec3 worldPos, 
     vec3 cameraPos, 
     in GlobalSDFTraceConstants data) {
    // 确定使用哪个clipmap层级(这里简单根据y高度选择), todo:: 默认3层 clip, 不是3层要做筛选
    int level = GetClipmapLevel(worldPos, cameraPos,data);


    float3 center = data.clipPosDistance[level].xyz;
    float extend = data.clipPosDistance[level].w * 2.0;
    float voxelSize = data.clipVoxelSize[level];
    // 计算纹理坐标
    vec3 voxelCoord = ((worldPos - center) / voxelSize) + data.dimension * 0.5; //[0, dimesion]
    vec3 texCoord = voxelCoord/data.dimension; //[0,1]
    // 采样纹理
    float distance = 0.0;
    if (level == 0) {
        distance = imageLoad(sdfClip0, ivec3(voxelCoord)).r;
    } else if (level == 1) {
        distance = imageLoad(sdfClip1, ivec3(voxelCoord)).r;
    } else if (level == 2){
        distance = imageLoad(sdfClip2, ivec3(voxelCoord)).r;
    } else if (level == 3){
        distance = imageLoad(sdfClip3, ivec3(voxelCoord)).r;
    }
    //distance 解码
    distance = distance* 2.0 -1.0; //[-1,1]
    distance = distance * data.dimension * voxelSize;

    // 返回采样结果
    return ClipmapSample(distance, level);
}


GlobalSDFHit tracyGlobalSDF(in GlobalSDFTraceConstants data, 
                            vec3 cameraPos,
                            in GlobalSDFTrace trace, 
                            float cascadeTraceStartBias)
{
    GlobalSDFHit hit;
    hit.stepsCount = 0;
    hit.hitTime = -1.0f;
    hit.hitNormal = vec3(0);
    hit.hitCascade = 0;
    hit.hitSDF = 0.0;

    float traceMaxDistance = min(trace.maxDistance, data.clipPosDistance[data.clipCount - 1 ].w * 2);
    
    float chunkSizeDistance     = float(GLOBAL_SDF_RASTERIZE_CHUNK_SIZE) /  data.dimension;  // Size of the chunk in SDF distance (0-1)
    float chunkMarginDistance   = float(GLOBAL_SDF_RASTERIZE_CHUNK_MARGIN) / data.dimension; // Size of the chunk margin in SDF distance (0-1)
    
    float nextIntersectionStart = 0.0f;
    vec3  traceEndPosition = trace.worldPosition + trace.worldDirection * traceMaxDistance;

     float t = 0.0;
     float maxSteps = 256;
     for (int i = 0; i < maxSteps ; i++) {
        vec3 p = trace.worldPosition +  trace.worldDirection * t;
        ClipmapSample sam =  sampleClipmap(p, cameraPos,data);

        float d = sam.distance;
      
        if(t > traceMaxDistance){
            break;

        }
        if (abs(d) < 0.1 ){
                hit.hitTime = max(t, 0.0f);
                hit.hitCascade = sam.level;
                hit.hitSDF = d;
        }

        t += d;
    }



//
//
//
//    for (uint clip = 0; clip < data.clipCount && hit.hitTime < 0.0f; clip++)
//    {
//        vec4  clipPosDistance    = data.clipPosDistance[clip];
//        float clipExtend         = data.clipPosDistance[clip].w * 2.0;
//        float voxelSize          = data.clipVoxelSize[clip];
//        float voxelHalf          = voxelSize * 0.5f;
//        vec3  worldPosition      = trace.worldPosition + trace.worldDirection * (voxelSize * cascadeTraceStartBias);
//
//        vec2 intersections  = lineHitAABB(worldPosition, traceEndPosition, clipPosDistance.xyz - clipPosDistance.www, clipPosDistance.xyz + clipPosDistance.www);
//        intersections.xy    *= traceMaxDistance;
//        intersections.x     = max(intersections.x, nextIntersectionStart);
//
//
//        if (intersections.x < intersections.y)
//        {
//
//            nextIntersectionStart = intersections.y;
//
//            //Raymarching
//            uint step = 0;
//            for (; step < 250 && stepTime < intersections.y; step++)
//            {
//                vec3 stepPosition = worldPosition + trace.worldDirection * stepTime;
//
//                float clipExtend;
//                vec3  clipUV;//, textureUV;
//                getGlobalSDFCascadeUV(data, clip, stepPosition, clipExtend, clipUV);
//                 float stepDistance;
//                if(clip == 0){
//                    stepDistance = texture(clip0,clipUV).r;
//                }
//                else if(clip == 1){
//                    stepDistance = texture(clip1,clipUV).r;
//                }
//                else if(clip == 2){
//                    stepDistance = texture(clip2,clipUV).r;
//                }
//                else if(clip == 3){
//                    stepDistance = texture(clip3,clipUV).r;
//                }
//                stepDistance = stepDistance*2.0 -1.0;
//                stepDistance *= clipExtend;
//
//
//                float minSurfaceThickness = voxelHalf * clamp(stepTime / voxelSize, 0.0, 1.0);
//                if (stepDistance < minSurfaceThickness)
//                {
//                    hit.hitTime = max(stepTime + stepDistance - minSurfaceThickness, 0.0f);
//                    hit.hitCascade = clip;
//                    hit.hitSDF = stepDistance;
//
//                    break;
//                }
//
//                // Move 
//                stepTime +=  max(stepDistance * trace.stepScale, voxelSize);
//            }
//
//            hit.stepsCount += step;
//
//        
//        }
//       
//    }
    return hit;
}


void main()
{
    const ivec2 texCoords   = ivec2(gl_GlobalInvocationID.xy);
    const int rayId         = texCoords.x;
    const int probeId       = texCoords.y;

    if(rayId >= g_ddgi.raysPerProbe)
        return;

    vec3  rayOrigin = probeLocation(g_ddgi, probeId);

    //sphericalFibonacci 生成一个 均匀分布在球面上的方向向量（基于球面斐波那契分布）。 
    vec3  direction  = normalize(mat3(g_pushConsts.randomOrientation) * sphericalFibonacci(rayId, g_ddgi.raysPerProbe));

   // Random random = randomInit(texCoords, g_pushConsts.numFrames);

    float hitDistance = GLOBAL_SDF_WORLD_SIZE;

    GlobalSDFTrace trace;

    trace.worldPosition = rayOrigin;
    trace.worldDirection = direction;
    trace.minDistance = 0;
    trace.maxDistance = GLOBAL_SDF_WORLD_SIZE;
    trace.stepScale = 1.0;
    trace.needsHitNormal = true;
    GlobalSDFHit hit = tracyGlobalSDF(
                                        g_sdfData, 
                                        g_pushConsts.cameraPos,
                                        trace, 
                                        0); 
    

    vec4 radiance = vec4(0);

    if(isHit(hit))
    {
        //hitSDF (setpDistance) <0, 表明 trace到物体内部了！
//        if (hit.hitSDF < 0.0f && hit.hitTime <= g_sdfData.clipVoxelSize[0]) //1.0
//        {
//            radiance = vec4(0, 0, 0, GLOBAL_SDF_WORLD_SIZE);
//        }
//        else
        {
            vec3 hitPosition = getHitPosition(hit,trace);
            //float surfaceThreshold = getGlobalSurfaceAtlasThreshold(g_sdfData, hit);
            vec4 surfaceColor = GetSurfaceColorFromVoxel(hitPosition, trace.worldDirection, voxelTexture3D, g_voxel);
//            if(hit.hitCascade == 0u)
//                 surfaceColor = vec4(1.0,0.0,0.0,1.0);//GetSurfaceColorFromVoxel(hitPosition,voxelTexture3D, g_voxel);
//            else if(hit.hitCascade == 1u)
//                 surfaceColor = vec4(0.0,1.0,0.0,1.0);
//
//           else if(hit.hitCascade == 2u)
//                 surfaceColor = vec4(0.0,0.0,1.0,1.0);
//            else if(hit.hitCascade == 3u)
//                 surfaceColor = vec4(1.0,1.0,0.0,1.0);
//
//

//            sampleGlobalSurfaceAtlas(surfaceColor,ubo.data, 
//                                atlasChunks.data,  
//                                cullObjects.data,  
//                                objects.data, 
//                                atlasTiles.data,  
//                                uSurfaceAtlasDepth,  
//                                uSurfaceAtlasTex, 
//                                getHitPosition(hit, trace),
//                                hit.hitNormal, 
//                                surfaceThreshold ,
//                                false);
            //get color from voxel


            radiance = vec4(surfaceColor.rgb, hit.hitTime);

            radiance.w = max(radiance.w + g_sdfData.clipVoxelSize[hit.hitCascade] * 0.5f, 0);
        }
    }
    else
    {
            vec4 skyBoxColor =// texture(u_Skybox0, TexCoords);
	        g_SkyConstants.factors.x * texture(u_Skybox0,direction)
				        +g_SkyConstants.factors.y * texture(u_Skybox1,direction)
				        +g_SkyConstants.factors.z * texture(u_Skybox2,direction);//*vec4(u_Material.diffuseColor,1.0);


        //TODO::
        radiance = vec4(skyBoxColor.rgb, GLOBAL_SDF_WORLD_SIZE);  
    }
    
    imageStore(iRadiance, texCoords, vec4(radiance.xyz, 0.0f));
    imageStore(iDirectionDistance, texCoords, vec4(direction, radiance.w));
}

// ------------------------------------------------------------------------