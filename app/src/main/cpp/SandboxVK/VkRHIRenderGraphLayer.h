#pragma once
#pragma once
#include <BlackPearl.h>

class VkRHIRenderGraphLayer :public BlackPearl::Layer {
public:

	VkRHIRenderGraphLayer(const std::string& name)
		: Layer(name)
	{

        ForwordRenderGraph = DBG_NEW BlackPearl::RenderGraph(m_DeviceManager);

        BlackPearl::DeviceCreationParameters deviceParams = _InitDeviceParms();

        if (!m_DeviceManager->CreateDeviceAndSwapChain(deviceParams)) {
            throw std::runtime_error("failed to create device and swapchain!");

        }
        m_DeviceManager->AddRenderGraphToFront(ForwordRenderGraph);

	}

	virtual ~VkRHIRenderGraphLayer() {

		DestroyObjects();
        m_DeviceManager->Shutdown();
	}
	void OnUpdate(BlackPearl::Timestep ts) override {

		BlackPearl::Renderer::BeginScene(*(m_MainCamera->GetObj()->GetComponent<BlackPearl::PerspectiveCamera>()), *GetLightSources());
	
        m_DeviceManager->Run();
    }

	void OnAttach() override {

	}

    BlackPearl::DeviceCreationParameters _InitDeviceParms() 
    {

        BlackPearl::DeviceCreationParameters deviceParams;
        // deviceParams.adapter = VrSystem::GetRequiredAdapter();
        deviceParams.backBufferWidth = 1920;
        deviceParams.backBufferHeight = 1080;
        deviceParams.swapChainSampleCount = 1;
        deviceParams.swapChainBufferCount = BlackPearl::Configuration::SwapchainCount;
        deviceParams.startFullscreen = false;
        deviceParams.vsyncEnabled = true;
        deviceParams.enableRayTracingExtensions = true;
        deviceParams.requireAdapterRaytracingSupport = true;
#if USE_DX11 || USE_DX12
        deviceParams.featureLevel = D3D_FEATURE_LEVEL_12_1;
#endif
#ifdef _DEBUG
        deviceParams.enableDebugRuntime = true;
        deviceParams.enableNvrhiValidationLayer = true;
        deviceParams.enableGPUValidation = false; // <- this severely impact performance but is good to enable from time to time
#endif
#if USE_VK
        deviceParams.requiredVulkanDeviceExtensions.push_back("VK_KHR_buffer_device_address");
        deviceParams.requiredVulkanDeviceExtensions.push_back("VK_KHR_format_feature_flags2");

        // Attachment 0 not written by fragment shader; undefined values will be written to attachment (OMM baker)
        deviceParams.ignoredVulkanValidationMessageLocations.push_back(0x0000000023e43bb7);

        // vertex shader writes to output location 0.0 which is not consumed by fragment shader (OMM baker)
        deviceParams.ignoredVulkanValidationMessageLocations.push_back(0x000000000609a13b);

        // vkCmdPipelineBarrier2(): pDependencyInfo.pBufferMemoryBarriers[0].dstAccessMask bit VK_ACCESS_SHADER_READ_BIT
        // is not supported by stage mask (Unhandled VkPipelineStageFlagBits)
        // Vulkan validation layer not supporting OMM?
        deviceParams.ignoredVulkanValidationMessageLocations.push_back(0x00000000591f70f2);

        // vkCmdPipelineBarrier2(): pDependencyInfo->pBufferMemoryBarriers[0].dstAccessMask(VK_ACCESS_SHADER_READ_BIT) is not supported by stage mask(VK_PIPELINE_STAGE_2_MICROMAP_BUILD_BIT_EXT)
        // Vulkan Validaiotn layer not supporting OMM bug
        deviceParams.ignoredVulkanValidationMessageLocations.push_back(0x000000005e6e827d);
#endif

        deviceParams.enablePerMonitorDPI = true;


	}
private:
	glm::vec4 m_BackgroundColor1 = { 1.0f,1.0f,1.0f,1.0f };

    BlackPearl::RenderGraph* ForwordRenderGraph;


};
#pragma once
