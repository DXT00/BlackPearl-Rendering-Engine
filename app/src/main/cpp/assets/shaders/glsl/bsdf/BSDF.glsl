
#ifndef BP_BSDF_H
#define BP_BSDF_H
#include <assets/shaders/glsl/common/CommonTextureSample.glsl>


half DielectricSpecularToF0(half Specular)
{
    half f0 = 0.08f * Specular;
	return f0;
}


half3 ComputeF0(half Specular, half3 BaseColor, half Metallic)
{
	return mix(half3(DielectricSpecularToF0(Specular)), BaseColor, half3(Metallic));
}

float3 ComputeF90(float3 F0, float3 EdgeColor, float Metallic)
{
	return mix(float3(1.0), EdgeColor, float3(Metallic));
}


#if COOK
	#include <assets/shaders/glsl/bsdf/CookBSDF.glsl>
#elif (Disney)
	#include <assets/shaders/glsl/bsdf/DisneyBSDF.glsl>
#endif

#endif