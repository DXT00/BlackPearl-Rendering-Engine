#pragma once
#ifdef GE_API_D3D12
#include <d3d12.h>
#include "D3D12Renderer.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
#include "BlackPearl/Scene/SDFScene.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/ConstantBuffer.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/StructuredBuffer.h"
#include "BlackPearl/MainCamera/MainCamera.h"
#include "BlackPearl/Component/CameraComponent/Camera.h"
#include "BlackPearl/Component/LightComponent/LightSources.h"


#include <DirectXMath.h>
using namespace DX;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace BlackPearl {

	class D3D12RayTracingModelRenderer: public D3D12Renderer, IDeviceNotify
	{
	public:
		D3D12RayTracingModelRenderer();
		~D3D12RayTracingModelRenderer();
		void Init();
		void InitScene(SDFScene* scene);
		void Render(const Camera& camera, const LightSources& lights, float elapsedTime);

		virtual void OnDeviceLost() override;
		virtual void OnDeviceRestored() override;

		

	private:
		void CreateDeviceDependentResources();
		void CreateRaytracingOutputResource();
		void UpdateCameraMatrices(const Camera& camera);
		//void UpdateCameraMatrices();

		void UpdateLightSources(const LightSources& lightSources);
		void UpdateElapsedTime(float timeSecond);
		void ReleaseResources();


		void SerializeAndCreateRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig);
		
		void BuildGeometry();
		void BuildProceduralGeometryAABBs();
		void BuildPlaneGeometry();

		//accelerate stuctures
		using BottomLevelASType = SDFScene::GeometryType;
		void BuildAccelerationStructures();
		void BuildGeometryDescsForBottomLevelAS(std::array<std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>, BottomLevelASType::Count>& geometryDescs);
		AccelerationStructureBuffers BuildBottomLevelAS(const std::vector<D3D12_RAYTRACING_GEOMETRY_DESC>& geometryDesc, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE);
		AccelerationStructureBuffers BuildTopLevelAS(AccelerationStructureBuffers bottomLevelAS[BottomLevelASType::Count], D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE);
		template <class InstanceDescType, class BLASPtrType>
		void BuildBotomLevelASInstanceDescs(BLASPtrType* bottomLevelASaddresses, ComPtr<ID3D12Resource>* instanceDescsResource);

		void BuildShaderTables();

		// initial raytracing pipline
		void CreateRaytracingPipelineStateObject();
		void CreateDescriptorHeap();
		void CreateRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
		void CreateGlobalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);

		void CreateShaderGlobalBuffers();



		void DoRayTracing();
		void CopyRaytracingOutputToBackbuffer();
		void SetCommonPipelineState();

		void UpdateAABBPrimitiveAttributes(float animationTime);

	private:
		XMVECTOR m_eye;
		XMVECTOR m_at;
		XMVECTOR m_up;

		int m_FrameCount;
		// Constants.
		const UINT NUM_BLAS = 2;          // Triangle + AABB bottom-level AS.

		// D3D device resources
		int m_adapterIDoverride;
		std::unique_ptr<DX::DeviceResources> m_deviceResources;

		//DirectX Raytracing (DXR) attributes
		ComPtr<ID3D12Device5> m_dxrDevice;
		ComPtr<ID3D12GraphicsCommandList5> m_dxrCommandList;
		ComPtr<ID3D12StateObject> m_dxrStateObject;

		//root signature
		ComPtr<ID3D12RootSignature> m_GlobalRootSignature;
		std::vector<ComPtr<ID3D12RootSignature>> m_LocalRootSignatures;

		// Descriptors
		ComPtr<ID3D12DescriptorHeap> m_DescriptorHeap;
		UINT m_DescriptorsAllocated;
		UINT m_DescriptorSize;

		//Geometry
		// for aabbs
		D3DBuffer m_aabbBuffer;
		// for plane
		D3DBuffer m_indexBuffer;
		D3DBuffer m_vertexBuffer;


		typedef UINT16 Index;
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT3 normal;
		};


		 // Raytracing scene
		// Dynamic attributes per primitive instance.
		struct PrimitiveInstancePerFrameBuffer
		{
			XMMATRIX localSpaceToBottomLevelAS;   // Matrix from local primitive space to bottom-level object space.
			XMMATRIX bottomLevelASToLocalSpace;   // Matrix from bottom-level object space to local primitive space.
		};

		// Raytracing scene, connect to shader -- g_SceneCB, g_AABBPrimitiveAttributes
		ConstantBuffer<SceneConstantBuffer> m_gSceneCB;
		StructuredBuffer<PrimitiveInstancePerFrameBuffer> m_gAabbPrimitiveAttributeBuffer;

		// Raytracing output
		ComPtr<ID3D12Resource> m_raytracingOutput;
		D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
		UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;

		//Shader Tables
		ComPtr<ID3D12Resource> m_MissShaderTable;
		ComPtr<ID3D12Resource> m_RayGenShaderTable;
		ComPtr<ID3D12Resource> m_HitGroupShaderTable;
		UINT m_MissShaderTableStrideInBytes;
		UINT m_HitGroupShaderTableStrideInBytes;

		// Acceleration structure
		ComPtr<ID3D12Resource> m_bottomLevelAS[BottomLevelASType::Count];
		ComPtr<ID3D12Resource> m_topLevelAS;

		SDFScene* m_Scene;


	};

}

#endif