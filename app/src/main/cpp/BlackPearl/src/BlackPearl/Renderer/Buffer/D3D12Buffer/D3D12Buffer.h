//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#ifdef GE_API_D3D12
#include <wrl/client.h>
#include <d3d12.h>
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "glm/glm.hpp"
#include "BlackPearl/RHI/RHIBuffer.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include <DirectXMath.h>
// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.

using namespace DirectX;
using Microsoft::WRL::ComPtr;

namespace BlackPearl {

	//------------------------ConstantBuffer-----------------//

	// Helper class to create and update a constant buffer with proper constant buffer alignments.
	// Usage: 
	//    ConstantBuffer<...> cb;
	//    cb.Create(...);
	//    cb.staging.var = ... ; | cb->var = ... ; 
	//    cb.CopyStagingToGPU(...);
	//    Set...View(..., cb.GputVirtualAddress());
	
	
	//------------------------HLSL Buffer-----------------//
	struct Viewport
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	struct RayGenConstantBuffer
	{
		Viewport viewport;
		Viewport stencil;
	};
	struct CubeConstantBuffer {
		XMFLOAT4 albedo;
	};

	struct SceneConstantBuffer
	{
		
		XMMATRIX projectionToWorld;
		XMVECTOR cameraPosition;
		XMVECTOR lightPosition;
		XMVECTOR lightAmbientColor;
		XMVECTOR lightDiffuseColor;

		float    reflectance;
		float    elapsedTime;                 // Elapsed application time.
	
	};

	// Attributes per primitive type.
	struct PrimitiveConstantBuffer
	{
		XMFLOAT4 albedo;
		float reflectanceCoef;
		float diffuseCoef;
		float specularCoef;
		float specularPower;
		float stepScale;                      // Step scale for ray marching of signed distance primitives. 
											  // - Some object transformations don't preserve the distances and 
											  //   thus require shorter steps.
		XMFLOAT3 padding;
	};

	// Attributes per primitive instance.
	struct PrimitiveInstanceConstantBuffer
	{
		UINT instanceIndex;
		UINT primitiveType; // Procedural primitive type
	};

	// Geometry //for indexBuffer or vertexBuffer
	struct D3DBuffer
	{
		ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;
	};


	//class D3D12Buffer : public RHIBuffer
	//{
	//	D3D12Buffer()
	//		: RHIBuffer(0, EBufferUsageFlags::None, 0)
	//		, LockedData(nullptr)
	//	{
	//	}
	//	D3D12Buffer(D3D12Device* InParent, uint32_t InSize, EBufferUsageFlags InUsage, uint32_t InStride)
	//		: RHIBuffer(InSize, InUsage, InStride)
	//		, D3D12BaseShaderResource(InParent)
	//		, LockedData(InParent)
	//	{
	//	}
	//};



	//// Allocate a descriptor and return its index. 
	//// If the passed descriptorIndexToUse is valid, it will be used instead of allocating a new one.
	//UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, ComPtr<ID3D12DescriptorHeap> pDescriptorHeap, UINT& descriptorsAllocated, UINT descriptorSize, UINT descriptorIndexToUse = UINT_MAX);

	inline UINT AllocateDescriptor(
		D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor,
		ComPtr<ID3D12DescriptorHeap> pDescriptorHeap,
		UINT& descriptorAllocated,
		UINT& descriptorSize,
		UINT descriptorIndexToUse = UINT_MAX) 
	{
		auto descriptorHeapCpuBase = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (descriptorIndexToUse >= pDescriptorHeap->GetDesc().NumDescriptors)
		{
			ThrowIfFalse(descriptorAllocated < pDescriptorHeap->GetDesc().NumDescriptors, L"Ran out of descriptors on the heap!");
			descriptorIndexToUse = descriptorAllocated++;
		}
		*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, descriptorSize);
		return descriptorIndexToUse;

	}

	//// Create SRV for a buffer.
	//// TODO::��װ descritorHeap
	//// ����Դ��������pDescriptorHeap �ϴ���SRV��Դ������
	inline UINT CreateBufferSRV(
		ID3D12Device* pDevice, 
		ComPtr<ID3D12DescriptorHeap> pDescriptorHeap,
		UINT& descriptorAllocated,
		UINT& descriptorSize,
		D3DBuffer* buffer,
		UINT numElements, 
		UINT elementSize) {
		// SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER; //����SRV������
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Buffer.NumElements = numElements;
		if (elementSize == 0)
		{
			srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
			srvDesc.Buffer.StructureByteStride = 0;
		}
		else
		{
			srvDesc.Format = DXGI_FORMAT_UNKNOWN;
			srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
			srvDesc.Buffer.StructureByteStride = elementSize;
		}
		UINT descriptorIndex = AllocateDescriptor(&buffer->cpuDescriptorHandle, pDescriptorHeap, descriptorAllocated, descriptorSize);
		pDevice->CreateShaderResourceView(buffer->resource.Get(), &srvDesc, buffer->cpuDescriptorHandle);
		buffer->gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(pDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, descriptorSize);
		return descriptorIndex;
	}

	inline void AllocateUAVBuffer(ID3D12Device* pDevice, UINT64 bufferSize, ID3D12Resource** ppResource, D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_COMMON, const wchar_t* resourceName = nullptr)
	{
		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			initialResourceState,
			nullptr,
			IID_PPV_ARGS(ppResource)));
		if (resourceName)
		{
			(*ppResource)->SetName(resourceName);
		}
	}

	inline void AllocateUploadBuffer(ID3D12Device* pDevice, void* pData, UINT64 datasize, ID3D12Resource** ppResource, const wchar_t* resourceName = nullptr)
	{
		auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
		ThrowIfFailed(pDevice->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&bufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(ppResource)));
		if (resourceName)
		{
			(*ppResource)->SetName(resourceName);
		}
		void* pMappedData;
		(*ppResource)->Map(0, nullptr, &pMappedData);
		memcpy(pMappedData, pData, datasize);
		(*ppResource)->Unmap(0, nullptr);
	}

}
#endif

