#include "pch.h"
#include "D3D12DynamicRHI.h"
#include "D3D12Window.h"

namespace BlackPearl {
	Window* D3D12DynamicRHI::InitWindow()
	{
		return new D3D12Window();
	}
}

