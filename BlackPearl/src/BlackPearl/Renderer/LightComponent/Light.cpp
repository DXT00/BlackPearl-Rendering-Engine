#include "pch.h"
#include "Light.h"
#include "ParallelLight.h"
#include "PointLight.h"
#include "SpotLight.h"
namespace BlackPearl {

	Light * Light::Create(EntityManager * entityManager, Entity::Id id, LightType type, const glm::vec3 & position, const glm::vec3& direction, const float cutOffAngle, const float outterCutOffAngle, Props props)
	{
		
		switch (type)
		{
		case LightType::ParallelLight:
			return DBG_NEW ParallelLight(entityManager,id,props);
		case LightType::PointLight:
			return DBG_NEW PointLight(entityManager, id, props);
		case LightType::SpotLight:
			return DBG_NEW SpotLight(entityManager, id, props);
		default:
			GE_CORE_ERROR("Unknown Light Type!")
				break;
		}
		return nullptr;
	}
}