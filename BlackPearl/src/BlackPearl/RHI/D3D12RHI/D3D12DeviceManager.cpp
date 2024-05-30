#include "pch.h"
#include "BlackPearl/Renderer/DeviceManager.h"
#include "BlackPearl/RHI/RHITexture.h"
#include "BlackPearl/RHI/DynamicRHI.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include "BlackPearl/RHI/D3D12RHI/d3dx12.h"
#include <dxgidebug.h>

namespace BlackPearl {
    class D3D12DeviceManager : public DeviceManager
    {
        RefCountPtr<ID3D12Device>                   m_Device12;
        RefCountPtr<ID3D12CommandQueue>             m_GraphicsQueue;
        RefCountPtr<ID3D12CommandQueue>             m_ComputeQueue;
        RefCountPtr<ID3D12CommandQueue>             m_CopyQueue;
        RefCountPtr<IDXGISwapChain3>                m_SwapChain;
        UINT                                        m_FrameBufferIndexFake = 0;
        DXGI_SWAP_CHAIN_DESC1                       m_SwapChainDesc{};
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC             m_FullScreenDesc{};
        RefCountPtr<IDXGIAdapter>                   m_DxgiAdapter;
        HWND                                        m_hWnd = nullptr;
        bool                                        m_TearingSupported = false;

        std::vector<RefCountPtr<ID3D12Resource>>    m_SwapChainBuffers;
        std::vector<TextureHandle>           m_RhiSwapChainBuffers;
        RefCountPtr<ID3D12Fence>                    m_FrameFence;
        std::vector<HANDLE>                         m_FrameFenceEvents;

        UINT64                                      m_FrameCount = 1;

       DeviceHandle                         m_NvrhiDevice;

        std::string                                 m_RendererString;

    public:
        [[nodiscard]] const char* GetRendererString() const override
        {
            return m_RendererString.c_str();
        }

        [[nodiscard]] IDevice* GetDevice() const override
        {
            return m_NvrhiDevice;
        }

        void BeginFrame() override;

        void ReportLiveObjects() override;

        [[nodiscard]] DynamicRHI::Type GetGraphicsAPI() const override
        {
            return DynamicRHI::Type::D3D12;
        }
    protected:
        bool CreateDeviceAndSwapChain() override;
        void DestroyDeviceAndSwapChain() override;
        void ResizeSwapChain() override;

        ITexture* GetCurrentBackBuffer() override
        {
            if (m_SwapChain)
                return m_RhiSwapChainBuffers[m_SwapChain->GetCurrentBackBufferIndex()];
            return m_RhiSwapChainBuffers[m_FrameBufferIndexFake];
        }

        ITexture* GetBackBuffer(uint32_t index) override
        {
            if (index < m_RhiSwapChainBuffers.size())
                return m_RhiSwapChainBuffers[index];
            return nullptr;
        }

        uint32_t GetCurrentBackBufferIndex() override
        {
            return 0;
        }

        uint32_t GetBackBufferCount() override
        {
            return m_SwapChainDesc.BufferCount;
        }

        void Present() override;


    private:
        bool CreateRenderTarget();
        void ReleaseRenderTarget();
    };

    bool D3D12DeviceManager::CreateDeviceAndSwapChain()
    {
        return false;
    }

    void D3D12DeviceManager::DestroyDeviceAndSwapChain()
    {
    }

    void D3D12DeviceManager::ResizeSwapChain()
    {
    }

    void D3D12DeviceManager::Present()
    {
    }

    bool D3D12DeviceManager::CreateRenderTarget()
    {
        return false;
    }

    void D3D12DeviceManager::ReleaseRenderTarget()
    {
    }

    void D3D12DeviceManager::BeginFrame()
    {
    }

    void D3D12DeviceManager::ReportLiveObjects()
    {
    }

    DeviceManager* DeviceManager::CreateD3D12()
    {
        return nullptr;
    }


}