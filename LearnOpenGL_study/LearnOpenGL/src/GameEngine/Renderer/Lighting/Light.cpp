#include "pch.h"
#include "Light.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "SpotLight.h"
Light * Light::Create(LightType type,const glm::vec3 & position, const glm::vec3& direction, const float cutOffAngle,const float outterCutOffAngle, Props props)
{
	switch (type)
	{
	case LightType::ParallelLight:
		return new ParallelLight(direction, props);
	case LightType::PointLight:
		return new PointLight(position, props);
	case LightType::SpotLight:
		return new SpotLight(position, direction, cutOffAngle, outterCutOffAngle, props);
	default:
		GE_CORE_ERROR("Unknown Light Type!")
		break;
	}
	return nullptr;
}
