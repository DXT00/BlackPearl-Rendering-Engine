

#include <assets/shaders/glsl/common/CommonTextureSample.glsl>


half DielectricSpecularToF0(half Specular)
{
	return half(0.08f * Specular);
}


half3 ComputeF0(half Specular, half3 BaseColor, half Metallic)
{
	return mix(DielectricSpecularToF0(Specular).xxx, BaseColor, Metallic.xxx);
}

float3 ComputeF90(float3 F0, float3 EdgeColor, float Metallic)
{
	return mix(float3(1.0), EdgeColor, Metallic.xxx);
}


#if COOK
	#include <assets/shaders/glsl/bsdf/CookBSDF.glsl>
#elif (Disney)
	#include <assets/shaders/glsl/bsdf/DisneyBSDF.glsl>
#endif