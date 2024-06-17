#include "pch.h"
#include "Light.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "SpotLight.h"
namespace BlackPearl {

	Light * Light::Create( LightType type, const glm::vec3 & position, const glm::vec3& direction, const float cutOffAngle, const float outterCutOffAngle, Props props)
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
}