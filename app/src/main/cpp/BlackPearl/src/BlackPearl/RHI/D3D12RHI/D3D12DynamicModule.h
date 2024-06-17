#pragma once
#ifdef GE_API_D3D12
#include "../DynamicModule.h"
#include "../DynamicRHI.h"
namespace BlackPearl {
	class D3D12DynamicModule :public DynamicModule
	{
		DynamicRHI* CreateRHI() override;
	};

}
#endif
