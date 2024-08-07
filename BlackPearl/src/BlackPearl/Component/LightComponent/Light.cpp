#include "pch.h"
#include "Light.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "SpotLight.h"
namespace BlackPearl {

	Light * Light::Create(LightType type, const math::float3 & position, const math::float3& direction, const float cutOffAngle, const float outterCutOffAngle, Props props)
	{
		
		switch (type)
		{
		case LightType::ParallelLight:
			return DBG_NEW ParallelLight(props);
		case LightType::PointLight:
			return DBG_NEW PointLight(props);
		case LightType::SpotLight:
			return DBG_NEW SpotLight(props);
		default:
			GE_CORE_ERROR("Unknown Light Type!")
				break;
		}
		return nullptr;
	}
	void Light::FillLightConstants(LightConstants& lightConstants) const
	{
		lightConstants.color = m_LightProp.diffuse;
		lightConstants.shadowCascades = int4(-1);
		lightConstants.perObjectShadows = int4(-1);
		lightConstants.shadowChannel = int4(shadowChannel, -1, -1, -1);
		if (shadowMap)
			lightConstants.outOfBoundsShadow = shadowMap->IsLitOutOfBounds() ? 1.f : 0.f;
		else
			lightConstants.outOfBoundsShadow = 1.f;
	}
}