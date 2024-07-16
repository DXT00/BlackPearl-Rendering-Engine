/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/
#pragma once
//#include "RenderGraph.h"
#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "BlackPearl/RHI/RHIDevice.h"
#include "BlackPearl/RHI/RHIDefinitions.h"
#include "BlackPearl/RHI/DynamicRHI.h"

#if GE_API_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#endif

#if GE_API_D3D12
#include <dxgi.h>
#include <d3dcommon.h>
#endif

namespace BlackPearl {
    struct DeviceCreationParameters
    {
        bool startMaximized = false;
        bool startFullscreen = false;
        bool allowModeSwitch = true;
        int windowPosX = -1;            // -1 means use default placement
        int windowPosY = -1;
        uint32_t backBufferWidth = 1280;
        uint32_t backBufferHeight = 720;
        uint32_t refreshRate = 0;
        uint32_t swapChainBufferCount = 2;
        Format swapChainFormat = Format::SRGBA8_UNORM;
        uint32_t swapChainSampleCount = 1;
        uint32_t swapChainSampleQuality = 0;
        uint32_t maxFramesInFlight = 2;
        bool enableDebugRuntime = false;
        bool enableNvrhiValidationLayer = false;
        bool enableGPUValidation = false;
        bool vsyncEnabled = false;
        bool enableRayTracingExtensions = false; // for vulkan
        bool enableComputeQueue = false;
        bool enableCopyQueue = false;
        bool requireAdapterRaytracingSupport = false; // will skip adapters with no raytracing support

        // Severity of the information log messages from the device manager, like the device name or enabled extensions.
       // log::Severity infoLogSeverity = log::Severity::Info;

#if GE_API_D3D12
        // Adapter to create the device on. Setting this to non-null overrides adapterNameSubstring.
        // If device creation fails on the specified adapter, it will *not* try any other adapters.
        IDXGIAdapter* adapter = nullptr;
#endif

        // For use in the case of multiple adapters; only effective if 'adapter' is null. If this is non-null, device creation will try to match
        // the given string against an adapter name.  If the specified string exists as a sub-string of the
        // adapter name, the device and window will be created on that adapter.  Case sensitive.
        std::wstring adapterNameSubstring = L"";

        // set to true to enable DPI scale factors to be computed per monitor
        // this will keep the on-screen window size in pixels constant
        //
        // if set to false, the DPI scale factors will be constant but the system
        // may scale the contents of the window based on DPI
        //
        // note that the backbuffer size is never updated automatically; if the app
        // wishes to scale up rendering based on DPI, then it must set this to true
        // and respond to DPI scale factor changes by resizing the backbuffer explicitly
        bool enablePerMonitorDPI = false;

#if GE_API_D3D12
        DXGI_USAGE swapChainUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
#endif

#if GE_API_VULKAN
        std::vector<std::string> requiredVulkanInstanceExtensions;
        std::vector<std::string> requiredVulkanDeviceExtensions;
        std::vector<std::string> requiredVulkanLayers;
        std::vector<std::string> optionalVulkanInstanceExtensions;
        std::vector<std::string> optionalVulkanDeviceExtensions;
        std::vector<std::string> optionalVulkanLayers;
        std::vector<size_t> ignoredVulkanValidationMessageLocations;
        std::function<void(VkDeviceCreateInfo&)> deviceCreateInfoCallback;
#endif
    };
    class RenderGraph;
	class DeviceManager
	{
    public:
        void Init(const DeviceCreationParameters& params);
        static DeviceManager* Create(DynamicRHI::Type api);

        IFramebuffer* GetFrameBuffer();


        void AddRenderGraphToFront(RenderGraph* pController);
        void AddRenderGraphToBack(RenderGraph* pController);
        void RemoveRenderGraph(RenderGraph* pController);

        void Run();

        // returns the size of the window in screen coordinates
        void GetWindowDimensions(int& width, int& height);
        // returns the screen coordinate to pixel coordinate scale factor
        void GetDPIScaleInfo(float& x, float& y) const
        {
            x = m_DPIScaleFactorX;
            y = m_DPIScaleFactorY;
        }
        virtual void BeginFrame() = 0;
        virtual void Present() = 0;

    protected:
        bool m_windowVisible = false;

        DeviceCreationParameters m_DeviceParams;
        //GLFWwindow* m_Window = nullptr;
        // set to true if running on NV GPU
        bool m_IsNvidia = false;
        std::list<RenderGraph*> m_vRenderGraphs;
        // timestamp in seconds for the previous frame
        double m_PreviousFrameTimestamp = 0.0;
        // current DPI scale info (updated when window moves)
        float m_DPIScaleFactorX = 1.f;
        float m_DPIScaleFactorY = 1.f;
        bool m_RequestedVSync = false;

        double m_AverageFrameTime = 0.0;
        double m_AverageTimeUpdateInterval = 0.5;
        double m_FrameTimeSum = 0.0;
        int m_NumberOfAccumulatedFrames = 0;

        uint32_t m_FrameIndex = 0;

        std::vector<FramebufferHandle> m_SwapChainFramebuffers;

        DeviceManager() = default;

        bool CreateWindowDeviceAndSwapChain(const DeviceCreationParameters& params, const char* windowTitle);
        bool CreateDeviceAndSwapChain(const DeviceCreationParameters& params);


        void UpdateWindowSize();

        void BackBufferResizing();
        void BackBufferResized();

        void Animate(double elapsedTime);
        
        void UpdateAverageFrameTime(double elapsedTime);

        // device-specific methods
        virtual bool CreateDeviceAndSwapChain() = 0;
        virtual void DestroyDeviceAndSwapChain() = 0;
        virtual void ResizeSwapChain() = 0;
    

    public:
        [[nodiscard]] virtual IDevice* GetDevice() const = 0;
        [[nodiscard]] virtual const char* GetRendererString() const = 0;
        [[nodiscard]] virtual DynamicRHI::Type GetGraphicsAPI() const = 0;

        const DeviceCreationParameters& GetDeviceParams();
        [[nodiscard]] double GetAverageFrameTimeSeconds() const { return m_AverageFrameTime; }
        [[nodiscard]] double GetPreviousFrameTimestamp() const { return m_PreviousFrameTimestamp; }
        void SetFrameTimeUpdateInterval(double seconds) { m_AverageTimeUpdateInterval = seconds; }
        [[nodiscard]] bool IsVsyncEnabled() const { return m_DeviceParams.vsyncEnabled; }
        virtual void SetVsyncEnabled(bool enabled) { m_RequestedVSync = enabled; /* will be processed later */ }
        virtual void ReportLiveObjects() {}

        // these are public in order to be called from the GLFW callback functions
        void WindowCloseCallback() { }
        void WindowIconifyCallback(int iconified) { }
        void WindowFocusCallback(int focused) { }
        void WindowRefreshCallback() { }
        void WindowPosCallback(int xpos, int ypos);

        void KeyboardUpdate(int key, int scancode, int action, int mods);
        void KeyboardCharInput(unsigned int unicode, int mods);
        void MousePosUpdate(double xpos, double ypos);
        void MouseButtonUpdate(int button, int action, int mods);
        void MouseScrollUpdate(double xoffset, double yoffset);

        //[[nodiscard]] GLFWwindow* GetWindow() const { return m_Window; }
        [[nodiscard]] uint32_t GetFrameIndex() const { return m_FrameIndex; }

        virtual ITexture* GetCurrentBackBuffer() = 0;
        virtual ITexture* GetBackBuffer(uint32_t index) = 0;
        virtual uint32_t GetCurrentBackBufferIndex() = 0;
        virtual uint32_t GetBackBufferCount() = 0;
        IFramebuffer* GetCurrentFramebuffer();
        IFramebuffer* GetFramebuffer(uint32_t index);

        virtual void Shutdown();
        virtual ~DeviceManager() = default;

        void SetWindowTitle(const char* title);
        void SetInformativeWindowTitle(const char* applicationName, bool includeFramerate, const char* extraInfo/* = nullptr*/);

        virtual bool IsVulkanInstanceExtensionEnabled(const char* extensionName) const { return false; }
        virtual bool IsVulkanDeviceExtensionEnabled(const char* extensionName) const { return false; }
        virtual bool IsVulkanLayerEnabled(const char* layerName) const { return false; }
        virtual void GetEnabledVulkanInstanceExtensions(std::vector<std::string>& extensions) const { }
        virtual void GetEnabledVulkanDeviceExtensions(std::vector<std::string>& extensions) const { }
        virtual void GetEnabledVulkanLayers(std::vector<std::string>& layers) const { }

        struct PipelineCallbacks
        {
            std::function<void(DeviceManager&)> beforeFrame = nullptr;
            std::function<void(DeviceManager&)> beforeAnimate = nullptr;
            std::function<void(DeviceManager&)> afterAnimate = nullptr;
            std::function<void(DeviceManager&)> beforeRender = nullptr;
            std::function<void(DeviceManager&)> afterRender = nullptr;
            std::function<void(DeviceManager&)> beforePresent = nullptr;
            std::function<void(DeviceManager&)> afterPresent = nullptr;
        } m_callbacks;

    private:

        static DeviceManager* CreateD3D12();
        static DeviceManager* CreateVK();
        static DeviceManager* CreateOpenGL();
        std::string m_WindowTitle;
	};

}

