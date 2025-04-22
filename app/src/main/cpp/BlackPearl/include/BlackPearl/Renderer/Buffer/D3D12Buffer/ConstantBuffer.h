#pragma once
#include "GpuUploadBuffer.h"

namespace BlackPearl {
	template <class T>
	class ConstantBuffer : public GpuUploadBuffer
	{
		uint8_t* m_mappedConstantData;
		UINT m_alignedInstanceSize;
		UINT m_numInstances;

	public:
		ConstantBuffer() : m_alignedInstanceSize(0), m_numInstances(0), m_mappedConstantData(nullptr) {}

		void Create(ID3D12Device* device, UINT numInstances = 1, LPCWSTR resourceName = nullptr)
		{
			m_numInstances = numInstances;
			m_alignedInstanceSize = Align(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			UINT bufferSize = numInstances * m_alignedInstanceSize;
			Allocate(device, bufferSize, resourceName);
			m_mappedConstantData = MapCpuWriteOnly();
		}

		void CopyStagingToGpu(UINT instanceIndex = 0)
		{
			memcpy(m_mappedConstantData + instanceIndex * m_alignedInstanceSize, &staging, sizeof(T));
		}

		// Accessors
		T staging;
		T* operator->() { return &staging; }
		UINT NumInstances() { return m_numInstances; }
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress(UINT instanceIndex = 0)
		{
			return m_resource->GetGPUVirtualAddress() + instanceIndex * m_alignedInstanceSize;
		}
	};

}