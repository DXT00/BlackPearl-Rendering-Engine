#pragma once
#include "../DynamicModule.h"
namespace BlackPearl {
	class OpenGLDynamicModule : public DynamicModule
	{
		bool IsSupported() override;

		DynamicRHI* CreateRHI() override;
	};

}

