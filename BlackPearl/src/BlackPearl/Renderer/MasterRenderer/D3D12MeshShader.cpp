#include "pch.h"
#include "D3D12MeshShader.h"
#include "BlackPearl/Application.h"
#include "BlackPearl/Common/CommonFunc.h"
namespace BlackPearl {
	D3D12MeshShader::D3D12MeshShader()
		:D3D12Renderer(Application::Get().GetWindow().GetWidth(), Application::Get().GetWindow().GetHeight()),
        m_FrameIndex(0)
	{
	}
	D3D12MeshShader::~D3D12MeshShader()
	{
	}
	void D3D12MeshShader::OnDeviceLost()
	{
	}
	void D3D12MeshShader::OnDeviceRestored()
	{
	}
	void D3D12MeshShader::Init()
	{
		LoadPipeline();
		LoadAssets();
	}

	void D3D12MeshShader::LoadPipeline()
	{
        UINT dxgiFactoryFlags = 0;

        ComPtr<IDXGIFactory4> factory;
        ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
        D3D12CreateDevice(hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_Device)
        );

        m_CommandQueue = CreateCommandQueue(m_Device);
       
        m_SwapChain = CreateSwapChan(factory, m_CommandQueue, m_FrameCount, m_Width, m_Height);
        m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
        
        CreateRenderTargetView();
        CreateDepthStencilView();

        ThrowIfFailed(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

	}

	void D3D12MeshShader::LoadAssets()
	{
	}

    void D3D12MeshShader::LoadScene(Scene* scene)
    {
    }

    void D3D12MeshShader::PreCheck()
    {
    }

    ComPtr<ID3D12CommandQueue> D3D12MeshShader::CreateCommandQueue(ComPtr<ID3D12Device> device)
    {
        ComPtr<ID3D12CommandQueue> commadQueue;
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commadQueue));
        return commadQueue;
    }

    ComPtr<IDXGISwapChain3> D3D12MeshShader::CreateSwapChan(ComPtr<IDXGIFactory4> factory, ComPtr<ID3D12CommandQueue> commandQueue, int bufferCount, int bufferWidth, int bufferHeight)
    {
        ComPtr<IDXGISwapChain3> swapChain3;
        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};
        swapchainDesc.BufferCount = bufferCount;
        swapchainDesc.Width = bufferWidth;
        swapchainDesc.Height = bufferHeight;
        swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
            (HWND)(Application::Get().GetWindow().GetNativeWindow()),
            &swapchainDesc,
            nullptr,
            nullptr,
            &swapChain
        ));

        ThrowIfFailed(swapChain.As(&swapChain3));
        return swapChain3;
    }

    ComPtr<ID3D12DescriptorHeap> D3D12MeshShader::CreateRtvHeap(ComPtr<ID3D12Device> device, int numDescriptors)
    {

        ComPtr<ID3D12DescriptorHeap> rtvHeap;
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = numDescriptors;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
        return rtvHeap;
    }

    ComPtr<ID3D12DescriptorHeap> D3D12MeshShader::CreateDsvHeap(ComPtr<ID3D12Device> device, int numDescriptors)
    {
        ComPtr<ID3D12DescriptorHeap> dsvHeap;

        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = numDescriptors;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap)));

        return dsvHeap;
    }

    void D3D12MeshShader::CreateRenderTargetView()
    {
        m_RtvHeap = CreateRtvHeap(m_Device, m_FrameCount);
        m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Create frame resources.
        {
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

            // Create a RTV for each frame.
            for (UINT n = 0; n < m_FrameCount; n++)
            {
                ThrowIfFailed(m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n])));
                m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
                rtvHandle.Offset(1, m_RtvDescriptorSize);
            }
        }
    }

    void D3D12MeshShader::CreateDepthStencilView()
    {
        m_DsvHeap = CreateDsvHeap(m_Device, 1);
        // Create the depth stencil view.
        {
            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
            depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
            depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            const CD3DX12_HEAP_PROPERTIES depthStencilHeapProps(D3D12_HEAP_TYPE_DEFAULT);
            const CD3DX12_RESOURCE_DESC depthStencilTextureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_Width, m_Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

            ThrowIfFailed(m_Device->CreateCommittedResource(
                &depthStencilHeapProps,
                D3D12_HEAP_FLAG_NONE,
                &depthStencilTextureDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &depthOptimizedClearValue,
                IID_PPV_ARGS(&m_DepthStencil)
            ));

            NAME_D3D12_OBJECT(m_DepthStencil);

            m_Device->CreateDepthStencilView(m_DepthStencil.Get(), &depthStencilDesc, m_DsvHeap->GetCPUDescriptorHandleForHeapStart());
        }
    }

    void D3D12MeshShader::CreateConstantBufferView()
    {
        const UINT64 constantBufferSize = sizeof(SceneConstantBuffer) * m_FrameCount;
        const CD3DX12_HEAP_PROPERTIES constantBufferHeapProps(D3D12_HEAP_TYPE_UPLOAD);
        const CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);

        ThrowIfFailed(m_Device->CreateCommittedResource(
            &constantBufferHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &constantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_ConstantBuffer)));

        // Describe and create a constant buffer view.
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = constantBufferSize;

        // Map and initialize the constant buffer. We don't unmap this until the
        // app closes. Keeping things mapped for the lifetime of the resource is okay.
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_ConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_CbvDataBegin)));

    }

	void D3D12MeshShader::GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter, bool requestHighPerformanceAdapter)
	{
	}
	void D3D12MeshShader::Render()
	{
	}
	void D3D12MeshShader::PopulateCommandList()
	{
	}
}