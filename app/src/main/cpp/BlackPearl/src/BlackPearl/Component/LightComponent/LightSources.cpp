#include "pch.h"
#include "Component/LightComponent/LightSources.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/SpotLight.h"
#include "BlackPearl/Config.h"
#include "Component/LightComponent/Light.h"


namespace BlackPearl {

	void LightSources::AddLight(Object* light)
	{
		m_LightSources.push_back(light);
		if (light->HasComponent<PointLight>()) {

		
			m_Lights.push_back(light->GetComponent<PointLight>());
			AddPointLight(light);

		}
		if (light->HasComponent<DirectionLight>()) {
			m_Lights.push_back(light->GetComponent<DirectionLight>());
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