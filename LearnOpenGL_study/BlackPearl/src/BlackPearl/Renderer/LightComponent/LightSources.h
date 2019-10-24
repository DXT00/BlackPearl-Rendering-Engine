#pragma once
#include"Light.h"
namespace BlackPearl {

	class LightSources
	{
	public:
		LightSources()
			:m_PointLightNums(0) {};
		~LightSources() = default;
		void AddLight(const std::shared_ptr<Light>&light);
		inline int const GetPointLightNum()const { return m_PointLightNums; }
		inline std::vector<std::shared_ptr<Light>> Get()const { return m_LightSources; }
	private:

		std::vector<std::shared_ptr<Light>>m_LightSources;
		int m_PointLightNums;
	};

}