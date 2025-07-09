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
#include <ddgi/ddgiCommon.glsl>
#include <sdf/sdfCommon.glsl>
#include <voxelization/voxelCommon.glsl>

layout(local_size_x = 16, local_size_y = 1, local_size_z = 1) in;

//-----------------------------------------------
// Global SDF sets
//-----------------------------------------------
layout(binding = 0) uniform sampler3D sdfClip0;  
layout(binding = 1) uniform sampler3D sdfClip1;  
layout(binding = 2) uniform sampler3D sdfClip2;  
layout(binding = 3) uniform sampler3D sdfClip3;  

layout(binding = 0, std140) uniform GDFUbo
{
    //GlobalSurfaceAtlasData data;
    GlobalSDFTraceConstants g_sdfData;
};

//-----------------------------------------------
// Voxel sets
//-----------------------------------------------

layout(binding = 5) uniform sampler3D voxelTexture3D;
layout(binding = 1, std140)  uniform VoxelUBO {
    VoxelConstants g_voxel;
};

//-----------------------------------------------
// Skybox sets
//-----------------------------------------------


layout(binding = 7) uniform samplerCube u_Skybox0;
layout(binding = 8) uniform samplerCube u_Skybox1;
layout(binding = 9) uniform samplerCube u_Skybox2;

layout(binding = 2, std140) uniform SkyConstantsUbo {

   SkyConstants g_SkyConstants;

};

//-----------------------------------------------
// DDGI sets
//-----------------------------------------------

layout (binding = 3, std140) uniform DDGIUBO
{
    DDGIConstants g_ddgi;
};
layout (binding = 4, std140) uniform DDGIRayUBO
{
    DDGIRayConstants g_pushConsts;
};


//-----------------------------------------------
// SDFTrace Output sets
//-----------------------------------------------


//rgba16f,
//rgba16f,
layout(binding = 0, rgba16f) writeonly uniform image2D iRadiance;
layout(binding = 1, rgba16f) writeonly uniform image2D iDirectionDistance;




float GetVoxelMipLevel(vec3 worldPos, in VoxelConstants voxelConst){
        
        float dist = length(worldPos - voxelConst.center);
        float voxelSize = (voxelConst.areaExtent / voxelConst.dimension).x;
		float l = (1+  dist/voxelSize); //跨过了多少Voxel
		float level = log2(l);

        return level;
}


vec4 GetSurfaceColorFromVoxel(in vec3 worldPos, in sampler3D voxelTex, in VoxelConstants voxelConst)
{
    float mipLevel = GetVoxelMipLevel(worldPos, voxelConst);
    vec3 originInVoxel = (worldPos - voxelConst.center);
	vec3 currentPoint =  (originInVoxel) / g_voxel.areaExtent; //[-1,1]
	if(!isInsideCube(currentPoint,0))
        return vec4(0.0);

	vec4 currentSample = textureLod(voxelTex, scaleAndBias(currentPoint), mipLevel);

    return currentSample;

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
                                        sdfClip0, 
                                        sdfClip1, 
                                        sdfClip2, 
                                        sdfClip3,
                                        trace, 
                                        0); 
    

    vec4 radiance = vec4(0);

    if(isHit(hit))
    {
        if (hit.hitSDF <= 0.0f && hit.hitTime <= g_sdfData.clipVoxelSize[0])
        {
            radiance = vec4(0, 0, 0, GLOBAL_SDF_WORLD_SIZE);
        }
        else
        {
            vec3 hitPosition = getHitPosition(hit,trace);
            //float surfaceThreshold = getGlobalSurfaceAtlasThreshold(g_sdfData, hit);

            vec4 surfaceColor = GetSurfaceColorFromVoxel(hitPosition,voxelTexture3D, g_voxel);

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