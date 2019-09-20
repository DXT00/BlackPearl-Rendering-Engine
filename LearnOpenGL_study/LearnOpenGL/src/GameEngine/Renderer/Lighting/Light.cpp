#include "pch.h"
#include "LightType.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "SpotLight.h"
Light * Light::Create(const glm::vec3 & position, const glm::vec3& direction, const float cutOffAngle,const float outterCutOffAngle, Props props)
{
	switch (LightType::Get())
	{
	case LightType::Type::ParallelLight:
		return new ParallelLight(direction);
	case LightType::Type::PointLight:
		return new PointLight(position);
	case LightType::Type::SpotLight:
		return new SpotLight(position, direction, cutOffAngle, outterCutOffAngle);
	default:
		GE_CORE_ERROR("Unknown Light Type!")
		break;
	}
	return nullptr;
}
