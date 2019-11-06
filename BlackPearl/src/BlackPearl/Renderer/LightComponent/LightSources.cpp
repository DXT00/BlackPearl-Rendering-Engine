#include "pch.h"
#include "LightSources.h"
#include "PointLight.h"
namespace BlackPearl {

	void LightSources::AddLight(Object* light)
	{
		m_LightSources.push_back(light);
		if (light->HasComponent<PointLight>())
			m_PointLightNums++;
	}
}