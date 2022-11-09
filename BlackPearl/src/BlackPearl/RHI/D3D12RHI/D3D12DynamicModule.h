#pragma once
#include "../DynamicModule.h"
#include "../DynamicRHI.h"
namespace BlackPearl {
	class D3D12DynamicModule :public DynamicModule
	{
		bool IsSupported() override;

		DynamicRHI* CreateRHI() override;
	};

}

