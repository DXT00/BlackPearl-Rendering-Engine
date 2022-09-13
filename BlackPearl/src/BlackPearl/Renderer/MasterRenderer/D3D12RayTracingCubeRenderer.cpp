#include "pch.h"
#include "D3D12RayTracingCubeRenderer.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/Renderer/Shader/D3D12Shader/ShaderTable.h"
#include "RaytracingCube.hlsl.h"
#include <DirectXMath.h>
using namespace DirectX;

namespace BlackPearl {
	const wchar_t* D3D12RayTracingCubeRenderer::c_hitGroupName = L"MyHitGroup";
	const wchar_t* D3D12RayTracingCubeRenderer::c_raygenShaderName = L"MyRaygenShader";
	const wchar_t* D3D12RayTracingCubeRenderer::c_closestHitShaderName = L"MyClosestHitShader";
	const wchar_t* D3D12RayTracingCubeRenderer::c_missShaderName = L"MyMissShader";
	D3D12RayTracingCubeRenderer::D3D12RayTracingCubeRenderer():
		D3D12Renderer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight()),
		m_adapterIDoverride(UINT_MAX),
		m_raytracingOutputResourceUAVDescriptorHeapIndex(UINT_MAX)
	{
		Init();
	}

	D3D12RayTracingCubeRenderer::~D3D12RayTracingCubeRenderer()
	{
	}

	// Release all device dependent resouces when a device is lost.
	void D3D12RayTracingCubeRenderer::OnDeviceLost()
	{
		ReleaseWindowSizeDependentResources();
		ReleaseDeviceDependentResources();
	}
	// Create all device dependent resources when a device is restored.

	void D3D12RayTracingCubeRenderer::OnDeviceRestored()
	{
		CreateDeviceDependentResources();
		CreateWindowSizeDependentResources();
	}

	void D3D12RayTracingCubeRenderer::Init()
	{
        m_deviceResources = std::make_unique<DeviceResources>(
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT_UNKNOWN,
            m_FrameCount,
            D3D_FEATURE_LEVEL_11_0,
            // Sample shows handling of use cases with tearing support, which is OS dependent and has been supported since TH2.
            // Since the sample requires build 1809 (RS5) or higher, we don't need to handle non-tearing cases.
            DeviceResources::c_RequireTearingSupport,
            m_adapterIDoverride
            );
        m_deviceResources->RegisterDeviceNotify(this);
		m_deviceResources->SetWindow(
			(HWND)(Application::Get().GetWindow().GetNativeWindow()),
			m_Width,
			m_Height);       
		m_deviceResources->InitializeDXGIAdapter();

        ThrowIfFalse(IsDirectXRaytracingSupported(m_deviceResources->GetAdapter()),
            L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");

        m_deviceResources->CreateDeviceResources();
        m_deviceResources->CreateWindowSizeDependentResources();

       InitializeScene();

        CreateDeviceDependentResources();
        CreateWindowSizeDependentResources();
	}

	void D3D12RayTracingCubeRenderer::InitializeScene()
	{
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		// Setup materials.
		{
			m_cubeCB.albedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		// Setup camera.
		{
			// Initialize the view and projection inverse matrices.
			m_rotate = 10.0f;
			m_eye = { 0.0f, 2.0f, -5.0f, 1.0f };
			m_at = { 0.0f, 0.0f, 0.0f, 1.0f };
			XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

			XMVECTOR direction = XMVector4Normalize(m_at - m_eye);
			m_up = XMVector3Normalize(XMVector3Cross(direction, right));

			// Rotate camera around Y axis.
			XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(45.0f));
			m_eye = XMVector3Transform(m_eye, rotate);
			m_up = XMVector3Transform(m_up, rotate);

			UpdateCameraMatrices();
		}

		// Setup lights.
		{

			XMFLOAT4 lightPosition;
			XMFLOAT4 lightAmbientColor;
			XMFLOAT4 lightDiffuseColor;

			lightPosition = XMFLOAT4(0.0f, 1.8f, -3.0f, 0.0f);
			m_sceneCB[frameIndex].lightPosition = XMLoadFloat4(&lightPosition);

			lightAmbientColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
			m_sceneCB[frameIndex].lightAmbientColor = XMLoadFloat4(&lightAmbientColor);

			lightDiffuseColor = XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f);
			m_sceneCB[frameIndex].lightDiffuseColor = XMLoadFloat4(&lightDiffuseColor);
		}

		// Apply the initial values to all frames' buffer instances.
		for (auto& sceneCB : m_sceneCB)
		{
			sceneCB = m_sceneCB[frameIndex];
		}
	}

	void D3D12RayTracingCubeRenderer::CreateConstantBuffers()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto frameCount = m_deviceResources->GetBackBufferCount();

		// Create the constant buffer memory and map the CPU and GPU addresses
		const D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		// Allocate one constant buffer per frame, since it gets updated every frame.
		size_t cbSize = frameCount * sizeof(AlignedSceneConstantBuffer);
		const D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(cbSize);

		ThrowIfFailed(device->CreateCommittedResource(
			&uploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_perFrameConstants)));

		// Map the constant buffer and cache its heap pointers.
		// We don't unmap this until the app closes. Keeping buffer mapped for the lifetime of the resource is okay.
		CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
		ThrowIfFailed(m_perFrameConstants->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedConstantData)));
	}

	void D3D12RayTracingCubeRenderer::CreateDeviceDependentResources()
	{
		// Initialize raytracing pipeline.

//		Create raytracing interfaces: raytracing device and commandlist.
		CreateRaytracingInterfaces();

		// Create root signatures for the shaders.
		CreateRootSignatures();

		// Create a raytracing pipeline state object which defines the binding of shaders, state and resources to be used during raytracing.
		CreateRaytracingPipelineStateObject();

		// Create a heap for descriptors.
		CreateDescriptorHeap();

		// Build geometry to be used in the sample.
		BuildGeometry();

		// Build raytracing acceleration structures from the generated geometry.
		BuildAccelerationStructures();

		// Create constant buffers for the geometry and the scene.
		CreateConstantBuffers();

		// Build shader tables, which define shaders and their local root arguments.
		BuildShaderTables();

		// Create an output 2D texture to store the raytracing result to.
		CreateRaytracingOutputResource();
	}

	void D3D12RayTracingCubeRenderer::CreateWindowSizeDependentResources()
	{
		CreateRaytracingOutputResource();
		UpdateCameraMatrices();
	}

	void D3D12RayTracingCubeRenderer::ReleaseDeviceDependentResources()
	{
	}

	void D3D12RayTracingCubeRenderer::ReleaseWindowSizeDependentResources()
	{
	}

	void D3D12RayTracingCubeRenderer::DoRaytracing()
	{
		auto commandList = m_deviceResources->GetCommandList();
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		auto DispatchRays = [&](auto* commandList, auto* stateObject, auto* dispatchDesc)
		{
			// Since each shader table has only one shader record, the stride is same as the size.
			dispatchDesc->HitGroupTable.StartAddress = m_hitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_hitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = dispatchDesc->HitGroupTable.SizeInBytes;
			dispatchDesc->MissShaderTable.StartAddress = m_missShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_missShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = dispatchDesc->MissShaderTable.SizeInBytes;
			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_rayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_rayGenShaderTable->GetDesc().Width;
			dispatchDesc->Width = m_Width;
			dispatchDesc->Height = m_Height;
			dispatchDesc->Depth = 1;
			commandList->SetPipelineState1(stateObject);
			commandList->DispatchRays(dispatchDesc);
		};

		auto SetCommonPipelineState = [&](auto* descriptorSetCommandList)
		{
			descriptorSetCommandList->SetDescriptorHeaps(1, m_descriptorHeap.GetAddressOf());
			// Set index and successive vertex buffer decriptor tables
			commandList->SetComputeRootDescriptorTable((UINT)GlobalRootSignatureParams::VertexBuffersSlot, m_indexBuffer.gpuDescriptorHandle);
			commandList->SetComputeRootDescriptorTable((UINT)GlobalRootSignatureParams::OutputViewSlot, m_raytracingOutputResourceUAVGpuDescriptor);
		};

		commandList->SetComputeRootSignature(m_raytracingGlobalRootSignature.Get());

		// Copy the updated scene constant buffer to GPU.
		memcpy(&m_mappedConstantData[frameIndex].constants, &m_sceneCB[frameIndex], sizeof(m_sceneCB[frameIndex]));
		auto cbGpuAddress = m_perFrameConstants->GetGPUVirtualAddress() + frameIndex * sizeof(m_mappedConstantData[0]);
		commandList->SetComputeRootConstantBufferView((UINT)GlobalRootSignatureParams::SceneConstantSlot, cbGpuAddress);

		// Bind the heaps, acceleration structure and dispatch rays.
		D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
		SetCommonPipelineState(commandList);
		commandList->SetComputeRootShaderResourceView((UINT)GlobalRootSignatureParams::AccelerationStructureSlot, m_topLevelAccelerationStructure->GetGPUVirtualAddress());
		DispatchRays(m_dxrCommandList.Get(), m_dxrStateObject.Get(), &dispatchDesc);
	}

	void D3D12RayTracingCubeRenderer::CreateRaytracingInterfaces()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();

		ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&m_dxrDevice)), L"Couldn't get DirectX Raytracing interface for the device.\n");
		ThrowIfFailed(commandList->QueryInterface(IID_PPV_ARGS(&m_dxrCommandList)), L"Couldn't get DirectX Raytracing interface for the command list.\n");
	}

	void D3D12RayTracingCubeRenderer::SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig)
	{
		auto device = m_deviceResources->GetD3DDevice();
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;

		ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
		ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));
	}

	void D3D12RayTracingCubeRenderer::CreateRootSignatures()
	{
		auto device = m_deviceResources->GetD3DDevice();

		// Global Root Signature
		// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		{
			CD3DX12_DESCRIPTOR_RANGE ranges[2]; // Perfomance TIP: Order from most frequent to least frequent.
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);  // 1 output texture
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 1);  // 2 static index and vertex buffers.

			CD3DX12_ROOT_PARAMETER rootParameters[(UINT)GlobalRootSignatureParams::Count];
			rootParameters[(UINT)GlobalRootSignatureParams::OutputViewSlot].InitAsDescriptorTable(1, &ranges[0]);
			rootParameters[(UINT)GlobalRootSignatureParams::AccelerationStructureSlot].InitAsShaderResourceView(0);
			rootParameters[(UINT)GlobalRootSignatureParams::SceneConstantSlot].InitAsConstantBufferView(0);
			rootParameters[(UINT)GlobalRootSignatureParams::VertexBuffersSlot].InitAsDescriptorTable(1, &ranges[1]);
			CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			SerializeAndCreateRaytracingRootSignature(globalRootSignatureDesc, &m_raytracingGlobalRootSignature);
		}

		// Local Root Signature
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.
		{
			CD3DX12_ROOT_PARAMETER rootParameters[(UINT)LocalRootSignatureParams::Count];
			rootParameters[(UINT)LocalRootSignatureParams::CubeConstantSlot].InitAsConstants(SizeOfInUint32(m_cubeCB), 1);
			CD3DX12_ROOT_SIGNATURE_DESC localRootSignatureDesc(ARRAYSIZE(rootParameters), rootParameters);
			localRootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
			SerializeAndCreateRaytracingRootSignature(localRootSignatureDesc, &m_raytracingLocalRootSignature);
		}
	}

	void D3D12RayTracingCubeRenderer::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		 // Ray gen and miss shaders in this sample are not using a local root signature and thus one is not associated with them.

		// Local root signature to be used in a hit group.
		auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
		localRootSignature->SetRootSignature(m_raytracingLocalRootSignature.Get());
		// Define explicit shader association for the local root signature. 
		{
			auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			rootSignatureAssociation->AddExport(c_hitGroupName);
		}
	}

	void D3D12RayTracingCubeRenderer::CreateRaytracingPipelineStateObject()
	{
		// Create 7 subobjects that combine into a RTPSO:
  // Subobjects need to be associated with DXIL exports (i.e. shaders) either by way of default or explicit associations.
  // Default association applies to every exported shader entrypoint that doesn't have any of the same type of subobject associated with it.
  // This simple sample utilizes default shader association except for local root signature subobject
  // which has an explicit association specified purely for demonstration purposes.
  // 1 - DXIL library
  // 1 - Triangle hit group
  // 1 - Shader config
  // 2 - Local root signature and association
  // 1 - Global root signature
  // 1 - Pipeline config
		CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


		// DXIL library
		// This contains the shaders and their entrypoints for the state object.
		// Since shaders are not considered a subobject, they need to be passed in via DXIL library subobjects.
		auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRaytracingCube, ARRAYSIZE(g_pRaytracingCube));
		lib->SetDXILLibrary(&libdxil);
		// Define which shader exports to surface from the library.
		// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
		// In this sample, this could be ommited for convenience since the sample uses all shaders in the library. 
		{
			lib->DefineExport(c_raygenShaderName);
			lib->DefineExport(c_closestHitShaderName);
			lib->DefineExport(c_missShaderName);
		}

		// Triangle hit group
		// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
		// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
		auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
		hitGroup->SetHitGroupExport(c_hitGroupName);
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		// Shader config
		// Defines the maximum sizes in bytes for the ray payload and attribute structure.
		auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		
		/*
			The maximum payload size used by the shaders, which defines the amount of data a shader can exchange with other shaders.
			A typical example is a payload containing a color value, 
			so that the hit or miss shaders can return this value to the ray generation program, which will write that value to the output buffer. 
			To achieve best performance the payload needs to be kept as small as possible. In our case the payload contains 4 floating-point values,
			representing the output color and the distance of the hit from the ray origin.
		*/
		UINT payloadSize = sizeof(XMFLOAT4);    // float4 pixelColor

		/*
			The attributes size, which is set by the intersection shader.
			We use the built-in triangle intersection shader that return 2
			floating-point values corresponding to the barycentric coordinates 
			of the hit point inside the triangle. Those values are accessed 
			using the MyAttributes structure in RayTracingCube.hlsl
		*/
		UINT attributeSize = sizeof(XMFLOAT2);  // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		// Local root signature and shader association
		// This is a root signature that enables a shader to have unique arguments that come from shader tables.
		CreateLocalRootSignatureSubobjects(&raytracingPipeline);

		// Global root signature
		// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

		// Pipeline config
		// Defines the maximum TraceRay() recursion depth.
		auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths.
		UINT maxRecursionDepth = 1; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

#if _DEBUG
		PrintStateObjectDesc(raytracingPipeline);
#endif

		// Create the state object.
		ThrowIfFailed(m_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)), L"Couldn't create DirectX Raytracing state object.\n");
	}

	void D3D12RayTracingCubeRenderer::CreateDescriptorHeap()
	{
		auto device = m_deviceResources->GetD3DDevice();

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		// Allocate a heap for 3 descriptors:
		// 2 - vertex and index buffer SRVs
		// 1 - raytracing output texture SRV
		descriptorHeapDesc.NumDescriptors = 3;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;
		device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));
		NAME_D3D12_OBJECT(m_descriptorHeap);

		m_descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	void D3D12RayTracingCubeRenderer::CreateRaytracingOutputResource()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto backbufferFormat = m_deviceResources->GetBackBufferFormat();

		// Create the output resource. The dimensions and format should match the swap-chain.
		auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, m_Width, m_Height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		ThrowIfFailed(device->CreateCommittedResource(
			&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_raytracingOutput)));
		NAME_D3D12_OBJECT(m_raytracingOutput);

		D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
		m_raytracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(&uavDescriptorHandle, m_raytracingOutputResourceUAVDescriptorHeapIndex);
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
		m_raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_descriptorSize);
	}

	void D3D12RayTracingCubeRenderer::BuildGeometry()
	{
		auto device = m_deviceResources->GetD3DDevice();

		// Cube indices.
		Index indices[] =
		{
			3,1,0,
			2,1,3,

			6,4,5,
			7,4,6,

			11,9,8,
			10,9,11,

			14,12,13,
			15,12,14,

			19,17,16,
			18,17,19,

			22,20,21,
			23,20,22
		};

		// Cube vertices positions and corresponding triangle normals.
		Vertex vertices[] =
		{
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, -1.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f) },

			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f) },

			{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
		};

		AllocateUploadBuffer(device, indices, sizeof(indices), &m_indexBuffer.resource);
		AllocateUploadBuffer(device, vertices, sizeof(vertices), &m_vertexBuffer.resource);

		// Vertex buffer is passed to the shader along with index buffer as a descriptor table.
		// Vertex buffer descriptor must follow index buffer descriptor in the descriptor heap.
		UINT descriptorIndexIB = CreateBufferSRV(&m_indexBuffer, sizeof(indices) / 4, 0);
		UINT descriptorIndexVB = CreateBufferSRV(&m_vertexBuffer, ARRAYSIZE(vertices), sizeof(vertices[0]));
		ThrowIfFalse(descriptorIndexVB == descriptorIndexIB + 1, L"Vertex Buffer descriptor index must follow that of Index Buffer descriptor index!");
	}

	// Allocate a descriptor and return its index. 
	// If the passed descriptorIndexToUse is valid, it will be used instead of allocating a new one.
	UINT D3D12RayTracingCubeRenderer::AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse)
	{
		auto descriptorHeapCpuBase = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		if (descriptorIndexToUse >= m_descriptorHeap->GetDesc().NumDescriptors)
		{
			descriptorIndexToUse = m_descriptorsAllocated++;
		}
		*cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeapCpuBase, descriptorIndexToUse, m_descriptorSize);
		return descriptorIndexToUse;
	}


	// Create SRV for a buffer.
	UINT D3D12RayTracingCubeRenderer::CreateBufferSRV(D3DBuffer* buffer, UINT numElements, UINT elementSize)
	{
		auto device = m_deviceResources->GetD3DDevice();

		// SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
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
		UINT descriptorIndex = AllocateDescriptor(&buffer->cpuDescriptorHandle);
		device->CreateShaderResourceView(buffer->resource.Get(), &srvDesc, buffer->cpuDescriptorHandle);
		buffer->gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_descriptorSize);
		return descriptorIndex;
	}

	void D3D12RayTracingCubeRenderer::UpdateCameraMatrices()
	{
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		m_sceneCB[frameIndex].cameraPosition = m_eye;
		float fovAngleY = 45.0f;
		XMMATRIX view = XMMatrixLookAtLH(m_eye, m_at, m_up);
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(fovAngleY), m_AspectRatio, 1.0f, 125.0f);
		XMMATRIX viewProj = view * proj;

		m_sceneCB[frameIndex].projectionToWorld = XMMatrixInverse(nullptr, viewProj);
	}

	void D3D12RayTracingCubeRenderer::BuildAccelerationStructures()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();
		auto commandQueue = m_deviceResources->GetCommandQueue();
		auto commandAllocator = m_deviceResources->GetCommandAllocator();

		// Reset the command list for the acceleration structure construction.
		commandList->Reset(commandAllocator, nullptr);

		D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
		geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
		geometryDesc.Triangles.IndexBuffer = m_indexBuffer.resource->GetGPUVirtualAddress();
		geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer.resource->GetDesc().Width) / sizeof(Index);
		geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
		geometryDesc.Triangles.Transform3x4 = 0;
		geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer.resource->GetDesc().Width) / sizeof(Vertex);
		geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer.resource->GetGPUVirtualAddress();
		geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

		// Mark the geometry as opaque. 
		// PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
		// Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
		geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

		// Get required sizes for an acceleration structure.
		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
		bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		bottomLevelInputs.Flags = buildFlags;
		bottomLevelInputs.NumDescs = 1;
		bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelInputs.pGeometryDescs = &geometryDesc;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
		topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelInputs.Flags = buildFlags;
		topLevelInputs.NumDescs = 1;
		topLevelInputs.pGeometryDescs = nullptr;
		topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
		m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
		ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
		m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
		ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

		ComPtr<ID3D12Resource> scratchResource;
		AllocateUAVBuffer(device, std::max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

		// Allocate resources for acceleration structures.
		// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
		// Default heap is OK since the application doesn’t need CPU read/write access to them. 
		// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
		// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
		//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
		//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
		{
			D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

			AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
			AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
		}

		// Create an instance desc for the bottom-level acceleration structure.
		ComPtr<ID3D12Resource> instanceDescs;
		D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
		instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
		instanceDesc.InstanceMask = 1;
		instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
		AllocateUploadBuffer(device, &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

		// Bottom Level Acceleration Structure desc
		{
			bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
			bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
		}

		// Top Level Acceleration Structure desc
		{
			topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
			topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
			topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
		}

		auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
		{
			raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
			commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get()));
			raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
		};

		// Build acceleration structure.
		BuildAccelerationStructure(m_dxrCommandList.Get());

		// Kick off acceleration structure construction.
		m_deviceResources->ExecuteCommandList();

		// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
		m_deviceResources->WaitForGpu();
	}

	void D3D12RayTracingCubeRenderer::BuildShaderTables()
	{
		auto device = m_deviceResources->GetD3DDevice();

		void* rayGenShaderIdentifier;
		void* missShaderIdentifier;
		void* hitGroupShaderIdentifier;

		auto GetShaderIdentifiers = [&](auto* stateObjectProperties)
		{
			rayGenShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_raygenShaderName);
			missShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_missShaderName);
			hitGroupShaderIdentifier = stateObjectProperties->GetShaderIdentifier(c_hitGroupName);
		};

		// Get shader identifiers.
		UINT shaderIdentifierSize;
		{
			ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
			ThrowIfFailed(m_dxrStateObject.As(&stateObjectProperties));
			GetShaderIdentifiers(stateObjectProperties.Get());
			shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		}

		// Ray gen shader table
		{
			UINT numShaderRecords = 1;
			UINT shaderRecordSize = shaderIdentifierSize;
			ShaderTable rayGenShaderTable(device, numShaderRecords, shaderRecordSize, L"RayGenShaderTable");
			rayGenShaderTable.push_back(ShaderRecord(rayGenShaderIdentifier, shaderIdentifierSize));
			m_rayGenShaderTable = rayGenShaderTable.GetResource();
		}

		// Miss shader table
		{
			UINT numShaderRecords = 1;
			UINT shaderRecordSize = shaderIdentifierSize;
			ShaderTable missShaderTable(device, numShaderRecords, shaderRecordSize, L"MissShaderTable");
			missShaderTable.push_back(ShaderRecord(missShaderIdentifier, shaderIdentifierSize));
			m_missShaderTable = missShaderTable.GetResource();
		}

		// Hit group shader table
		{
			struct RootArguments {
				CubeConstantBuffer cb;
			} rootArguments;
			rootArguments.cb = m_cubeCB;

			UINT numShaderRecords = 1;
			UINT shaderRecordSize = shaderIdentifierSize + sizeof(rootArguments);
			ShaderTable hitGroupShaderTable(device, numShaderRecords, shaderRecordSize, L"HitGroupShaderTable");
			hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderIdentifier, shaderIdentifierSize, &rootArguments, sizeof(rootArguments)));
			m_hitGroupShaderTable = hitGroupShaderTable.GetResource();
		}

	}

	

	void D3D12RayTracingCubeRenderer::CopyRaytracingOutputToBackbuffer()
	{
		auto commandList = m_deviceResources->GetCommandList();
		auto renderTarget = m_deviceResources->GetRenderTarget();

		D3D12_RESOURCE_BARRIER preCopyBarriers[2];
		preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
		preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_SOURCE);
		commandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

		commandList->CopyResource(renderTarget, m_raytracingOutput.Get());

		D3D12_RESOURCE_BARRIER postCopyBarriers[2];
		postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
		postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		commandList->ResourceBarrier(ARRAYSIZE(postCopyBarriers), postCopyBarriers);
	}

	void D3D12RayTracingCubeRenderer::CalculateFrameStats()
	{
	}


	void D3D12RayTracingCubeRenderer::Render(const Camera& camera, Object* pointLight)
	{
		if (!m_deviceResources->IsWindowVisible())
		{
			return;
		}

		//auto frameIndex = m_deviceResources->GetCurrentFrameIndex();
		//m_sceneCB[frameIndex].cameraPosition = glm::vec4(camera.GetPosition(),1.0);

		//glm::mat4 viewProj = camera.GetViewMatrix() * camera.GetProjectionMatrix();
		//m_sceneCB[frameIndex].projectionToWorld = glm::inverse(viewProj);
		//m_sceneCB[frameIndex].lightPosition = glm::vec4(0.0f, 1.8f, -3.0f, 0.0f); ;// pointLight->GetComponent<Transform>()->GetPosition();


		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();
		auto prevFrameIndex = m_deviceResources->GetPreviousFrameIndex();

	// Rotate the camera around Y axis.
		{
			float secondsToRotateAround = 24.0f;
			float angleToRotateBy = 0.05f;// 360.0f * m_rotate / 100;
			m_rotate += 1;
			if (m_rotate == 100)
				m_rotate = 10;
			XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(angleToRotateBy));
			m_eye = XMVector3Transform(m_eye, rotate);
			m_up = XMVector3Transform(m_up, rotate);
			m_at = XMVector3Transform(m_at, rotate);
			UpdateCameraMatrices();
		}

		// Rotate the second light around Y axis.
		{
			float secondsToRotateAround = 8.0f;
			float angleToRotateBy = 0.05f;
			XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(angleToRotateBy));
			const XMVECTOR& prevLightPosition = m_sceneCB[prevFrameIndex].lightPosition;
			m_sceneCB[frameIndex].lightPosition = XMVector3Transform(prevLightPosition, rotate);
		}



		m_deviceResources->Prepare();
		
		
		DoRaytracing();
		CopyRaytracingOutputToBackbuffer();

		m_deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
	}
}