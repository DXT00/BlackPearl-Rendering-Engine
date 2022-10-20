#pragma once
#include "D3D12Renderer.h"
#include "BasicRenderer.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
#include "BlackPearl/Scene/Scene.h"
#include <DirectXMath.h>
using namespace DX;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace BlackPearl {
	class D3D12MeshShaderRenderer : public BasicRenderer, D3D12Renderer, IDeviceNotify
	{
	public:
		D3D12MeshShaderRenderer();
		~D3D12MeshShaderRenderer();
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();
		
		void Render(const Camera& camera);

	private:
		void Init(Scene* scene);
		void LoadPipeline();
		void LoadAssets();
		void UploadObj(Object* obj);
		void UploadModel(Object* model);
		
		void WaitForGpu();
		void LoadScene(Scene* scene);
		void PreCheck();
		ComPtr<ID3D12CommandQueue>	 CreateCommandQueue(ComPtr<ID3D12Device> device);
		ComPtr<IDXGISwapChain3>		 CreateSwapChan(ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue, int bufferCount, int bufferWidth, int bufferHeight);
		ComPtr<ID3D12DescriptorHeap> CreateRtvHeap(ComPtr<ID3D12Device> device, int numDescriptors);
		ComPtr<ID3D12DescriptorHeap> CreateDsvHeap(ComPtr<ID3D12Device> device, int numDescriptors);
		void CreateFence();

		void CreateRenderTargetView();
		void CreateDepthStencilView();
		void CreateConstantBufferView();
		void UpdateCameraMatrices(const Camera& camera);
		void GetHardwareAdapter(
			IDXGIFactory1* pFactory,
			IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		void PopulateCommandList();
		std::wstring GetAssetFullPath(LPCWSTR assetName);
		void MoveToNextFrame();

	private:
		_declspec(align(256u)) struct SceneConstantBuffer
		{
			XMFLOAT4X4 World;
			XMFLOAT4X4 WorldView;
			XMFLOAT4X4 WorldViewProj;
			uint32_t   DrawMeshlets;
		};
		std::wstring m_BasicShaderPath;
		static const UINT m_FrameCount = 2;

		// Pipeline objects.
		ComPtr<ID3D12Device2> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<IDXGISwapChain3> m_SwapChain;
		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

		UINT m_RtvDescriptorSize;
		ComPtr<ID3D12Resource> m_RenderTargets[m_FrameCount];
		ComPtr<ID3D12Resource> m_DepthStencil;
		ComPtr<ID3D12Resource> m_ConstantBuffer;
		SceneConstantBuffer m_ConstantBufferData;

		UINT8* m_CbvDataBegin;


		ComPtr<ID3D12CommandAllocator> m_CommandAllocators[m_FrameCount];
		ComPtr<ID3D12PipelineState> m_PipelineState;
		ComPtr<ID3D12GraphicsCommandList6> m_CommandList;
		ComPtr<ID3D12RootSignature> m_RootSignature;

		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;

		// Synchronization objects.
		UINT m_FrameIndex;
		UINT m_FrameCounter;
		HANDLE m_FenceEvent;
		ComPtr<ID3D12Fence> m_Fence;
		UINT64 m_FenceValues[m_FrameCount];

	private:
		static const wchar_t* c_meshFilename;
		static const wchar_t* c_meshShaderFilename;
		static const wchar_t* c_pixelShaderFilename;
	private:
		Scene* m_Scene;
	};

}

