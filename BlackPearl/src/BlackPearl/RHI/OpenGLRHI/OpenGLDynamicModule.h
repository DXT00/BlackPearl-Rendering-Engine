#pragma once
#include "../DynamicModule.h"
namespace BlackPearl {
	class OpenGLDynamicModule : public DynamicModule
	{
		DynamicRHI* CreateRHI() override;
	};

}

