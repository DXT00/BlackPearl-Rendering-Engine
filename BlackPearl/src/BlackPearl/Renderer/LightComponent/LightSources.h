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
	private:

		std::vector<Object*>m_LightSources;//TODO::×¢ÒâÄÚ´æÐ¹Â©
		unsigned int m_PointLightNums;
	};

}