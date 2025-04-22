#pragma once
#include "D3D12Renderer.h"
#include "BasicRenderer.h"
#include "BlackPearl/RHI/D3D12RHI/D3D12DeviceResources.h"
#include <DirectXMath.h>
using namespace DX;
using Microsoft::WRL::ComPtr;
using namespace DirectX;

namespace BlackPearl {
	class D3D12BasicRenderer : public BasicRenderer, D3D12Renderer, IDeviceNotify
	{
	public:
		D3D12BasicRenderer();
		~D3D12BasicRenderer();
		virtual void OnDeviceLost();
		virtual void OnDeviceRestored();
		void Init();
		void LoadPipline();
		void LoadAsset();
		void GetHardwareAdapter(
			IDXGIFactory1* pFactory,
			IDXGIAdapter1** ppAdapter,
			bool requestHighPerformanceAdapter = false);
		void Render();
		void PopulateCommandList();

	private:
		std::wstring GetAssetFullPath(LPCWSTR assetName);
		void WaitForPreviousFrame();
	private:
		struct Vertex {
			XMFLOAT3 pos;
			XMFLOAT4 albedo;
		};

		static const UINT m_FrameCount = 2;

		// Pipeline objects.
		ComPtr<ID3D12Device> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<IDXGISwapChain3> m_SwapChain;
		ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		UINT m_RtvDescriptorSize;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		ComPtr<ID3D12PipelineState> m_PipelineState;
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12Resource> m_RenderTargets[m_FrameCount];
		ComPtr<ID3D12RootSignature> m_RootSignature;

		CD3DX12_VIEWPORT m_Viewport;
		CD3DX12_RECT m_ScissorRect;

		// App resources.
		ComPtr<ID3D12Resource> m_VertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView;

	
		// Synchronization objects.
		UINT m_FrameIndex;
		HANDLE m_FenceEvent;
		ComPtr<ID3D12Fence> m_Fence;
		UINT64 m_FenceValue;

		std::wstring m_BasicShaderPath;

	};
}


