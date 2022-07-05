#pragma once
#include <d3d12.h>
#include "BasicRenderer.h"
#include "D3D12Renderer.h"
#include "BlackPearl/RHI/D3D12RHI/DirectXRaytracingHelper.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include "BlackPearl/Renderer/Buffer/D3D12Buffer.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace DX;

namespace BlackPearl {
	class D3D12RayTracingCubeRenderer : public BasicRenderer, D3D12Renderer, IDeviceNotify
	{

	public:
		D3D12RayTracingCubeRenderer();
		~D3D12RayTracingCubeRenderer();

        //override IDeviceNotify
        virtual void OnDeviceLost() override;
        virtual void OnDeviceRestored() override;

		void Render(const Camera& camera, Object* pointLight = nullptr);
    public:
        enum class GlobalRootSignatureParams {
            OutputViewSlot = 0,
            AccelerationStructureSlot,
            SceneConstantSlot,
            VertexBuffersSlot,
            Count
        };

        enum class LocalRootSignatureParams {
            CubeConstantSlot = 0,
            Count
        };
	private:
		void Init();

        void InitializeScene();
        void CreateConstantBuffers();

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

        void UpdateForSizeChange(UINT clientWidth, UINT clientHeight);
        void CopyRaytracingOutputToBackbuffer();
        void CalculateFrameStats();

        UINT AllocateDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptor, UINT descriptorIndexToUse = UINT_MAX);
        UINT CreateBufferSRV(D3DBuffer* buffer, UINT numElements, UINT elementSize);

        void UpdateCameraMatrices();
        typedef UINT16 Index;

        struct Vertex
        {
            XMFLOAT3 position;
            XMFLOAT3 normal;
        };

        // D3D device resources
        static const UINT m_FrameCount = 3;

        // We'll allocate space for several of these and they will need to be padded for alignment.
        static_assert(sizeof(SceneConstantBuffer) < D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT, "Checking the size here.");

        union AlignedSceneConstantBuffer
        {
            SceneConstantBuffer constants;
            uint8_t alignmentPadding[D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT];
        };
        AlignedSceneConstantBuffer* m_mappedConstantData;
        ComPtr<ID3D12Resource>       m_perFrameConstants;

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
        // Application state
       // StepTimer m_timer;
        float m_curRotationAngleRad;
        XMVECTOR m_eye;
        XMVECTOR m_at;
        XMVECTOR m_up;
        float m_rotate;

        // Geometry
        D3DBuffer m_indexBuffer;
        D3DBuffer m_vertexBuffer;

        // Acceleration structure
        ComPtr<ID3D12Resource> m_accelerationStructure;
        ComPtr<ID3D12Resource> m_bottomLevelAccelerationStructure;
        ComPtr<ID3D12Resource> m_topLevelAccelerationStructure;

        // Raytracing output
        ComPtr<ID3D12Resource> m_raytracingOutput;
        D3D12_GPU_DESCRIPTOR_HANDLE m_raytracingOutputResourceUAVGpuDescriptor;
        UINT m_raytracingOutputResourceUAVDescriptorHeapIndex;

        // Raytracing scene
        SceneConstantBuffer m_sceneCB[m_FrameCount];
        CubeConstantBuffer m_cubeCB;

        UINT m_width;
        UINT m_height;
        float m_aspectRatio;

	};
}


