#pragma once
#include "BlackPearl/Timestep/Timestep.h"
namespace BlackPearl {
	
	class BaseSystem
	{
	public:
		typedef size_t Family;
		BaseSystem();
		virtual ~BaseSystem() = default;

		virtual void Configure() = 0;

		virtual void Update(Timestep ts) = 0;


		//static Family s_FamliyCounter;
		


	};

	//BaseSystem::Family BaseSystem::s_FamliyCounter = 0;

}


