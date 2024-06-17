#include "pch.h"
#ifdef GE_API_D3D12
#include "D3D12RayTracingModelRenderer.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Common/CommonFunc.h"
#include "BlackPearl/RHI/D3D12RHI/DirectXRaytracingHelper.h"
#include "BlackPearl/Renderer/Shader/D3D12Shader/ShaderTable.h"
#include "BlackPearl/Renderer/Shader/D3D12Shader/ShaderRecord.h"
#include "BlackPearl/Config.h"
#include "BlackPearl/Math/Math.h"
#include "BlackPearl/Scene/DemoScene/MetaBallSDFScene.h"
#include "BlackPearl/Debugger/D3D12Debugger/HLSLPixDebugger.h"
#include "RayTracingMetaball.hlsl.h"
#include "RaytracingHlslCompat.h"
#include <windows.h>
#include <DirectXMathConvert.inl>

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif
namespace BlackPearl {

	extern  DynamicRHI::Type g_RHIType;

	D3D12RayTracingModelRenderer::D3D12RayTracingModelRenderer()
		: D3D12Renderer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight()),
		m_adapterIDoverride(UINT_MAX),
		m_FrameCount(3),
		m_DescriptorsAllocated(0)

	{
	}

	D3D12RayTracingModelRenderer::~D3D12RayTracingModelRenderer()
	{
		// Let GPU finish before releasing D3D resources.
		m_deviceResources->WaitForGpu();
		OnDeviceLost();
	}

	void D3D12RayTracingModelRenderer::Init()
	{
		if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
		{
			LoadLibrary(HLSLPixDebugger::GetLatestWinPixGpuCapturerPath_Cpp17().c_str());
		}
		m_deviceResources = std::make_unique<DeviceResources>(
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_UNKNOWN,
			m_FrameCount,
			D3D_FEATURE_LEVEL_11_0,
			DeviceResources::c_RequireTearingSupport,
			m_adapterIDoverride
			);
		m_deviceResources->RegisterDeviceNotify(this);
		m_deviceResources->SetWindow((HWND)(Application::Get().GetWindow().GetNativeWindow()),
			m_Width,
			m_Height);

		m_deviceResources->InitializeDXGIAdapter();
		ThrowIfFalse(IsDirectXRaytracingSupported(m_deviceResources->GetAdapter()),
			L"ERROR: DirectX Raytracing is not supported by your OS, GPU and/or driver.\n\n");

		m_deviceResources->CreateDeviceResources();
		m_deviceResources->CreateWindowSizeDependentResources();

		// Setup camera.
		{
			// Initialize the view and projection inverse matrices.
			/*m_eye = { 0.0f, 5.3f, -17.0f, 1.0f };
			m_at = { 0.0f, 0.0f, 0.0f, 1.0f };
			XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

			XMVECTOR direction = XMVector4Normalize(m_at - m_eye);
			m_up = XMVector3Normalize(XMVector3Cross(direction, right));*/
			//    XMMATRIX view = XMMatrixLookAtLH(m_eye, m_at, m_up);
			// 
			// Rotate camera around Y axis.
			//XMMATRIX rotate = XMMatrixRotationY(XMConvertToRadians(45.0f));
			//m_eye = XMVector3Transform(m_eye, rotate);
			//m_up = XMVector3Transform(m_up, rotate);

			//UpdateCameraMatrices();
		}

		// Setup lights.
		{
			// Initialize the lighting parameters.
			XMFLOAT4 lightPosition;
			XMFLOAT4 lightAmbientColor;
			XMFLOAT4 lightDiffuseColor;

			lightPosition = XMFLOAT4(0.0f, 18.0f, -20.0f, 0.0f);
			m_gSceneCB->lightPosition = XMLoadFloat4(&lightPosition);

			lightAmbientColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
			m_gSceneCB->lightAmbientColor = XMLoadFloat4(&lightAmbientColor);

			float d = 0.6f;
			lightDiffuseColor = XMFLOAT4(d, d, d, 1.0f);
			m_gSceneCB->lightDiffuseColor = XMLoadFloat4(&lightDiffuseColor);
		}
		CreateDeviceDependentResources();
		CreateRaytracingOutputResource();

		//UpdateCameraMatrices();
	}
	void D3D12RayTracingModelRenderer::InitScene(SDFScene* scene)
	{
		m_Scene = scene;
	}

	void D3D12RayTracingModelRenderer::Render(const Camera& camera, const LightSources& lights, float elapsedTime)
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

		{
			UpdateCameraMatrices(camera);

		//	UpdateCameraMatrices(camera);
			UpdateLightSources(lights);
			//UpdateElapsedTime(elapsedTime);
			UpdateElapsedTime(elapsedTime);

			UpdateAABBPrimitiveAttributes(elapsedTime);

		}
		
		m_deviceResources->Prepare();
		

		DoRayTracing();
		CopyRaytracingOutputToBackbuffer();



		m_deviceResources->Present(D3D12_RESOURCE_STATE_PRESENT);
	
	}

	void D3D12RayTracingModelRenderer::OnDeviceLost()
	{
		ReleaseResources();
	}

	void D3D12RayTracingModelRenderer::OnDeviceRestored()
	{
		CreateDeviceDependentResources();
		CreateRaytracingOutputResource();
	}

	void D3D12RayTracingModelRenderer::CreateDeviceDependentResources()
	{
		// [1] Create raytracing device and command list.
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();

		ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&m_dxrDevice)), L"Couldn't get DirectX Raytracing interface for the device.\n");
		ThrowIfFailed(commandList->QueryInterface(IID_PPV_ARGS(&m_dxrCommandList)), L"Couldn't get DirectX Raytracing interface for the command list.\n");

		// [2] Create root signatures for the shaders.
		// get root signature from scene 
		// serialize global root signature
		//CD3DX12_ROOT_SIGNATURE_DESC& desc = m_Scene->GetGlobalRootSignatureDesc();
		SerializeAndCreateRootSignature(m_Scene->GetGlobalRootSignatureDesc(), &m_GlobalRootSignature);
		// serialize local root signature
		int localDescSize = m_Scene->GetLocalRootSignatruesDesc().size();
		m_LocalRootSignatures.assign(localDescSize, ComPtr<ID3D12RootSignature>());
		for (size_t i = 0; i < localDescSize; i++)
		{
			SerializeAndCreateRootSignature(m_Scene->GetLocalRootSignatruesDesc()[i], &m_LocalRootSignatures[i]);
		}

		//[3] -----------------------------------pipline state object---------m_dxrStateObject----------------------------------------------------------------------------------

		CreateRaytracingPipelineStateObject();
		// ------------------------------------------------------------------------------------------------------------------------------
		//[4] create discriptor heap
		  // Create a heap for descriptors.
		CreateDescriptorHeap();

		// Build geometry to be used in the sample.
		BuildGeometry();

		// Build raytracing acceleration structures from the generated geometry.
		BuildAccelerationStructures();

		// Create constant buffers for the geometry and the scene.
		CreateShaderGlobalBuffers();

		// Build shader tables, which define shaders and their local root arguments.
		BuildShaderTables();

		// Create an output 2D texture to store the raytracing result to.
		CreateRaytracingOutputResource();

	}
	// Create a 2D output texture for raytracing.
	void D3D12RayTracingModelRenderer::CreateRaytracingOutputResource()
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
		m_raytracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(&uavDescriptorHandle, m_DescriptorHeap, m_DescriptorsAllocated, m_DescriptorSize, m_raytracingOutputResourceUAVDescriptorHeapIndex);
		D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
		UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
		m_raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_DescriptorSize);
	}
	//void D3D12RayTracingModelRenderer::UpdateCameraMatrices()
	void D3D12RayTracingModelRenderer::UpdateCameraMatrices(const Camera& camera)
	{
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();
		m_gSceneCB->cameraPosition = { camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z, 1.0f };
		XMVECTOR front = { camera.Front().x, camera.Front().y, camera.Front().z, 0.0 };
		XMVECTOR up = { camera.Up().x, camera.Up().y, camera.Up().z, 0.0 };

		//XMMATRIX view = XMMatrixLookAtLH(m_gSceneCB->cameraPosition, m_at, m_up);
		XMMATRIX view = XMMatrixLookAtLH(m_gSceneCB->cameraPosition, m_gSceneCB->cameraPosition + front, up);

		//XMMATRIX view = XMMatrixLookAtLH(m_gSceneCB->cameraPosition, m_at, m_up);
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0), m_AspectRatio, 0.01f, 125.0f);
	/*	XMMATRIX view = XMMATRIX(&camera.GetViewMatrix()[0][0]);
		XMMATRIX project = XMMATRIX(&camera.GetProjectionMatrix()[0][0]);*/
		XMMATRIX viewProj = view * proj;
		m_gSceneCB->projectionToWorld = XMMatrixInverse(nullptr, viewProj);
	}

	void D3D12RayTracingModelRenderer::UpdateLightSources(const LightSources& lightSources)
	{
		for (Object* lightObj : lightSources.Get()) {
			if (lightObj->HasComponent<ParallelLight>()) {

			}
			if (lightObj->HasComponent<PointLight>()) {
				m_gSceneCB->lightPosition = {
					lightObj->GetComponent<Transform>()->GetPosition().x, 
					lightObj->GetComponent<Transform>()->GetPosition().y,
					lightObj->GetComponent<Transform>()->GetPosition().z,
					1.0f
				};
				XMFLOAT4 lightAmbientColor;
				XMFLOAT4 lightDiffuseColor;
				lightAmbientColor = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
				m_gSceneCB->lightAmbientColor = XMLoadFloat4(&lightAmbientColor);

				float d = 0.6f;
				lightDiffuseColor = XMFLOAT4(d, d, d, 1.0f);
				m_gSceneCB->lightDiffuseColor = XMLoadFloat4(&lightDiffuseColor);
				break;
			}
		}

	}

	void D3D12RayTracingModelRenderer::UpdateElapsedTime(float timeSecond) {
		m_gSceneCB->elapsedTime = timeSecond;
	}

	void D3D12RayTracingModelRenderer::ReleaseResources()
	{
		m_dxrDevice.Reset();
		m_dxrCommandList.Reset();
		m_dxrStateObject.Reset();


		m_GlobalRootSignature.Reset();
		ResetComPtrVector(m_LocalRootSignatures);

		m_DescriptorHeap.Reset();
		m_DescriptorsAllocated = 0;

		m_aabbBuffer.resource.Reset();
		m_indexBuffer.resource.Reset();
		m_vertexBuffer.resource.Reset();

		m_gSceneCB.Release();
		m_gAabbPrimitiveAttributeBuffer.Release();

		m_raytracingOutput.Reset();
		m_raytracingOutputResourceUAVDescriptorHeapIndex = UINT_MAX;

		m_MissShaderTable.Reset();
		m_RayGenShaderTable.Reset();
		m_HitGroupShaderTable.Reset();

		ResetComPtrArray(&m_bottomLevelAS);
		m_topLevelAS.Reset();
	}
	
	void D3D12RayTracingModelRenderer::SerializeAndCreateRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig)
	{
		auto device = m_deviceResources->GetD3DDevice();
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;
		ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error));
		ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));
	}

	void D3D12RayTracingModelRenderer::BuildGeometry()
	{
		BuildProceduralGeometryAABBs();
		BuildPlaneGeometry();
	}

	void D3D12RayTracingModelRenderer::BuildProceduralGeometryAABBs()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto& aabbs = m_Scene->GetAABBsGeometry();
		AllocateUploadBuffer(device, aabbs.data(), aabbs.size() * sizeof(aabbs[0]), &m_aabbBuffer.resource);
	}

	void D3D12RayTracingModelRenderer::BuildPlaneGeometry()
	{
		auto device = m_deviceResources->GetD3DDevice();
		// Plane indices.
		Index indices[] =
		{
			3,1,0,
			2,1,3,

		};

		// Cube vertices positions and corresponding triangle normals.
		Vertex vertices[] =
		{
			{ XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
		};

		AllocateUploadBuffer(device, indices, sizeof(indices), &m_indexBuffer.resource);
		AllocateUploadBuffer(device, vertices, sizeof(vertices), &m_vertexBuffer.resource);

		// Vertex buffer is passed to the shader along with index buffer as a descriptor range.
		UINT descriptorIndexIB = CreateBufferSRV(device, m_DescriptorHeap, m_DescriptorsAllocated, m_DescriptorSize, &m_indexBuffer, sizeof(indices) / 4, 0);
		UINT descriptorIndexVB = CreateBufferSRV(device, m_DescriptorHeap, m_DescriptorsAllocated, m_DescriptorSize, &m_vertexBuffer, ARRAYSIZE(vertices), sizeof(vertices[0]));
		ThrowIfFalse(descriptorIndexVB == descriptorIndexIB + 1, L"Vertex Buffer descriptor index must follow that of Index Buffer descriptor index");
	}
	
	void D3D12RayTracingModelRenderer::BuildAccelerationStructures()
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();
		auto commandQueue = m_deviceResources->GetCommandQueue();
		auto commandAllocator = m_deviceResources->GetCommandAllocator();

		// Reset the command list for the acceleration structure construction.
		commandList->Reset(commandAllocator, nullptr);

		// Build bottom-level AS.
		AccelerationStructureBuffers bottomLevelAS[BottomLevelASType::Count];
		std::array<std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>, BottomLevelASType::Count> geometryDescs;
		{
			BuildGeometryDescsForBottomLevelAS(geometryDescs);

			// Build all bottom-level AS.
			for (UINT i = 0; i < BottomLevelASType::Count; i++)
			{
				bottomLevelAS[i] = BuildBottomLevelAS(geometryDescs[i]);
			}
		}

		// Batch all resource barriers for bottom-level AS builds.
		D3D12_RESOURCE_BARRIER resourceBarriers[BottomLevelASType::Count];
		for (UINT i = 0; i < BottomLevelASType::Count; i++)
		{
			resourceBarriers[i] = CD3DX12_RESOURCE_BARRIER::UAV(bottomLevelAS[i].accelerationStructure.Get());
		}
		commandList->ResourceBarrier(BottomLevelASType::Count, resourceBarriers);

		// Build top-level AS.
		AccelerationStructureBuffers topLevelAS = BuildTopLevelAS(bottomLevelAS);

		// Kick off acceleration structure construction.
		m_deviceResources->ExecuteCommandList();

		// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
		m_deviceResources->WaitForGpu();

		// Store the AS buffers. The rest of the buffers will be released once we exit the function.
		for (UINT i = 0; i < BottomLevelASType::Count; i++)
		{
			m_bottomLevelAS[i] = bottomLevelAS[i].accelerationStructure;
		}
		m_topLevelAS = topLevelAS.accelerationStructure;
	}


	void D3D12RayTracingModelRenderer::BuildGeometryDescsForBottomLevelAS(std::array<std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>, BottomLevelASType::Count>& geometryDescs)
	{
		D3D12_RAYTRACING_GEOMETRY_FLAGS geometryFlags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
		// Triangle geometry desc
		{
			// Triangle bottom-level AS contains a single plane geometry.
			geometryDescs[BottomLevelASType::Triangle].resize(1);

			// Plane geometry
			auto& geometryDesc = geometryDescs[BottomLevelASType::Triangle][0];
			geometryDesc = {};
			geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geometryDesc.Triangles.IndexBuffer = m_indexBuffer.resource->GetGPUVirtualAddress();
			geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer.resource->GetDesc().Width) / sizeof(Index);
			geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
			geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer.resource->GetDesc().Width) / sizeof(Vertex);
			geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer.resource->GetGPUVirtualAddress();
			geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);
			geometryDesc.Flags = geometryFlags;
		}

		// AABB geometry desc
		{
			D3D12_RAYTRACING_GEOMETRY_DESC aabbDescTemplate = {};
			aabbDescTemplate.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_PROCEDURAL_PRIMITIVE_AABBS;
			aabbDescTemplate.AABBs.AABBCount = 1;
			aabbDescTemplate.AABBs.AABBs.StrideInBytes = sizeof(D3D12_RAYTRACING_AABB);
			aabbDescTemplate.Flags = geometryFlags;

			// One AABB primitive per geometry.
			geometryDescs[BottomLevelASType::AABB].resize(m_Scene->GetTotalPrimitiveCount(), aabbDescTemplate);

			// Create AABB geometries. 
			// Having separate geometries allows of separate shader record binding per geometry.
			// In this sample, this lets us specify custom hit groups per AABB geometry.
			for (UINT i = 0; i < m_Scene->GetTotalPrimitiveCount(); i++)
			{
				auto& geometryDesc = geometryDescs[BottomLevelASType::AABB][i];
				geometryDesc.AABBs.AABBs.StartAddress = m_aabbBuffer.resource->GetGPUVirtualAddress() + i * sizeof(D3D12_RAYTRACING_AABB);
			}
		}

	}
	AccelerationStructureBuffers D3D12RayTracingModelRenderer::BuildBottomLevelAS(const std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>& geometryDesc, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags)
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();
		ComPtr<ID3D12Resource> scratch;
		ComPtr<ID3D12Resource> bottomLevelAS;

		// Get the size requirements for the scratch and AS buffers.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
		bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		bottomLevelInputs.Flags = buildFlags;
		bottomLevelInputs.NumDescs = static_cast<UINT>(geometryDesc.size());
		bottomLevelInputs.pGeometryDescs = geometryDesc.data();

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
		m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
		ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

		// Create a scratch buffer.
		AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ScratchDataSizeInBytes, &scratch, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

		// Allocate resources for acceleration structures.
		// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
		// Default heap is OK since the application doesn�t need CPU read/write access to them. 
		// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
		// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
		//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
		//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
		{
			D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &bottomLevelAS, initialResourceState, L"BottomLevelAccelerationStructure");
		}

		// bottom-level AS desc.
		{
			bottomLevelBuildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
			bottomLevelBuildDesc.DestAccelerationStructureData = bottomLevelAS->GetGPUVirtualAddress();
		}

		// Build the acceleration structure.
		m_dxrCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);

		AccelerationStructureBuffers bottomLevelASBuffers;
		bottomLevelASBuffers.accelerationStructure = bottomLevelAS;
		bottomLevelASBuffers.scratch = scratch;
		bottomLevelASBuffers.ResultDataMaxSizeInBytes = bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes;
		return bottomLevelASBuffers;
	}
	AccelerationStructureBuffers D3D12RayTracingModelRenderer::BuildTopLevelAS(AccelerationStructureBuffers bottomLevelAS[BottomLevelASType::Count], D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags)
	{
		auto device = m_deviceResources->GetD3DDevice();
		auto commandList = m_deviceResources->GetCommandList();
		ComPtr<ID3D12Resource> scratch;
		ComPtr<ID3D12Resource> topLevelAS;

		// Get required sizes for an acceleration structure.
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
		topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		topLevelInputs.Flags = buildFlags;
		topLevelInputs.NumDescs = NUM_BLAS;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
		m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
		ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

		AllocateUAVBuffer(device, topLevelPrebuildInfo.ScratchDataSizeInBytes, &scratch, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

		// Allocate resources for acceleration structures.
		// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
		// Default heap is OK since the application doesn�t need CPU read/write access to them. 
		// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
		// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
		//  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
		//  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
		{
			D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;
			AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &topLevelAS, initialResourceState, L"TopLevelAccelerationStructure");
		}

		// Create instance descs for the bottom-level acceleration structures.
		ComPtr<ID3D12Resource> instanceDescsResource;
		{
			D3D12_RAYTRACING_INSTANCE_DESC instanceDescs[BottomLevelASType::Count] = {};
			D3D12_GPU_VIRTUAL_ADDRESS bottomLevelASaddresses[BottomLevelASType::Count] =
			{
				bottomLevelAS[0].accelerationStructure->GetGPUVirtualAddress(),
				bottomLevelAS[1].accelerationStructure->GetGPUVirtualAddress()
			};
			BuildBotomLevelASInstanceDescs<D3D12_RAYTRACING_INSTANCE_DESC>(bottomLevelASaddresses, &instanceDescsResource);
		}

		// Top-level AS desc
		{
			topLevelBuildDesc.DestAccelerationStructureData = topLevelAS->GetGPUVirtualAddress();
			topLevelInputs.InstanceDescs = instanceDescsResource->GetGPUVirtualAddress();
			topLevelBuildDesc.ScratchAccelerationStructureData = scratch->GetGPUVirtualAddress();
		}

		// Build acceleration structure.
		m_dxrCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);

		AccelerationStructureBuffers topLevelASBuffers;
		topLevelASBuffers.accelerationStructure = topLevelAS;
		topLevelASBuffers.instanceDesc = instanceDescsResource;
		topLevelASBuffers.scratch = scratch;
		topLevelASBuffers.ResultDataMaxSizeInBytes = topLevelPrebuildInfo.ResultDataMaxSizeInBytes;
		return topLevelASBuffers;
	}

	template <class InstanceDescType, class BLASPtrType>
	void D3D12RayTracingModelRenderer::BuildBotomLevelASInstanceDescs(BLASPtrType* bottomLevelASaddresses, ComPtr<ID3D12Resource>* instanceDescsResource)
	{
		auto device = m_deviceResources->GetD3DDevice();
		UINT aabbWidth = dynamic_cast<MetaBallSDFScene*>(m_Scene)->GetAABBWidth();
		UINT aabbDistance = dynamic_cast<MetaBallSDFScene*>(m_Scene)->GetAABBDistance();
		std::vector<InstanceDescType> instanceDescs;
		instanceDescs.resize(NUM_BLAS);

		// Width of a bottom-level AS geometry.
		// Make the plane a little larger than the actual number of primitives in each dimension.
		const XMUINT3 NUM_AABB = XMUINT3(700, 1, 700);
		const XMFLOAT3 fWidth = XMFLOAT3(
			NUM_AABB.x * aabbWidth + (NUM_AABB.x - 1) * aabbDistance,
			NUM_AABB.y * aabbWidth + (NUM_AABB.y - 1) * aabbDistance,
			NUM_AABB.z * aabbWidth + (NUM_AABB.z - 1) * aabbDistance);
		const XMVECTOR vWidth = XMLoadFloat3(&fWidth);


		// Bottom-level AS with a single plane.
		{
			auto& instanceDesc = instanceDescs[BottomLevelASType::Triangle];
			instanceDesc = {};
			instanceDesc.InstanceMask = 1;
			instanceDesc.InstanceContributionToHitGroupIndex = 0;
			instanceDesc.AccelerationStructure = bottomLevelASaddresses[BottomLevelASType::Triangle];

			// Calculate transformation matrix.
			const XMVECTOR vBasePosition = vWidth * XMLoadFloat3(&XMFLOAT3(-0.35f, 0.0f, -0.35f));

			// Scale in XZ dimensions.
			XMMATRIX mScale = XMMatrixScaling(fWidth.x, fWidth.y, fWidth.z);
			XMMATRIX mTranslation = XMMatrixTranslationFromVector(vBasePosition);
			XMMATRIX mTransform = mScale * mTranslation;
			XMStoreFloat3x4(reinterpret_cast<XMFLOAT3X4*>(instanceDesc.Transform), mTransform);
		}

		// Create instanced bottom-level AS with procedural geometry AABBs.
		// Instances share all the data, except for a transform.
		{
			auto& instanceDesc = instanceDescs[BottomLevelASType::AABB];
			instanceDesc = {};
			instanceDesc.InstanceMask = 1;

			// Set hit group offset to beyond the shader records for the triangle AABB.
			instanceDesc.InstanceContributionToHitGroupIndex = BottomLevelASType::AABB * RayType::Count;
			instanceDesc.AccelerationStructure = bottomLevelASaddresses[BottomLevelASType::AABB];

			// Move all AABBS above the ground plane.
			XMMATRIX mTranslation = XMMatrixTranslationFromVector(XMLoadFloat3(&XMFLOAT3(0, aabbWidth / 2, 0)));
			XMStoreFloat3x4(reinterpret_cast<XMFLOAT3X4*>(instanceDesc.Transform), mTranslation);
		}
		UINT64 bufferSize = static_cast<UINT64>(instanceDescs.size() * sizeof(instanceDescs[0]));
		AllocateUploadBuffer(device, instanceDescs.data(), bufferSize, &(*instanceDescsResource), L"InstanceDescs");
	};
	void D3D12RayTracingModelRenderer::BuildShaderTables()
	{
		auto device = m_deviceResources->GetD3DDevice();

		void* rayGenShaderID;
		void* missShaderIDs[RayType::Count];
		void* hitGroupShaderIDs_TriangleGeometry[RayType::Count];
		void* hitGroupShaderIDs_AABBGeometry[SDFScene::IntersectionShaderType::Count_IntersectionShaderType][RayType::Count];

		// A shader name look-up table for shader table debug print out.
		std::unordered_map<void*, std::wstring> shaderIdToStringMap;

		auto GetShaderIDs = [&](auto* stateObjectProperties)
		{
			rayGenShaderID = stateObjectProperties->GetShaderIdentifier(m_Scene->GetRaygenShaderName());
			shaderIdToStringMap[rayGenShaderID] = m_Scene->GetRaygenShaderName();

			for (UINT i = 0; i < RayType::Count; i++)
			{
				missShaderIDs[i] = stateObjectProperties->GetShaderIdentifier(m_Scene->GetMissShaderNames()[RayType(i)]);
				shaderIdToStringMap[missShaderIDs[i]] = m_Scene->GetMissShaderNames()[RayType(i)];
			}
			for (UINT i = 0; i < RayType::Count; i++)
			{
				hitGroupShaderIDs_TriangleGeometry[i] = stateObjectProperties->GetShaderIdentifier(m_Scene->GetTriangleHitGroupNames()[RayType(i)]);
				shaderIdToStringMap[hitGroupShaderIDs_TriangleGeometry[i]] = m_Scene->GetTriangleHitGroupNames()[RayType(i)];
			}
			for (UINT r = 0; r < SDFScene::IntersectionShaderType::Count_IntersectionShaderType; r++)
				for (UINT c = 0; c < RayType::Count; c++)
				{
					hitGroupShaderIDs_AABBGeometry[r][c] = stateObjectProperties->GetShaderIdentifier(m_Scene->GetAABBHitGroupNames()[SDFScene::IntersectionShaderType(r)][RayType(c)]);
					shaderIdToStringMap[hitGroupShaderIDs_AABBGeometry[r][c]] = m_Scene->GetAABBHitGroupNames()[SDFScene::IntersectionShaderType(r)][RayType(c)];
				}
		};
		// Get shader identifiers.
		UINT shaderIDSize;
		{
			ComPtr<ID3D12StateObjectProperties> stateObjectProperties;
			ThrowIfFailed(m_dxrStateObject.As(&stateObjectProperties));
			GetShaderIDs(stateObjectProperties.Get());
			shaderIDSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
		}
		/*************--------- Shader table layout -------*******************
		| --------------------------------------------------------------------
		| Shader table - HitGroupShaderTable:
		| [0] : MyHitGroup_Triangle
		| [1] : MyHitGroup_Triangle_ShadowRay
		| [2] : MyHitGroup_AABB_AnalyticPrimitive
		| [3] : MyHitGroup_AABB_AnalyticPrimitive_ShadowRay
		| ...
		| [6] : MyHitGroup_AABB_VolumetricPrimitive
		| [7] : MyHitGroup_AABB_VolumetricPrimitive_ShadowRay
		| [8] : MyHitGroup_AABB_SignedDistancePrimitive
		| [9] : MyHitGroup_AABB_SignedDistancePrimitive_ShadowRay,
		| ...
		| [20] : MyHitGroup_AABB_SignedDistancePrimitive
		| [21] : MyHitGroup_AABB_SignedDistancePrimitive_ShadowRay
		| --------------------------------------------------------------------
		**********************************************************************/

		// RayGen shader table.
		{

			UINT numShaderRecord = 1;
			UINT shaderRecordSize = shaderIDSize; // No root arguments

			ShaderTable rayGenShaderTable(device, numShaderRecord, shaderRecordSize, L"RayGenShaderTable");
			rayGenShaderTable.push_back(ShaderRecord(rayGenShaderID, shaderRecordSize, nullptr, 0));
			rayGenShaderTable.DebugPrint(shaderIdToStringMap);
			m_RayGenShaderTable = rayGenShaderTable.GetResource();
		}
		// Miss shader table
		{
			UINT numShaderRecord = RayType::Count;
			UINT shaderRecordSize = shaderIDSize; // No root arguments
			ShaderTable missShaderTable(device, numShaderRecord, shaderRecordSize, L"MissShaderTable");
			for (UINT i = 0; i < RayType::Count; i++)
			{
				missShaderTable.push_back(ShaderRecord(missShaderIDs[i], shaderIDSize, nullptr, 0));
			}
			missShaderTable.DebugPrint(shaderIdToStringMap);
			m_MissShaderTableStrideInBytes = missShaderTable.GetShaderRecordSize();
			m_MissShaderTable = missShaderTable.GetResource();
		}

		// Hit group shader table
		{

			UINT numShaderRecord = RayType::Count + m_Scene->GetTotalPrimitiveCount() * RayType::Count;
			UINT shaderRecordSize = shaderIDSize + m_Scene->GetMaxRootArgumentsSize();
			ShaderTable hitGroupShaderTable(device, numShaderRecord, shaderRecordSize, L"HitGroupShaderTable");

			//Triangle geometry hit group
			{
				SDFScene::TriangleRootArguments rootArgs = m_Scene->GetTriangleRootArgument();
				rootArgs.materialCb = dynamic_cast<MetaBallSDFScene*>(m_Scene)->GetMaterialCbPlane();
				for (auto& hitGroupShaderID : hitGroupShaderIDs_TriangleGeometry)
				{
					hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderID, shaderIDSize, &rootArgs, sizeof(rootArgs)));
				}
			}

			//AABB geometry hit group
			{
				SDFScene::AABBRootArguments rootArgs = m_Scene->GetAABBRootArgument();
				UINT instanceIndex = 0;

				// Create a shader record for each primitive.
				for (UINT iShader = 0, instanceIndex = 0; iShader < SDFScene::IntersectionShaderType::Count_IntersectionShaderType; iShader++)
				{
					UINT numPrimitiveTypes = m_Scene->PerPrimitiveTypeCount(static_cast<SDFScene::IntersectionShaderType>(iShader));

					// Primitives for each intersection shader.
					for (UINT primitiveIndex = 0; primitiveIndex < numPrimitiveTypes; primitiveIndex++, instanceIndex++)
					{
						rootArgs.materialCb = dynamic_cast<MetaBallSDFScene*>(m_Scene)->GetMaterialCbAABB()[instanceIndex];
						rootArgs.aabbCB.instanceIndex = instanceIndex;
						rootArgs.aabbCB.primitiveType = primitiveIndex;

						// Ray types.
						for (UINT r = 0; r < RayType::Count; r++)
						{
							auto& hitGroupShaderID = hitGroupShaderIDs_AABBGeometry[iShader][r];
							hitGroupShaderTable.push_back(ShaderRecord(hitGroupShaderID, shaderIDSize, &rootArgs, sizeof(rootArgs)));
						}
					}
				}
			}
			hitGroupShaderTable.DebugPrint(shaderIdToStringMap);
			m_HitGroupShaderTableStrideInBytes = hitGroupShaderTable.GetShaderRecordSize();
			m_HitGroupShaderTable = hitGroupShaderTable.GetResource();
		}
	}
	void D3D12RayTracingModelRenderer::CreateRaytracingPipelineStateObject()
	{
		// [3] Create a raytracing pipeline state object (RTPSO).
		CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };

		// Create DXIL library subobject
		CD3DX12_DXIL_LIBRARY_SUBOBJECT* lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE((void*)g_pRayTracingMetaball, ARRAYSIZE(g_pRayTracingMetaball));
		lib->SetDXILLibrary(&libdxil);

		// Create Hit group subobject
		// Triangle geometry hit groups
		auto& closestHitShaderNames = m_Scene->GetClosestHitShaderNames();
		auto& triangleHitGroupNames = m_Scene->GetTriangleHitGroupNames();

		{
			for (UINT rayType = 0; rayType < RayType::Count; rayType++)
			{
				auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
				if (rayType == RayType::Radiance)
				{
					auto it = closestHitShaderNames.find(SDFScene::GeometryType::Triangle);
					if (it != closestHitShaderNames.end())
						hitGroup->SetClosestHitShaderImport(it->second);
					else
						continue;
				}
				auto it = triangleHitGroupNames.find(RayType(rayType));
				if (it != triangleHitGroupNames.end()) {
					hitGroup->SetHitGroupExport(it->second);
					hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);
				}

			}
		}

		// AABB geometry hit groups
		{
			// Create hit groups for each intersection shader.
			auto& intersectionHitShaderNames = m_Scene->GetIntersectionShaderNames();
			auto& aabbHitGroupNames = m_Scene->GetAABBHitGroupNames();

			for (UINT t = 0; t < SDFScene::IntersectionShaderType::Count_IntersectionShaderType; t++)
				for (UINT rayType = 0; rayType < RayType::Count; rayType++)
				{
					auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
					GE_ASSERT(intersectionHitShaderNames.find(SDFScene::IntersectionShaderType(t)) != intersectionHitShaderNames.end(), "cannot find intersetion shader name");
					hitGroup->SetIntersectionShaderImport(intersectionHitShaderNames[SDFScene::IntersectionShaderType(t)]);

					if (rayType == RayType::Radiance)
					{
						GE_ASSERT(closestHitShaderNames.find(SDFScene::GeometryType::AABB) != closestHitShaderNames.end(), "cannot find aabb closesthit shader name");
						hitGroup->SetClosestHitShaderImport(closestHitShaderNames[SDFScene::GeometryType::AABB]);

					}
					GE_ASSERT(aabbHitGroupNames[SDFScene::IntersectionShaderType(t)].find(RayType(rayType)) != aabbHitGroupNames[SDFScene::IntersectionShaderType(t)].end(), "cannot find aabb closesthit shader name");
					hitGroup->SetHitGroupExport(aabbHitGroupNames[SDFScene::IntersectionShaderType(t)][RayType(rayType)]);
					hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE);
				}
		}

		// Shader config
		// Defines the maximum sizes in bytes for the ray rayPayload and attribute structure.
		auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		UINT payloadSize = static_cast<UINT>(max(sizeof(RayPayload), sizeof(ShadowRayPayload)));
		UINT attributeSize = sizeof(struct ProceduralPrimitiveAttributes); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);

		CreateRootSignatureSubobjects(&raytracingPipeline);

		// Pipeline config
		// Defines the maximum TraceRay() recursion depth.
		auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed
		// as drivers may apply optimization strategies for low recursion depths.
		UINT maxRecursionDepth = Configuration::MaxRayRecursionDepth;
		pipelineConfig->Config(maxRecursionDepth);

		PrintStateObjectDesc(raytracingPipeline);

		// Create the state object.
		ThrowIfFailed(m_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject)), L"Couldn't create DirectX Raytracing state object.\n");
	}

	void D3D12RayTracingModelRenderer::CreateDescriptorHeap()
	{
		ID3D12Device* device = m_deviceResources->GetD3DDevice();
		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		// Allocate a heap for 6 descriptors:
		// 2 - vertex and index  buffer SRVs
		// 1 - raytracing output texture SRV
		descriptorHeapDesc.NumDescriptors = 3;
		descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = 0;
		device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_DescriptorHeap));
		NAME_D3D12_OBJECT(m_DescriptorHeap);

		m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	}

	void D3D12RayTracingModelRenderer::CreateRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		CreateLocalRootSignatureSubobjects(raytracingPipeline);
		CreateGlobalRootSignatureSubobjects(raytracingPipeline);
	}
	void D3D12RayTracingModelRenderer::CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		//Hit group
		using LocalRootSignatureType = SDFScene::GeometryType;

		//Triangle geometry
		{
			auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(m_LocalRootSignatures[LocalRootSignatureType::Triangle].Get());
			//shader association
			auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			const wchar_t* triangleHitGroupName[] = {
				 m_Scene->GetTriangleHitGroupNames()[RayType::Radiance],
				 m_Scene->GetTriangleHitGroupNames()[RayType::Shadow]
			};
			rootSignatureAssociation->AddExports(triangleHitGroupName);
		}
		// AABB geometry
		{
			auto localRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_LOCAL_ROOT_SIGNATURE_SUBOBJECT>();
			localRootSignature->SetRootSignature(m_LocalRootSignatures[LocalRootSignatureType::AABB].Get());
			// Shader association
			auto rootSignatureAssociation = raytracingPipeline->CreateSubobject<CD3DX12_SUBOBJECT_TO_EXPORTS_ASSOCIATION_SUBOBJECT>();
			rootSignatureAssociation->SetSubobjectToAssociate(*localRootSignature);
			for (int i = 0; i < SDFScene::IntersectionShaderType::Count_IntersectionShaderType; i++)
			{
				const wchar_t* hitGroupsForIntersectionShaderType[] = {
					m_Scene->GetAABBHitGroupNames()[(SDFScene::IntersectionShaderType)(i)][RayType::Radiance],
					m_Scene->GetAABBHitGroupNames()[(SDFScene::IntersectionShaderType)(i)][RayType::Shadow]
				};
				rootSignatureAssociation->AddExports(hitGroupsForIntersectionShaderType);
			}
		}
	}
	void D3D12RayTracingModelRenderer::CreateGlobalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline)
	{
		// Global root signature
	// This is a root signature that is shared across all raytracing shaders invoked during a DispatchRays() call.
		auto globalRootSignature = raytracingPipeline->CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		globalRootSignature->SetRootSignature(m_GlobalRootSignature.Get());

	}
	void D3D12RayTracingModelRenderer::CreateShaderGlobalBuffers()
	{
		//CreateConstantBuffers();
		auto device = m_deviceResources->GetD3DDevice();
		auto frameCount = m_deviceResources->GetBackBufferCount();

		m_gSceneCB.Create(device, frameCount, L"Scene Constant Buffer");

		// Create AABB primitive attribute buffers.
		m_gAabbPrimitiveAttributeBuffer.Create(device, m_Scene->GetTotalPrimitiveCount(), frameCount, L"AABB primitive attributes");

	}
	void D3D12RayTracingModelRenderer::DoRayTracing()
	{
		auto commandList = m_deviceResources->GetCommandList();
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		auto DispatchRays = [&](auto* raytracingCommandList, auto* stateObject, auto* dispatchDesc) {
			dispatchDesc->HitGroupTable.StartAddress = m_HitGroupShaderTable->GetGPUVirtualAddress();
			dispatchDesc->HitGroupTable.SizeInBytes = m_HitGroupShaderTable->GetDesc().Width;
			dispatchDesc->HitGroupTable.StrideInBytes = m_HitGroupShaderTableStrideInBytes;
			dispatchDesc->MissShaderTable.StartAddress = m_MissShaderTable->GetGPUVirtualAddress();
			dispatchDesc->MissShaderTable.SizeInBytes = m_MissShaderTable->GetDesc().Width;
			dispatchDesc->MissShaderTable.StrideInBytes = m_MissShaderTableStrideInBytes;
			dispatchDesc->RayGenerationShaderRecord.StartAddress = m_RayGenShaderTable->GetGPUVirtualAddress();
			dispatchDesc->RayGenerationShaderRecord.SizeInBytes = m_RayGenShaderTable->GetDesc().Width;
			dispatchDesc->Width = m_Width;
			dispatchDesc->Height = m_Height;
			dispatchDesc->Depth = 1;
			raytracingCommandList->SetPipelineState1(stateObject);

			//	m_gpuTimers[GpuTimers::Raytracing].Start(commandList);
			raytracingCommandList->DispatchRays(dispatchDesc);
			//	m_gpuTimers[GpuTimers::Raytracing].Stop(commandList);
		};

		//Bind the heaps, acceleration structure and dispatch rays

		//Set index and successive vertex buffer descriptor table

		SetCommonPipelineState();
		
		D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};
		DispatchRays(m_dxrCommandList.Get(), m_dxrStateObject.Get(), &dispatchDesc);


	}

	void D3D12RayTracingModelRenderer::CopyRaytracingOutputToBackbuffer()
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

	//TODO:: ��ô��scene�� DoRaytracing ����

	void D3D12RayTracingModelRenderer::SetCommonPipelineState()
	{
		UINT vertexBuffers_slot;
		UINT outputView_slot;
		UINT aabbAttributeBuffer_slot;
		UINT accelerationStructure_slot;
		UINT sceneConstant_slot;

		switch (m_Scene->GetDemoType())
		{
		case Scene::MetaBallSDFScene:
			vertexBuffers_slot = MetaBallSDFScene::GlobalRootSignature::VertexBuffers;
			outputView_slot = MetaBallSDFScene::GlobalRootSignature::OutputView;
			aabbAttributeBuffer_slot = MetaBallSDFScene::GlobalRootSignature::AABBattributeBuffer;
			accelerationStructure_slot = MetaBallSDFScene::GlobalRootSignature::AccelerationStructure;
			sceneConstant_slot = MetaBallSDFScene::GlobalRootSignature::SceneConstant;
			break;
		case Scene::SDFScene:
			break;
		default:
			GE_CORE_ERROR("no such scene type");
			break;
		}

		auto commandList = m_deviceResources->GetCommandList();
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		//Sets the layout of the compute root signature.
		commandList->SetComputeRootSignature(m_GlobalRootSignature.Get());
		
		// Copy dynamic buffers to GPU.
		{
			m_gSceneCB.CopyStagingToGpu(frameIndex);
			commandList->SetComputeRootConstantBufferView(MetaBallSDFScene::GlobalRootSignature::SceneConstant, m_gSceneCB.GpuVirtualAddress(frameIndex));

			m_gAabbPrimitiveAttributeBuffer.CopyStagingToGpu(frameIndex);
			commandList->SetComputeRootShaderResourceView(MetaBallSDFScene::GlobalRootSignature::AABBattributeBuffer, m_gAabbPrimitiveAttributeBuffer.GpuVirtualAddress(frameIndex));
		}
		

		commandList->SetDescriptorHeaps(1, m_DescriptorHeap.GetAddressOf());
		// Set index and successive vertex buffer decriptor tables.
		commandList->SetComputeRootDescriptorTable(MetaBallSDFScene::GlobalRootSignature::VertexBuffers, m_indexBuffer.gpuDescriptorHandle);
		commandList->SetComputeRootDescriptorTable(MetaBallSDFScene::GlobalRootSignature::OutputView, m_raytracingOutputResourceUAVGpuDescriptor);

		//Sets a CPU descriptor handle for the shader resource in the compute root signature.
		commandList->SetComputeRootShaderResourceView(MetaBallSDFScene::GlobalRootSignature::AccelerationStructure, m_topLevelAS->GetGPUVirtualAddress());

	}
	void D3D12RayTracingModelRenderer::UpdateAABBPrimitiveAttributes(float animationTime)
	{
		auto frameIndex = m_deviceResources->GetCurrentFrameIndex();

		XMMATRIX mIdentity = XMMatrixIdentity();

		XMMATRIX mScale15y = XMMatrixScaling(1, 1.5, 1);
		XMMATRIX mScale15 = XMMatrixScaling(1.5, 1.5, 1.5);
		XMMATRIX mScale2 = XMMatrixScaling(2, 2, 2);
		XMMATRIX mScale3 = XMMatrixScaling(3, 3, 3);

		XMMATRIX mRotation = XMMatrixRotationY(-2 * animationTime);

		// Apply scale, rotation and translation transforms.
		// The intersection shader tests in this sample work with local space, so here
		// we apply the BLAS object space translation that was passed to geometry descs.
		auto SetTransformForAABB = [&](UINT primitiveIndex, XMMATRIX& mScale, XMMATRIX& mRotation)
		{
			//��object�ƶ��� aabb�е�����
			XMVECTOR vTranslation =
				0.5f * (XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&m_Scene->GetAABBsGeometry()[primitiveIndex].MinX))
					+ XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&m_Scene->GetAABBsGeometry()[primitiveIndex].MaxX)));
			XMMATRIX mTranslation = XMMatrixTranslationFromVector(vTranslation);

			//XMMATRIX mTransform = mScale * mRotation * mTranslation;
			XMMATRIX mTransform = mScale * mRotation* mTranslation;

			m_gAabbPrimitiveAttributeBuffer[primitiveIndex].localSpaceToBottomLevelAS = mTransform;
			m_gAabbPrimitiveAttributeBuffer[primitiveIndex].bottomLevelASToLocalSpace = XMMatrixInverse(nullptr, mTransform);
		};

		UINT offset = 0;
		// Analytic primitives.
		{
			SetTransformForAABB(offset + MetaBallSDFScene::AnalyticPrimitive::AABB, mScale15y, mIdentity);
			SetTransformForAABB(offset + MetaBallSDFScene::AnalyticPrimitive::Spheres, mScale15, mRotation);
			offset += MetaBallSDFScene::AnalyticPrimitive::Count_AnalyticPrimitive;
		}

		// Volumetric primitives.
		{
			SetTransformForAABB(offset + MetaBallSDFScene::VolumetricPrimitive::Metaballs, mScale15, mRotation);
			offset += MetaBallSDFScene::VolumetricPrimitive::Count_VolumetricPrimitive;
		}

		// Signed distance primitives.
		{
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::MiniSpheres, mIdentity, mIdentity);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::IntersectedRoundCube, mIdentity, mIdentity);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::SquareTorus, mScale15, mIdentity);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::TwistedTorus, mIdentity, mRotation);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::Cog, mIdentity, mRotation);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::Cylinder, mScale15y, mIdentity);
			SetTransformForAABB(offset + MetaBallSDFScene::SignedDistancePrimitive::FractalPyramid, mScale3, mIdentity);
		}
	}
}
#endif