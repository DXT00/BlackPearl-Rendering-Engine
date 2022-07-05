#pragma once
#include "../DynamicRHI.h"
#include "BlackPearl/Window.h"
namespace BlackPearl {
	class D3D12DynamicRHI :public DynamicRHI
	{
	public:
		Window* InitWindow();
	};
}


