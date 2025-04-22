#pragma once
#include "BasicRenderer.h"
#include <d3d12.h>
#include "BlackPearl/RHI/D3D12RHI/DirectXRaytracingHelper.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer/D3D12Buffer.h"
#include "BlackPearl/Renderer/MasterRenderer/D3D12Renderer.h"
using namespace DX;

namespace BlackPearl {

	class D3D12RayTracingRenderer :public BasicRenderer, D3D12Renderer, IDeviceNotify
	{
	public:
		D3D12RayTracingRenderer();
        ~D3D12RayTracingRenderer();
        //override IDeviceNotify
        virtual void OnDeviceLost() override;
        virtual void OnDeviceRestored() override;

        void Render();
        virtual void OnSizeChanged(UINT width, UINT height, bool minimized);
        virtual void OnDestroy();
        virtual IDXGISwapChain* GetSwapchain() { return m_deviceResources->GetSwapChain(); }
        virtual void UpdateForSizeChange(int clientWidth, int clientHeight) override;

    public:
        enum class GlobalRootSignatureParams {
            OutputViewSlot = 0,
            AccelerationStructureSlot,
            Count
        };

        enum class LocalRootSignatureParams {
            ViewportConstantSlot = 0,
            Count
        };
	private:
        void Init();
        void RecreateD3D();
        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();
        void ReleaseWindowSizeDependentResources();

        void DoRaytracing();

        void CreateRaytracingInterfaces();
        void SerializeAndCreateRaytracingRootSignature(D3D12_ROOT_SIGNATURE_DESC& desc, ComPtr<ID3D12RootSignature>* rootSig);
        void CreateRootSignatures();
        void CreateLocalRootSignatureSubobjects(CD3DX12_STATE_OBJECT_DESC* raytracingPipeline);
        void CreateRaytracingPipelineStateObject();
        void CreateDescriptorHeap();
        void CreateRaytracingOutputResource();

        void BuildGeometry();
        void BuildAccelerationStructures();
        void BuildShaderTables();

        void CopyRaytracingOutputToBackbuffer();
        void CalculateFrameStats();
        UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);

        // D3D device resources
		static const UINT m_FrameCount = 3;

		UINT m_adapterIDoverride;
		std::unique_ptr<DX::DeviceResources> m_deviceResources;

        // Descriptors
        ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
        UINT m_descriptorsAllocated = 0;
        UINT m_descriptorSize;

        // Raytracing scene
        RayGenConstantBuffer m_rayGenCB;

        // DirectX Raytracing (DXR) attributes
        ComPtr<ID3D12Device5> m_dxrDevice;
        ComPtr<ID3D12GraphicsCommandList4> m_dxrCommandList;
        ComPtr<ID3D12StateObject> m_dxrStateObject;

        // Root signatures
        ComPtr<ID3D12RootSignature> m_raytracingGlobalRootSignature;
        ComPtr<ID3D12RootSignature> m_raytracingLocalRootSignature;

        // Shader tables
        static const wchar_t* c_hitGroupName;
        static const wchar_t* c_raygenShaderName;
        static const wchar_t* c_closestHitShaderName;
        static const wchar_t* c_missShaderName;
        ComPtr<ID3D12Resource> m_missShaderTable;
        ComPtr<ID3D12Resource> m_hitGroupShaderTable;
        ComPtr<ID3D12Resource> m_rayGenShaderTable;

        // Application state
        // StepTimer m_timer;

        // Geometry
        typedef UINT16 Index;
        struct Vertex { float v1, v2, v3; };
        ComPtr<ID3D12Resource> m_indexBuffer;
        ComPtr<ID3D12Resource> m_vertexBuffer;

        // Acceleration structure
        ComPtr<ID3D12Resource> m_accelerationStructure;
        ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
        ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

        // Raytracing output
        ComPtr<ID3D12Resource> m_raytracingOutput;
        D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
        UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;

      
	};

}
