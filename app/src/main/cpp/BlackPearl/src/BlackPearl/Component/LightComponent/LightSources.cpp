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
			AddPointLight(light);
		}
		if (light->HasComponent<DirectionLight>()) {
			AddParallelLight(light);
		}

		if (light->HasComponent<SpotLight>()) {
			AddSpotLight(light);
		}
	}
	void LightSources::AddPointLight(Object* pointLight)
	{
		m_PontLights.push_back(pointLight);
        m_Lights.push_back(pointLight->GetComponent<PointLight>());

	}
	void LightSources::AddParallelLight(Object* parallelLight)
	{
		m_ParallelLights.push_back(parallelLight);
        m_Lights.push_back(parallelLight->GetComponent<DirectionLight>());

	}
	void LightSources::AddSpotLight(Object* spotLight)
	{
		m_SpotLights.push_back(spotLight);
        m_Lights.push_back(spotLight->GetComponent<SpotLight>());

	}
}