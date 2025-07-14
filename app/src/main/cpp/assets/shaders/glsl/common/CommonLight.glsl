
#ifndef BP_COMMON_LIGHT_H
#define BP_COMMON_LIGHT_H

#include <assets/shaders/glsl/common/CommonMath.glsl>



/** 
 * Returns a radial attenuation factor for a point light.  
 * worldLightVector is the vector from the position being shaded to the light, divided by the radius of the light. 
 */
float RadialAttenuationMask(float3 worldLightVector)
{
	float NormalizeDistanceSquared = dot(worldLightVector, worldLightVector);
	return 1.0f - saturate(NormalizeDistanceSquared);
}
float RadialAttenuation(float3 worldLightVector, half falloffExponent)
{
	// Old (fast, but now we not use the default of 2 which looks quite bad):
	return pow(RadialAttenuationMask(worldLightVector), falloffExponent);

}

/** 
 * Calculates attenuation for a spot light.
 * L normalize vector to light. 
 * SpotDirection is the direction of the spot light.
 * SpotAngles.x is CosOuterCone, SpotAngles.y is InvCosConeDifference. 
 */
float SpotAttenuationMask(float3 L, float3 SpotDirection, float2 SpotAngles)
{
	return saturate((dot(L, -SpotDirection) - SpotAngles.x) * SpotAngles.y);
}
float SpotAttenuation(float3 L, float3 SpotDirection, float2 SpotAngles)
{
	float ConeAngleFalloff = square(SpotAttenuationMask(L, SpotDirection, SpotAngles));
	return ConeAngleFalloff;
}


float GetLocalLightAttenuation(
	in float3 fragPos, 
	in LightConstants light)
{
	float3 ToLight = light.position - fragPos;
		
	float DistanceSqr = dot( ToLight, ToLight );
	float3 L = ToLight * 1.0 / sqrt(DistanceSqr);

	float LightMask;

	if (light.lightType == LightType_Directional) {

		return 1.0;
	}



	if (light.bInverseSquared != 0)
	{
		LightMask = square( saturate( 1.0 - square( DistanceSqr * square(light.invRadius) ) ) );
		// This extra attenuation has been added for supporting existing 'legacy' shading model on mobile. 
		// This is not needed for Strata which unifies all lighting paths
#if SHADING_PATH_MOBILE 
		LightMask *= 1.0f / (DistanceSqr + 1.0f);
#endif
	}
	else
	{
		LightMask = RadialAttenuation(ToLight * light.invRadius, light.falloffExponent);
	}

	if (light.lightType == LightType_Spot)
	{
    
        float CosOuterCone = light.innerAngle;
        float CosInnerCone = light.outerAngle;
        float InvCosConeDifference = 1.0f / (CosInnerCone - CosOuterCone);
		LightMask *= SpotAttenuation(L, -light.direction, vec2(CosOuterCone,InvCosConeDifference ));
	}

	if(light.lightType == LightType_Rect)
	{
		// Rect normal points away from point
		LightMask = dot( light.direction, L ) < 0.0 ? 0.0 : LightMask;
	}

	return LightMask;
}

#endif //COMMON_LIGHT_H