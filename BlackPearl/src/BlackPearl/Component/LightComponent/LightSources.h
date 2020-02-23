#pragma once
#include"Light.h"
#include "BlackPearl/Object/Object.h"
namespace BlackPearl {

	class LightSources
	{
	public:
		LightSources()
			:m_PointLightNums(0) {};
		~LightSources() {
			m_LightSources.clear();
		
		};
		void AddLight(Object* light);
		inline unsigned int const GetPointLightNum()const { return m_PointLightNums; }
		inline std::vector<Object*> Get()const { return m_LightSources; }
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
							 

		unsigned int m_PointLightNums;


	};

}