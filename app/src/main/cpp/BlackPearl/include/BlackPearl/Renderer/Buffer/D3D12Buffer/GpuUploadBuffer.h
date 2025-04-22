#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include "BlackPearl/Common/CommonFunc.h"

using Microsoft::WRL::ComPtr;

namespace BlackPearl {
	class GpuUploadBuffer
	{
	public:
		ComPtr<ID3D12Resource> GetResource() { return m_resource; }
		virtual void Release() { m_resource.Reset(); }
	protected:
		ComPtr<ID3D12Resource> m_resource;
		GpuUploadBuffer() {}
		~GpuUploadBuffer() {
			if (m_resource.Get())
			{
				m_resource->Unmap(0, nullptr);
			}
		}
		void Allocate(ID3D12Device* device, UINT bufferSize, LPCWSTR resourceName = nullptr)
		{
			auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

			auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
			ThrowIfFailed(device->CreateCommittedResource(
				&uploadHeapProperties,
				D3D12_HEAP_FLAG_NONE,
				&bufferDesc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&m_resource)));
			m_resource->SetName(resourceName);
		}

		uint8_t* MapCpuWriteOnly()
		{
			uint8_t* mappedData;
			// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
			CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
			ThrowIfFailed(m_resource->Map(0, &readRange, reinterpret_cast<void**>(&mappedData)));
			return mappedData;
		}
	};
}


