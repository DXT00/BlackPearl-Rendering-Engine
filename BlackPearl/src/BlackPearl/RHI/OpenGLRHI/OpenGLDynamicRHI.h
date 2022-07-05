#pragma once
#include "../DynamicRHI.h"
#include "BlackPearl/Window.h"
namespace BlackPearl {
	class OpenGLDynamicRHI : public DynamicRHI
	{
	public:
		Window* InitWindow();

	};
}


