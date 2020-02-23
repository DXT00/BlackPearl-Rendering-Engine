#include "pch.h"
#include "LightSources.h"
#include "PointLight.h"
#include "ParallelLight.h"
#include "SpotLight.h"
namespace BlackPearl {

	void LightSources::AddLight(Object* light)
	{
		m_LightSources.push_back(light);
		if (light->HasComponent<PointLight>()) {
			m_PointLightNums++;
			AddPointLight(light);
		}
		if (light->HasComponent<ParallelLight>())
			AddParallelLight(light);

		if (light->HasComponent<SpotLight>())
			AddSpotLight(light);
	}
	void LightSources::AddPointLight(Object* pointLight)
	{
		m_PontLights.push_back(pointLight);
	}
	void LightSources::AddParallelLight(Object* parallelLight)
	{
		m_ParallelLights.push_back(parallelLight);
	}
	void LightSources::AddSpotLight(Object* spotLight)
	{
		m_SpotLights.push_back(spotLight);
	}
}