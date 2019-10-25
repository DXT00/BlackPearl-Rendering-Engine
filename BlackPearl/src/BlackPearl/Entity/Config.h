#pragma once
#include<bitset>
namespace BlackPearl {
	//一个Entity最多32个Component
	static const unsigned int MaxComponents = 32;
	typedef std::bitset<MaxComponents> ComponentMask;

}