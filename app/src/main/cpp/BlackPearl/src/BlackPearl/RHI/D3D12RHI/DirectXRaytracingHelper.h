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
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#include <sstream>
#include <iostream>
#include <iomanip>
#include <DirectXMath.h>
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/GpuUploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace BlackPearl {
#define SizeOfInUint32(obj) ((sizeof(obj) - 1) / sizeof(UINT32) + 1)

    // Ray types traced in this sample.
    enum RayType {
        Radiance = 0,   // ~ Primary, reflected camera/view rays calculating color for each hit.
        Shadow,         // ~ Shadow/visibility rays, only testing for occlusion
        Count
    };

    struct AccelerationStructureBuffers
    {
        ComPtr<ID3D12Resource> scratch;
        ComPtr<ID3D12Resource> accelerationStructure;
        ComPtr<ID3D12Resource> instanceDesc;    // Used only for top-level AS
        UINT64                 ResultDataMaxSizeInBytes;
    };

    struct RayPayload
    {
        XMFLOAT4 color;
        UINT   recursionDepth;
    };

    struct ShadowRayPayload
    {
        bool hit;
    };



    template<class T, size_t N>
    void DefineExports(T* obj, LPCWSTR(&Exports)[N])
    {
        for (UINT i = 0; i < N; i++)
        {
            obj->DefineExport(Exports[i]);
        }
    }

    template<class T, size_t N, size_t M>
    void DefineExports(T* obj, LPCWSTR(&Exports)[N][M])
    {
        for (UINT i = 0; i < N; i++)
            for (UINT j = 0; j < M; j++)
            {
                obj->DefineExport(Exports[i][j]);
            }
    }

    // Resets all elements in a ComPtr array.
    template<class T>
    void ResetComPtrArray(T* comPtrArray)
    {
        for (auto& i : *comPtrArray)
        {
            i.Reset();
        }
    }
    // Resets all elements in a ComPtr vector.

    template<class T>
    void ResetComPtrVector(std::vector<T> comPtrVector)
    {
        for (auto& i : comPtrVector)
        {
            i.Reset();
        }
    }
    //inline void AllocateUploadBuffer(ID3D12Device* pDevice, void* pData, UINT64 datasize, ID3D12Resource** ppResource, const wchar_t* resourceName = nullptr)
    //{
    //    auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    //    auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(datasize);
    //    ThrowIfFailed(pDevice->CreateCommittedResource(
    //        &uploadHeapProperties,
    //        D3D12_HEAP_FLAG_NONE,
    //        &bufferDesc,
    //        D3D12_RESOURCE_STATE_GENERIC_READ,
    //        nullptr,
    //        IID_PPV_ARGS(ppResource)));
    //    if (resourceName)
    //    {
    //        (*ppResource)->SetName(resourceName);
    //    }
    //    void* pMappedData;
    //    (*ppResource)->Map(0, nullptr, &pMappedData);
    //    memcpy(pMappedData, pData, datasize);
    //    (*ppResource)->Unmap(0, nullptr);
    //}

    // Pretty-print a state object tree.
    inline void PrintStateObjectDesc(const D3D12_STATE_OBJECT_DESC* desc)
    {
        std::wstringstream wstr;
        wstr << L"\n";
        wstr << L"--------------------------------------------------------------------\n";
        wstr << L"| D3D12 State Object 0x" << static_cast<const void*>(desc) << L": ";
        if (desc->Type == D3D12_STATE_OBJECT_TYPE_COLLECTION) wstr << L"Collection\n";
        if (desc->Type == D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE) wstr << L"Raytracing Pipeline\n";

        auto ExportTree = [](UINT depth, UINT numExports, const D3D12_EXPORT_DESC* exports)
        {
            std::wostringstream woss;
            for (UINT i = 0; i < numExports; i++)
            {
                woss << L"|";
                if (depth > 0)
                {
                    for (UINT j = 0; j < 2 * depth - 1; j++) woss << L" ";
                }
                woss << L" [" << i << L"]: ";
                if (exports[i].ExportToRename) woss << exports[i].ExportToRename << L" --> ";
                woss << exports[i].Name << L"\n";
            }
            return woss.str();
        };

        for (UINT i = 0; i < desc->NumSubobjects; i++)
        {
            wstr << L"| [" << i << L"]: ";
            switch (desc->pSubobjects[i].Type)
            {
            case D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE:
                wstr << L"Global Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
                break;
            case D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE:
                wstr << L"Local Root Signature 0x" << desc->pSubobjects[i].pDesc << L"\n";
                break;
            case D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK:
                wstr << L"Node Mask: 0x" << std::hex << std::setfill(L'0') << std::setw(8) << *static_cast<const UINT*>(desc->pSubobjects[i].pDesc) << std::setw(0) << std::dec << L"\n";
                break;
            case D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY:
            {
                wstr << L"DXIL Library 0x";
                auto lib = static_cast<const D3D12_DXIL_LIBRARY_DESC*>(desc->pSubobjects[i].pDesc);
                wstr << lib->DXILLibrary.pShaderBytecode << L", " << lib->DXILLibrary.BytecodeLength << L" bytes\n";
                wstr << ExportTree(1, lib->NumExports, lib->pExports);
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_EXISTING_COLLECTION:
            {
                wstr << L"Existing Library 0x";
                auto collection = static_cast<const D3D12_EXISTING_COLLECTION_DESC*>(desc->pSubobjects[i].pDesc);
                wstr << collection->pExistingCollection << L"\n";
                wstr << ExportTree(1, collection->NumExports, collection->pExports);
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
            {
                wstr << L"Subobject to Exports Association (Subobject [";
                auto association = static_cast<const D3D12_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
                UINT index = static_cast<UINT>(association->pSubobjectToAssociate - desc->pSubobjects);
                wstr << index << L"])\n";
                for (UINT j = 0; j < association->NumExports; j++)
                {
                    wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
                }
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION:
            {
                wstr << L"DXIL Subobjects to Exports Association (";
                auto association = static_cast<const D3D12_DXIL_SUBOBJECT_TO_EXPORTS_ASSOCIATION*>(desc->pSubobjects[i].pDesc);
                wstr << association->SubobjectToAssociate << L")\n";
                for (UINT j = 0; j < association->NumExports; j++)
                {
                    wstr << L"|  [" << j << L"]: " << association->pExports[j] << L"\n";
                }
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG:
            {
                wstr << L"Raytracing Shader Config\n";
                auto config = static_cast<const D3D12_RAYTRACING_SHADER_CONFIG*>(desc->pSubobjects[i].pDesc);
                wstr << L"|  [0]: Max Payload Size: " << config->MaxPayloadSizeInBytes << L" bytes\n";
                wstr << L"|  [1]: Max Attribute Size: " << config->MaxAttributeSizeInBytes << L" bytes\n";
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG:
            {
                wstr << L"Raytracing Pipeline Config\n";
                auto config = static_cast<const D3D12_RAYTRACING_PIPELINE_CONFIG*>(desc->pSubobjects[i].pDesc);
                wstr << L"|  [0]: Max Recursion Depth: " << config->MaxTraceRecursionDepth << L"\n";
                break;
            }
            case D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP:
            {
                wstr << L"Hit Group (";
                auto hitGroup = static_cast<const D3D12_HIT_GROUP_DESC*>(desc->pSubobjects[i].pDesc);
                wstr << (hitGroup->HitGroupExport ? hitGroup->HitGroupExport : L"[none]") << L")\n";
                wstr << L"|  [0]: Any Hit Import: " << (hitGroup->AnyHitShaderImport ? hitGroup->AnyHitShaderImport : L"[none]") << L"\n";
                wstr << L"|  [1]: Closest Hit Import: " << (hitGroup->ClosestHitShaderImport ? hitGroup->ClosestHitShaderImport : L"[none]") << L"\n";
                wstr << L"|  [2]: Intersection Import: " << (hitGroup->IntersectionShaderImport ? hitGroup->IntersectionShaderImport : L"[none]") << L"\n";
                break;
            }
            }
            wstr << L"|--------------------------------------------------------------------\n";
        }
        wstr << L"\n";
        OutputDebugStringW(wstr.str().c_str());
    }

    // Returns bool whether the device supports DirectX Raytracing tier.
    inline bool IsDirectXRaytracingSupported(IDXGIAdapter1* adapter)
    {
        ComPtr<ID3D12Device> testDevice;
        D3D12_FEATURE_DATA_D3D12_OPTIONS5 featureSupportData = {};

        return SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&testDevice)))
            && SUCCEEDED(testDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &featureSupportData, sizeof(featureSupportData)))
            && featureSupportData.RaytracingTier != D3D12_RAYTRACING_TIER_NOT_SUPPORTED;
    }
}
