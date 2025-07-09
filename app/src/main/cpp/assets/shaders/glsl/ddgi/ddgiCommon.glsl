
#ifndef BP_DDGI_COMMON
#define BP_DDGI_COMMON

#include <ddgi_cb.h>
#include <common/CommonMath.glsl>

//struct DDGIUniform
//{
//    vec4  startPosition;
//    vec4  step;
//    ivec4 probeCounts;
//
//    float maxDistance;
//    float depthSharpness;
//    float hysteresis;
//    float normalBias;
//    
//    float ddgiGamma;
//    int   irradianceProbeSideLength;
//    int   irradianceTextureWidth;
//    int   irradianceTextureHeight;
//    
//    int   depthProbeSideLength;
//    int   depthTextureWidth;
//    int   depthTextureHeight;
//    int   raysPerProbe;
//};

//struct GIPayload
//{
//    vec3  L;
//    vec3  T;
//    float hitDistance;
//    Random random;
//};
//

vec3 sphericalFibonacci(float i,float raysPerProbe) 
{
    const float PHI = sqrt(5) * 0.5 + 0.5;

#define madfrac(A, B) ((A) * (B)-floor((A) * (B)))
    float phi       = 2.0 * PI * madfrac(i, PHI - 1);
    float cosTheta = 1.0 - (2.0 * i + 1.0) * (1.0 / raysPerProbe);
    float sinTheta = sqrt(clamp(1.0 - cosTheta * cosTheta, 0.0f, 1.0f));
    return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
#undef madfrac
}


float signNotZero(in float k)
{
    return (k >= 0.0) ? 1.0 : -1.0;
}

vec2 signNotZero(in vec2 v)
{
    return vec2(signNotZero(v.x), signNotZero(v.y));
}

vec2 octEncode(in vec3 v) 
{
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    vec2 result = v.xy * (1.0 / l1norm);
    if (v.z < 0.0)
        result = (1.0 - abs(result.yx)) * signNotZero(result.xy);
    return result;
}

vec3 octDecode(vec2 o)
{
    vec3 v = vec3(o.x, o.y, 1.0 - abs(o.x) - abs(o.y));

    if (v.z < 0.0)
        v.xy = (1.0 - abs(v.yx)) * signNotZero(v.xy);

    return normalize(v);
}

// Compute normalized oct coord, mapping top left of top left pixel to (-1,-1)
vec2 normalizedOctCoord(ivec2 fragCoord, int probeSideLength)
{
//    int probeWithBorderSide = probeSideLength + 2;
//
//    vec2 octFragCoord = ivec2((fragCoord.x - 2) % probeWithBorderSide, (fragCoord.y - 2) % probeWithBorderSide);
//    // Add back the half pixel to get pixel center normalized coordinates
//    return (vec2(octFragCoord) + vec2(0.5f)) * (2.0f / float(probeSideLength)) - vec2(1.0f, 1.0f);

    int probeWithBorderSide = probeSideLength;

    vec2 octFragCoord = ivec2((fragCoord.x) % probeSideLength, (fragCoord.y) % probeSideLength);
    // Add back the half pixel to get pixel center normalized coordinates

     return (vec2(octFragCoord)) * (2.0f / float(probeSideLength)) - vec2(1.0f, 1.0f); //range [-1,1]

   // return (vec2(octFragCoord) + vec2(0.5f)) * (2.0f / float(probeSideLength)) - vec2(1.0f, 1.0f); //range [-1,1]
}

int getProbeId(vec2 texel, int width, int probeSideLength)
{
//    int probeWithBorderSide = probeSideLength + 2;
//    int probesPerSide     = (width - 2) / probeWithBorderSide;
//    return int(texel.x / probeWithBorderSide) + probesPerSide * int(texel.y / probeWithBorderSide);

    int probeWithBorderSide = probeSideLength; //8
    int probesPerSide     = width / probeWithBorderSide; //
    return int(texel.x / probeWithBorderSide) + probesPerSide * int(texel.y / probeWithBorderSide);
}

vec3 gridToPosition(in DDGIConstants ddgi, ivec3 c)
{
    return ddgi.probeDistance.xyz * vec3(c) + ddgi.startPosition.xyz;
}

//编号先填满x，在填满y, 在填满 z
vec3 probeLocation(in DDGIConstants ddgi, int index)
{
    ivec3 gridCoord;
    gridCoord.x = index % ddgi.probeCounts.x;
    gridCoord.y = (index % (ddgi.probeCounts.x * ddgi.probeCounts.y)) / ddgi.probeCounts.x;
    gridCoord.z = index / (ddgi.probeCounts.x * ddgi.probeCounts.y);

    return gridToPosition(ddgi, gridCoord);
}

ivec3 probeIndexToGridCoord(in DDGIConstants ddgi, int index)
{
    ivec3 gridCoord;
    gridCoord.x = index % ddgi.probeCounts.x;
    gridCoord.y = (index % (ddgi.probeCounts.x * ddgi.probeCounts.y)) / ddgi.probeCounts.x;
    gridCoord.z = index / (ddgi.probeCounts.x * ddgi.probeCounts.y);
    return gridCoord;
}

ivec3 baseGridCoord(in DDGIConstants ddgi, vec3 X) 
{
    return clamp(ivec3((X - ddgi.startPosition.xyz) / ddgi.probeDistance.xyz), ivec3(0, 0, 0), ivec3(ddgi.probeCounts.xyz) - ivec3(1, 1, 1));
}

vec3 gridCoordToPosition(in DDGIConstants ddgi, ivec3 c)
{
    return ddgi.probeDistance.xyz * vec3(c) + ddgi.startPosition.xyz;
}

//Three dimension -> One dimension
int gridCoordToProbeIndex(in DDGIConstants ddgi, in ivec3 probeCoords) 
{
    return int(probeCoords.x + probeCoords.y * ddgi.probeCounts.x + probeCoords.z * ddgi.probeCounts.x * ddgi.probeCounts.y);
}

vec2 textureCoordFromDirection(vec3 dir, int probeIndex, int width, int height, int probeSideLength) 
{
//    vec2 normalizedOctCoord = octEncode(normalize(dir));
//    vec2 normalizedOctCoordZeroOne = (normalizedOctCoord + vec2(1.0f)) * 0.5f;
//
//    float probeWithBorderSide = float(probeSideLength) + 2.0f;
//
//    vec2 octCoordNormalizedToTextureDimensions = (normalizedOctCoordZeroOne * float(probeSideLength)) / vec2(float(width), float(height));
//
//    int probesPerRow = (width - 2) / int(probeWithBorderSide); // how many probes in the texture altas
//
//    vec2 probeTopLeftPosition = vec2(mod(probeIndex, probesPerRow) * probeWithBorderSide,
//        (probeIndex / probesPerRow) * probeWithBorderSide) + vec2(2.0f, 2.0f);
//
//    vec2 probeTopLeftPositionNormalized = vec2(probeTopLeftPosition) / vec2(float(width), float(height));
//
//    return vec2(probeTopLeftPositionNormalized + octCoordNormalizedToTextureDimensions);

    vec2 normalizedOctCoord = octEncode(normalize(dir));
    vec2 normalizedOctCoordZeroOne = (normalizedOctCoord + vec2(1.0f)) * 0.5f;

    float probeWithBorderSide = float(probeSideLength) ;//+ 2.0f

    vec2 octCoordNormalizedToTextureDimensions = (normalizedOctCoordZeroOne * float(probeSideLength)) / vec2(float(width), float(height));

    int probesPerRow = (width) / int(probeWithBorderSide); //(width - 2) / int(probeWithBorderSide); // how many probes in the texture altas

    vec2 probeTopLeftPosition = vec2(mod(probeIndex, probesPerRow) * probeWithBorderSide,
        (probeIndex / probesPerRow) * probeWithBorderSide) ;//+ vec2(2.0f, 2.0f);

    vec2 probeTopLeftPositionNormalized = vec2(probeTopLeftPosition) / vec2(float(width), float(height));

    return vec2(probeTopLeftPositionNormalized + octCoordNormalizedToTextureDimensions);
}
bool allGreaterZero(float3 v){
    return v.x >0.0 &&v.y>0.0 &&v.z>0.0;
}
bool allLessZero(float3 v){
    return v.x <0.0 &&v.y<0.0 &&v.z<0.0;
}
bool IsInsideDDGIVolume(in DDGIConstants ddgi, vec3 P){
    
    float3 volumeMin = ddgi.startPosition.xyz;
    float3 volumeMax = ddgi.startPosition.xyz + ddgi.probeDistance.xyz * (ddgi.probeCounts.xyz - float3(1.0));

    return allGreaterZero(P - volumeMin) && allLessZero(P - volumeMax) ;
    //return clamp(ivec3((X - ddgi.startPosition.xyz) / ddgi.probeDistance.xyz), ivec3(0, 0, 0), ivec3(ddgi.probeCounts.xyz) - ivec3(1, 1, 1));


}

#endif
