#pragma once
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {
	class Light;
	class LightSources
	{
	public:
        LightSources() {}
		~LightSources() {
			m_LightSources.clear();
		
		};
		void AddLight(Object* light);
        inline unsigned int const GetPointLightNum()const { return m_PontLights.size(); }
        inline unsigned int const GetParallelLightNum()const { return m_ParallelLights.size(); }
        inline unsigned int const GetSpotLightNum()const { return m_SpotLights.size(); }
        inline unsigned int const GetLightsNum()const { return m_Lights.size(); }

        inline std::vector<Object*> Get()const { return m_LightSources; }
		std::vector<Light*> GetLights()const { return m_Lights; }

		std::vector<Object*> GetPointLights() const { return m_PontLights; }
		std::vector<Object*> GetParallelLights() const { return m_ParallelLights; }
		std::vector<Object*> GetSpotLights() const { return m_SpotLights; }

	private:
		void AddPointLight(Object* pointLight);
		void AddParallelLight(Object* parallelLight);
		void AddSpotLight(Object* spotLight);
		/*所有类型的Light*/
		std::vector<Object*> m_LightSources;
		std::vector<Object*> m_PontLights;
		std::vector<Object*> m_ParallelLights;
		std::vector<Object*> m_SpotLights;
		std::vector<Light*>  m_Lights;




	};

}