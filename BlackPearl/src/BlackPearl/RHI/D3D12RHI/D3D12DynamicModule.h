#pragma once
#include "../DynamicModule.h"
#include "../DynamicRHI.h"
namespace BlackPearl {
	class D3D12DynamicModule :public DynamicModule
	{
		DynamicRHI* CreateRHI() override;
	};

}

