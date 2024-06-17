#pragma once
#ifdef GE_API_D3D12
#include "GpuUploadBuffer.h"
#include "BlackPearl/Core.h"
namespace BlackPearl {
	//-----------------------------StructuredBuffer---------------------------//
	template <class T>
	class StructuredBuffer : public GpuUploadBuffer
	{

	public:
		StructuredBuffer() :
			m_mappedBuffers(nullptr), m_numInstances(0) {
			int sizeOfT = sizeof(T);
			GE_ASSERT(sizeOfT % 16 == 0, "Align structure buffers on 16 byte boundary for performance reasons.");
		}
		void Create(ID3D12Device* device, UINT numElements, UINT frameCount = 1, LPCWSTR resourceName = nullptr)
		{
			m_staging.resize(numElements);
			UINT bufferSize = frameCount * numElements * sizeof(T);
			Allocate(device, bufferSize, resourceName);
			m_mappedBuffers = reinterpret_cast<T*>(MapCpuWriteOnly());
		}

		T& operator[](UINT elementIndex) { return m_staging[elementIndex]; }

		size_t InstanceSize() { return m_staging.size() * sizeof(T); }
		void CopyStagingToGpu(UINT instanceIndex = 0)
		{
			memcpy(m_mappedBuffers + instanceIndex * m_staging.size(), &m_staging[0], m_staging.size() * sizeof(T));
		}
		D3D12_GPU_VIRTUAL_ADDRESS GpuVirtualAddress(UINT instanceIndex = 0)
		{
			return m_resource->GetGPUVirtualAddress() + instanceIndex * InstanceSize();
		}

	private:
		T* m_mappedBuffers;
		std::vector<T> m_staging;
		UINT m_numInstances;
	};

}
#endif