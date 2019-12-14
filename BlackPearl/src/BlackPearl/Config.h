#pragma once
#include<bitset>
namespace BlackPearl {
	//一个Entity最多32个Component
	class Configuration {
	public:
		static const unsigned int MaxComponents = 32;
		typedef std::bitset<MaxComponents> ComponentMask;

		static const unsigned int WindowWidth = 960;
		static const unsigned int WindowHeight = 540;

		//多重采样
		static const unsigned int MSAA_SAMPLES = 0;

	};


}