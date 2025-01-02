#include "pch.h"
#include "BlackPearl/Core.h"
#include "D3D12DynamicModule.h"
#include "D3D12DynamicRHI.h"

namespace BlackPearl {

	DynamicRHI* D3D12DynamicModule::CreateRHI()
	{
		return DBG_NEW D3D12DynamicRHI();
	}

}
