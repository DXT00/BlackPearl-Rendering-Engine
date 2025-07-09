#type compute
#version 450 core

#define NUM_THREADS_X 32
#define NUM_THREADS_Y 32

#include<ddgi_cb.h>
layout(local_size_x = NUM_THREADS_X, local_size_y = NUM_THREADS_Y, local_size_z = 1) in;


layout(binding = 0, rgba16f) uniform image2D outColor;
layout(binding = 0)  uniform sampler2D  uIrradiance0;
layout(binding = 1)  uniform sampler2D  uDepth0;
layout(binding = 2)  uniform sampler2D  uIrradiance1;
layout(binding = 3)  uniform sampler2D  uDepth1;
layout(binding = 4)  uniform sampler2D  uIrradiance2;
layout(binding = 5)  uniform sampler2D  uDepth2;
layout(binding = 6)  uniform sampler2D  uIrradiance3;
layout(binding = 7)  uniform sampler2D  uDepth3;


layout(binding = 8) uniform sampler2D SceneDepth;//uDepthSampler;
layout(binding = 9) uniform sampler2D GBufferA;//uNormalSampler;encode normal.xy + Encode IndirectIrradiance + reserve

layout(std140, binding = 2) buffer DDGIUbo {
	DDGIConstants g_volumes[];
};

//

layout(std140, binding = 3) uniform volumesInfoUbo
{
    DDGIAreaConstants g_areaInfo;
};
//
#include <common/CommonMath.glsl>
#include <ddgi/ddgiCommon.glsl>
#include <common/CommonOctahedral.glsl>
#include <common/CommonDepth.glsl>
#include <common/CommonViewStruct.glsl>
#include <common/CommonTransformStruct.glsl>
//#include <common/CommonTransform.glsl>

const float FLT_EPS = 0.00000001;

vec3 GetVolumeIrradianceTexture(int volumeId,  vec2 uv){
    if(volumeId == 0){
       return textureLod(uIrradiance0, uv, 0.0f).rgb;
    }else if(volumeId == 1){
       return textureLod(uIrradiance1, uv, 0.0f).rgb;

    }else if(volumeId == 2){
       return textureLod(uIrradiance2, uv, 0.0f).rgb;
    }else if(volumeId == 3){
       return textureLod(uIrradiance3, uv, 0.0f).rgb;
    }
    return vec3(0.0);

}

vec2 GetVolumeDepthTexture(int volumeId, vec2 uv){
    if(volumeId == 0){
    return textureLod(uDepth0, uv, 0.0f).rg;

    }else if(volumeId == 1){
    return textureLod(uDepth1, uv, 0.0f).rg;
    }else if(volumeId == 2){
    return textureLod(uDepth2, uv, 0.0f).rg;
    }else if(volumeId == 3){
    return textureLod(uDepth3, uv, 0.0f).rg;
    }

    return vec2(0.0);
}


/*
        6-------7
       /|      /|
      / |     / |
     2-------3  |
     |  | .p |  |
     |  4----|--5
     | /     | /
     |/      |/
     0-------1
*/

//P vertex.position
//N vertex.normal
vec3 sampleIrradiance(in DDGIConstants ddgi, vec3 P, vec3 N, vec3 Wo, int samplerId)
{

    ivec3 baseGridCoord = baseGridCoord(ddgi, P); //P 所在probe的xyz id
    vec3 baseProbePos   = gridCoordToPosition(ddgi, baseGridCoord); //P 所在probe的位置
    
    vec3  sumIrradiance = vec3(0.0f);
    float sumWeight = 0.0f;

    vec3 alpha = clamp((P - baseProbePos) / ddgi.probeDistance.xyz, vec3(0.0f), vec3(1.0f));

    for (int i = 0; i < 8; ++i) 
    {
        ivec3 offset = ivec3(i, i >> 1, i >> 2) & ivec3(1);
        ivec3 probeGridCoord = clamp(baseGridCoord + offset, ivec3(0), ddgi.probeCounts.xyz - ivec3(1)); //边界probe会clamp to edge
        vec3 probePos = gridToPosition(ddgi, probeGridCoord);

        vec3 trilinear = mix(1.0 - alpha, alpha, offset);
        float weight = 1.0;

        // Smooth backface test
    
        vec3 dirToProbe = normalize(probePos - P);
        weight *= square(max(0.0001, (dot(dirToProbe, N) + 1.0) * 0.5)) + 0.2;

        int probeIdx = gridCoordToProbeIndex(ddgi, probeGridCoord);


        // Moment visibility test
        vec3 vBias = (N + 3.0 * Wo) * ddgi.normalBias;
        vec3 probeToPoint = P - probePos + vBias;
        vec3 dir = normalize(-probeToPoint);

        vec2 texCoord = textureCoordFromDirection(-dir, probeIdx, ddgi.depthTextureWidth, ddgi.depthTextureHeight, ddgi.depthProbeSideLength);

        float dist = length(probeToPoint);

        vec2 temp = GetVolumeDepthTexture(samplerId, texCoord);   
        float mean = temp.x;
        float variance = abs(square(temp.x) - temp.y);

        float chebyshevWeight = variance / (variance + square(max(dist - mean, 0.0)));
            
        chebyshevWeight = max(chebyshevWeight * chebyshevWeight * chebyshevWeight, 0.0);

        weight *= (dist <= mean) ? 1.0 : chebyshevWeight;

        weight = max(0.000001, weight);
                 
        vec3 irradianceDir = N;

        texCoord = textureCoordFromDirection(normalize(irradianceDir), probeIdx, ddgi.irradianceTextureWidth, ddgi.irradianceTextureHeight, ddgi.irradianceProbeSideLength);

        vec3 probeIrradiance = GetVolumeIrradianceTexture(samplerId, texCoord);
     
      //  probeIrradiance = pow(probeIrradiance, vec3(ddgi.ddgiGamma * 0.5f));

        const float crushThreshold = 0.2f;
        if (weight < crushThreshold)
            weight *= weight * weight * (1.0f / square(crushThreshold)); 

        // Trilinear weights
        weight *= trilinear.x * trilinear.y * trilinear.z;
        sumIrradiance += weight * probeIrradiance;
        sumWeight += weight;
    }

    vec3 netIrradiance = sumIrradiance / sumWeight;
   // netIrradiance   *= netIrradiance; 

    return netIrradiance;// 2 * PI * netIrradiance;
}


vec3 worldPositionFromDepth(vec2 texCoords, float ndcDepth, mat4 viewProjInv)
{
    vec2 screenPos = texCoords * 2.0 - 1.0; //[-1,1]
    vec4 ndcPos = vec4(screenPos, ndcDepth, 1.0);
    vec4 worldPos = viewProjInv * ndcPos;
    worldPos = worldPos / worldPos.w;
    return worldPos.xyz;
}


/*
    gl_pos /w = ndc
    gl_pos = p x v x model 
    ndc = w*p x v x model 
    world Pos = model = inverse(p x v) * ndc/w ;
*/

float3 ScreenSpaceToWorldPosition(float2 pixelPos, float z_viewSpace)
{    
    float4 viewPos = float4(pixelPos.x, pixelPos.y, z_viewSpace, 1.0);
    float4 clipPos = g_View.matProjection* viewPos;
    float4 worldPos = clipPos * inverse(g_View.matProjectionView);
    return worldPos.xyz;
}
void main()
{
    const ivec2 currentCoord = ivec2(gl_GlobalInvocationID.xy);
    const ivec2 size         = textureSize(SceneDepth,0);
    if(currentCoord.x >= size.x || currentCoord.y >= size.y)
        return;

    const vec2  pixelCenter  = vec2(currentCoord) + vec2(0.5);

    for(int volumeId = 0; volumeId < g_areaInfo.numVolumeInArea; volumeId++){
         
         const vec2  texelSize    = vec2(1.0f) / vec2(size);
         
         const vec2  texCoord     = pixelCenter / vec2(size);
         
         float depth = texelFetch(SceneDepth, currentCoord, 0).r;
         
         if (depth == 1.0f)
         {
             imageStore(outColor, currentCoord, vec4(0.0f));
             //imageStore(outColor, currentCoord, vec4(1.0,1.0,0.0,1.0));

             return;
         }
       //  float sceneZ = ConvertFromDeviceZ(depth, g_View.zNear, g_View.zFar);
         const vec3 P  = worldPositionFromDepth(texCoord, depth, inverse(g_View.matProjectionView));
         
         const vec3 N  = OctahedronToUnitVector(texture(GBufferA,texCoord).xy * 2.0 - 1.0);//octohedralToDirection(texelFetch(GbufferA, currentCoord, 0).xy);
         const vec3 Wo = normalize(g_View.cameraPos - P);
         vec3 irradiance =  vec3(0.0);
         if(IsInsideDDGIVolume(g_volumes[volumeId], P)){
             irradiance = sampleIrradiance(g_volumes[volumeId], P, N, Wo, volumeId); //vec3(1.0,0.0,0.0);//
         }
         //irradiance = P;//vec3(sceneZ,0.0,0.0);//vec3(1.0,0.0,0.0);
         imageStore(outColor, currentCoord, vec4(irradiance, 1.0f));
    
    }

   
}