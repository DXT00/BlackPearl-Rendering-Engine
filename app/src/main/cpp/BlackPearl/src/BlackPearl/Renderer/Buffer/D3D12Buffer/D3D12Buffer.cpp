#include "pch.h"
#ifdef GE_API_D3D12
#include "D3D12Buffer.h"

namespace BlackPearl {
	//UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, ComPtr<ID3D12DescriptorHeap> pDescriptorHeap, UINT& descriptorsAllocated, UINT descriptorSize, UINT descriptorIndexToUse)
	//{

	//	auto descriptorHeapCpuBase = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//	if (descriptorIndexToUse >= pDescriptorHeap->GetDesc().NumDescriptors)
	//	{
	//		descriptorIndexToUse = descriptorsAllocated++;
	//	}
	//	*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, descriptorSize);
	//	return descriptorIndexToUse;

	//}
}
#endif