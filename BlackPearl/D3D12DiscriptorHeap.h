#pragma once
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
using namespace DX;
using Microsoft::WRL::ComPtr;

namespace BlackPearl {

	class D3D12DiscriptorHeap
	{
	public:
		D3D12DiscriptorHeap();
		~D3D12DiscriptorHeap();

		ComPtr<ID3D12DescriptorHeap> Get() { return m_descriptorHeap; }
	private:
		ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
		UINT m_descriptorsAllocated;
		UINT m_descriptorSize;
	};
}


