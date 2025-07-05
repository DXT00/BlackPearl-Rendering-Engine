
#ifndef DDGI_CB_H
#define DDGI_CB_H
#include "align.h"

struct DDGIConstants
{
    ALIGN(16) float4  startPosition;
    ALIGN(16) float4  probeDistance;//align
    ALIGN(16) int4   probeCounts;

    ALIGN(4) float maxDistance;
    ALIGN(4) float depthSharpness;
    ALIGN(4) float hysteresis;
    ALIGN(4) float normalBias;

    ALIGN(4) float   ddgiGamma;// = 5.f;
    ALIGN(4) int irradianceProbeSideLength;//  = IrradianceOctSize;
    ALIGN(4) int irradianceTextureWidth;
    ALIGN(4) int irradianceTextureHeight;

    ALIGN(4) int depthProbeSideLength;// = DepthOctSize;
    ALIGN(4) int depthTextureWidth;
    ALIGN(4) int depthTextureHeight;
    ALIGN(4) int raysPerProbe;// = 128;
};

struct DDGIRayConstants {
    ALIGN(16) float4x4  randomOrientation;
    ALIGN(4)  uint  numFrames;
    ALIGN(4)  uint  infiniteBounces;
    ALIGN(4)  int   numLights;
    ALIGN(4)  float intensity;
};

#endif // DDGI_CB_H