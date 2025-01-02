#include "pch.h"
#include "LightSources.h"
#include "PointLight.h"
#include "ParallelLight.h"
#include "SpotLight.h"
#include "BlackPearl/Config.h"
#include"Light.h"


namespace BlackPearl {

	void LightSources::AddLight(Object* light)
	{
		m_LightSources.push_back(light);
		if (light->HasComponent<PointLight>()) {

			GE_ASSERT(m_PointLightNums <= Configuration::MaxComponents, "m_PointLightNums > Configuration::MaxComponents !");
			m_PointLightNums++;
			m_Lights.push_back(light->GetComponent<PointLight>());
			AddPointLight(light);

		}
		if (light->HasComponent<ParallelLight>()) {
			m_Lights.push_back(light->GetComponent<ParallelLight>());
			AddParallelLight(light);
		}

		if (light->HasComponent<SpotLight>()) {
			m_Lights.push_back(light->GetComponent<SpotLight>());
			AddSpotLight(light);

		}
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