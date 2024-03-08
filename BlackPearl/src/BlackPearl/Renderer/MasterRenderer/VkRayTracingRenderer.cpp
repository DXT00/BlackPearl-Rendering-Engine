#include "pch.h"
#ifdef GE_API_VULKAN
#include "VkRayTracingRenderer.h"
#include "BlackPearl/Renderer/Shader/VkShader/vkShader.h"
#include "BlackPearl/Application.h"
#include "vulkan/vulkan_core.h"
#include <examples/imgui_impl_glfw.cpp>
#include "BlackPearl/Renderer/Buffer/VkBuffer/VkBuffer.h"
#include "BlackPearl/Renderer/Image/VkImage.h"
#include "BlackPearl/RHI/VulkanRHI/VkRenderConfig.h"
#include "BlackPearl/RHI/RHIBindingLayoutDesc.h"
#include "BlackPearl/Log.h"
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include "stb_image.h"
#include <random>

namespace BlackPearl {
    //标志可以并发处理多少帧，允许多个帧同时运行，也就是说，允许一帧的渲染不干扰下一帧的录制
    const uint32_t PARTICLE_COUNT = 8192;


    void VkRayTracingRenderer::_populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = _debugCallback;
    }


    bool VkRayTracingRenderer::_checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    bool VkRayTracingRenderer::_isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensionsSupported = _checkDeviceExtensionSupport(device);

        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }
    VkRayTracingRenderer::VkRayTracingRenderer()
    {
    }
    VkRayTracingRenderer::~VkRayTracingRenderer()
    {
    }
    void VkRayTracingRenderer::Init(RayTraceScene* scene)
    {
        mScene = scene;
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateCommandPool();
        //graphic shader pipeline -->postprocess


        //compute shader pipeline
        CreateUniformBuffers();
        CreateComputeTextureImage();
        CreateComputeTextureImageView();
        CreateShaderStorageBuffers();
        CreateComputeDescriptorPool();
        CreateComputeDescriptorSetLayout();
        CreateComputeDescriptorSets();
   
       
        CreateComputePipeline();

        CreateFrameBuffers();
        CreateVertexBuffer();
        CreateIndexBuffer();

        CreateTextureSampler();

        CreateDescriptorPool();
        CreateDescriptorSetLayout();
        CreateDescriptorSets();
        CreateGraphicsPipeline();

     
        CreateCommandBuffers();
        CreateComputeCommandBuffers();
        //for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        //            RecordComputeCommandBuffer(m_ComputeCommandBuffers[m_CurrentFrame], i);

        //}

        CreateSyncObjects();

    }
    void VkRayTracingRenderer::Render(Camera* camera)
    {
 
        //uint32_t imageIndex;
        //VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
        //if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //    RecreateSwapChain();
        //    return;
        //}
        //else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        //    throw std::runtime_error("failed to acquire swap chain image!");
        //}


        //// Compute submission        
        //vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        //UpdateUniformBuffer(m_CurrentFrame, camera);
        //vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);


        //VkSubmitInfo submitInfo{};
        //submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //vkResetCommandBuffer(m_ComputeCommandBuffers[m_CurrentFrame], 0);
        //RecordComputeCommandBuffer(m_ComputeCommandBuffers[m_CurrentFrame], imageIndex);


        //VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        //VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT };
        ////VkSemaphore waitSemaphores[] = {  m_ImageAvailableSemaphores[m_CurrentFrame] };
        ////VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        //submitInfo.waitSemaphoreCount = 1;
        //submitInfo.pWaitSemaphores = waitSemaphores;
        //submitInfo.pWaitDstStageMask = waitStages;

        //submitInfo.commandBufferCount = 1;
        //submitInfo.pCommandBuffers = &m_ComputeCommandBuffers[m_CurrentFrame];

        //VkSemaphore signalSemaphores[] = { m_RenderFinishSemaphores[m_CurrentFrame] };
        //submitInfo.signalSemaphoreCount = 1;
        //submitInfo.pSignalSemaphores = signalSemaphores;

        //if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to submit compute command buffer!");

        //}

        ////// Graphics submission        
        ////vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        /////*通过调用vKResetFences可以让一个Fence恢复成unsignaled的状态*/
        ////vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);
        //////vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        ////RecordCommandBuffer(m_ComputeCommandBuffers[m_CurrentFrame], imageIndex); 
        ///*
        //Fence用于同步渲染队列和CPU之间的同步，它有两种状态——signaled和unsignaled
        //在调用vkQueueSubmit时，可以传入一个Fence，这样当Queue中的所有命令都被完成以后，Fence就会被设置成signaled的状态
        //*/
        ///*if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
        //    throw std::runtime_error("failed to submit draw command buffer!");
        //}*/

        //VkPresentInfoKHR presentInfo{};
        //presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        //presentInfo.waitSemaphoreCount = 1;
        //presentInfo.pWaitSemaphores = signalSemaphores;

        //VkSwapchainKHR swapChains[] = { m_SwapChain };
        //presentInfo.swapchainCount = 1;
        //presentInfo.pSwapchains = swapChains;

        //presentInfo.pImageIndices = &imageIndex;

        //result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

        //if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        //    RecreateSwapChain();
        //}
        //else if (result != VK_SUCCESS) {
        //    throw std::runtime_error("failed to present swap chain image!");
        //}

        //m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        vkWaitForFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
        /*通过调用vKResetFences可以让一个Fence恢复成unsignaled的状态*/
        vkResetFences(m_Device, 1, &m_InFlightFences[m_CurrentFrame]);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        UpdateUniformBuffer(m_CurrentFrame, camera);

        vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
        RecordComputeCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];

        VkSemaphore signalSemaphores[] = { m_RenderFinishSemaphores[m_CurrentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        /*
        Fence用于同步渲染队列和CPU之间的同步，它有两种状态——signaled和unsignaled
        在调用vkQueueSubmit时，可以传入一个Fence，这样当Queue中的所有命令都被完成以后，Fence就会被设置成signaled的状态
        */
        if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_SwapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void VkRayTracingRenderer::UpdateUniformBuffer(uint32_t currentImage, Camera* camera)
    {
        double currentTime = glfwGetTime();
        m_LastFrameTime = (currentTime - m_LastTime) * 1000.0f;
        m_LastTime = currentTime;

      

        UniformBufferObject ubo{};
        glm::vec3 camRot = camera->GetRotation();
        ubo.camPos = camera->GetPosition();
        GE_CORE_INFO("CamPos = " + std::to_string(ubo.camPos.x)+","+std::to_string(ubo.camPos.y) + ","+std::to_string(ubo.camPos.z));

        if (ubo.camPos != m_LastCamPos || camRot!= m_LastCamRotate) {
            m_CurSample = 1;
        }
        ubo.numTriangles = (uint32_t)mScene->GetSceneTriangles().size();
        ubo.numSpheres = 0;
        ubo.numLights = (uint32_t)mScene->GetSceneLight().size();
        ubo.screenSize = glm::vec2(m_SwapChainExtent.width, m_SwapChainExtent.height);
        ubo.InvertProjectionView = glm::inverse(camera->GetProjectionMatrix() * camera->GetViewMatrix());
        
        ubo.currentSample = m_CurSample;

        m_LastCamPos = ubo.camPos;
        m_LastCamRotate = camRot;
        m_CurSample++;
        /*
        memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

        */
        memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

    }

    void VkRayTracingRenderer::PrepareResources()
    {
    }

    VkRayTracingRenderer::SwapChainSupportDetails VkRayTracingRenderer::_QuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR VkRayTracingRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    /** mode details: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Swap_chain */
    VkPresentModeKHR VkRayTracingRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }
    /** The swap extent is the resolution of the swap chain images and it's almost always exactly equal to the resolution of the window that we're drawing to in pixels (more on that in a moment). The range of the possible resolutions is defined in the VkSurfaceCapabilitiesKHR structure */
    VkExtent2D VkRayTracingRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    /**
     * we need to specify how to handle swap chain images that will be used across multiple queue families.
     *
     * VK_SHARING_MODE_EXCLUSIVE: An image is owned by one queue family at a time and ownership must be explicitly transferred before using it in another queue family. This option offers the best performance.
     * VK_SHARING_MODE_CONCURRENT: Images can be used across multiple queue families without explicit ownership transfers.
     *
     */
    VkRayTracingRenderer::QueueFamilyIndices VkRayTracingRenderer::FindQueueFamilies(const VkPhysicalDevice& device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                indices.graphicsAndComputeFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }
    void VkRayTracingRenderer::CreateInstance()
    {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            _populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void VkRayTracingRenderer::SetupDebugMessenger()
    {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        _populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    void VkRayTracingRenderer::CreateSurface()
    {
        if (glfwCreateWindowSurface(m_Instance, static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), nullptr, &m_Surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VkRayTracingRenderer::PickPhysicalDevice()
    {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (_isDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void VkRayTracingRenderer::CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_Device, indices.graphicsAndComputeFamily.value(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, indices.graphicsAndComputeFamily.value(), 0, &m_ComputeQueue);
        vkGetDeviceQueue(m_Device, indices.presentFamily.value(), 0, &m_PresentQueue);
    }

    void VkRayTracingRenderer::CreateSwapChain() {
        SwapChainSupportDetails swapChainSupport = _QuerySwapChainSupport(m_PhysicalDevice);
        VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        /** The imageArrayLayers specifies the amount of layers each image consists of.  */
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
        uint32_t queueFamilyIndices[] = { indices.graphicsAndComputeFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsAndComputeFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;

    }
    /**
     *  creates a basic image view for every image in the swap chain
     *  so that we can use them as color targets later on.
     *  reference: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Image_views
     */
    void VkRayTracingRenderer::CreateImageViews()
    {
        m_ImageViews.resize(m_SwapChainImages.size());
        for (int i = 0; i < m_SwapChainImages.size(); i++) {
            VkImageViewCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_SwapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            /**
             * The subresourceRange field describes what the image's purpose is and which part of the image should be accessed.
             *  Our images will be used as color targets without any mipmapping levels or multiple layers..
             *
             */
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS) {
                GE_CORE_ERROR("failed to create image views!");
            }
        }
    }

    void VkRayTracingRenderer::CreateRenderPass()
    {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_SwapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        /** The loadOp and storeOp determine what to do with the data in the attachment before rendering and after rendering */
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        /**
         * The loadOp and storeOp apply to color and depth data, and stencilLoadOp / stencilStoreOp apply to stencil data.
         * Our application won't do anything with the stencil buffer, so the results of loading and storing are irrelevant..
         *
         */
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        /** Using VK_IMAGE_LAYOUT_UNDEFINED for initialLayout means that we don't care what previous layout the image was in. */
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        /** The finalLayout specifies the layout to automatically transition to when the render pass finishes. */
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // Attachment for a sub-pass.
        VkAttachmentReference colorAttachmentRef{};
        // Index of the attachement in the attachments array.
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        /**
         * Subpasses are subsequent rendering operations that depend on the contents of framebuffers in previous passes,
         *  for example a sequence of post-processing effects that are applied one after another.
         * If you group these rendering operations into one render pass, then Vulkan is able to reorder
         * the operations and conserve memory bandwidth for possibly better performance. .
         *
         */
        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        // Subpass dependencies for layout transitions
      /*  std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
*/
// Subpass dependencies for layout transitions
        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    /*    renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies.data();*/

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 2;
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    //ubo desctriptor set
    void VkRayTracingRenderer::CreateComputeDescriptorSetLayout()
    {
    /*    RHIBindingLayoutDesc layoutDesc;

        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::ConstantBuffer(0),
            RHIBindingLayoutItem::Texture_UAV(1),
            RHIBindingLayoutItem::Texture_UAV(2),
            RHIBindingLayoutItem::RawBuffer_SRV(3),
            RHIBindingLayoutItem::RawBuffer_SRV(4),
            RHIBindingLayoutItem::RawBuffer_SRV(5),
            RHIBindingLayoutItem::RawBuffer_SRV(6),
            RHIBindingLayoutItem::RawBuffer_SRV(7)
        };*/

        std::array<VkDescriptorSetLayoutBinding, 7> bindings;

        bindings[0].binding = 0;
        bindings[0].descriptorCount = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        bindings[0].pImmutableSamplers = nullptr;
        bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[1].binding = 1;
        bindings[1].descriptorCount = 1;
        bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        bindings[1].pImmutableSamplers = nullptr;
        bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[2].binding = 2;
        bindings[2].descriptorCount = 1;
        bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        bindings[2].pImmutableSamplers = nullptr;
        bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[3].binding = 3;
        bindings[3].descriptorCount = 1;
        bindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[3].pImmutableSamplers = nullptr;
        bindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[4].binding = 4;
        bindings[4].descriptorCount = 1;
        bindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[4].pImmutableSamplers = nullptr;
        bindings[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[5].binding = 5;
        bindings[5].descriptorCount = 1;
        bindings[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[5].pImmutableSamplers = nullptr;
        bindings[5].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        bindings[6].binding = 6;
        bindings[6].descriptorCount = 1;
        bindings[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        bindings[6].pImmutableSamplers = nullptr;
        bindings[6].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;


        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_ComputeDescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }



    void VkRayTracingRenderer::CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        //createBuffer(m_PhysicalDevice, m_Device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_VertexBuffer, m_VertexBufferMemory);

        // use stage buffer

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        createBuffer(
            m_PhysicalDevice,
            m_Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);



        void* data;
        vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(m_Device, stagingBufferMemory);


        createBuffer(
            m_PhysicalDevice,
            m_Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_VertexBuffer, m_VertexBufferMemory);


        copyBuffer(m_GraphicsQueue, m_Device, m_CommandPool, stagingBuffer, m_VertexBuffer, bufferSize);


        vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
        vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
    }

    void VkRayTracingRenderer::CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(m_PhysicalDevice,
            m_Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(m_Device, stagingBufferMemory);

        createBuffer(
            m_PhysicalDevice,
            m_Device,
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_IndexBuffer, m_IndexBufferMemory);

        copyBuffer(m_GraphicsQueue, m_Device, m_CommandPool, stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
        vkFreeMemory(m_Device, stagingBufferMemory, nullptr);
    }

    void VkRayTracingRenderer::CreateGraphicsPipeline()
    {
        vkShader* vertShaderCode = DBG_NEW vkShader("assets/shaders/spv/postProcess_vert.spv");
        vkShader* fragShaderCode = DBG_NEW vkShader("assets/shaders/spv/postProcess_frag.spv");

       // vkShader* vertShaderCode = DBG_NEW vkShader("assets/shaders/spv/vkBasic_vert.spv");
       // vkShader* fragShaderCode = DBG_NEW vkShader("assets/shaders/spv/vkBasic_frag.spv");

        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode->Code());
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode->Code());

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        auto bindingDescription = Quad::getBindingDescription();
        auto attributeDescriptions = Quad::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f;          // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = .2f;           // min fraction for sample shading; closer to one is smoother
        multisampling.pSampleMask = nullptr;            // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE;      // Optional

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
       /* pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;*/
        if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {};  // Optional

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = m_PipelineLayout;
        pipelineInfo.renderPass = m_RenderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex = -1;              // Optional
        pipelineInfo.pDepthStencilState = &depthStencil;


        if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
       
    }

    void VkRayTracingRenderer::CreateComputePipeline()
    {
        vkShader* compShaderCode = DBG_NEW vkShader("assets/shaders/spv/rayTracing_comp.spv");


        VkShaderModule compShaderModule = CreateShaderModule(compShaderCode->Code());

        VkPipelineShaderStageCreateInfo compShaderStageInfo{};
        compShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        compShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        compShaderStageInfo.module = compShaderModule;
        compShaderStageInfo.pName = "main";



        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &m_ComputeDescriptorSetLayout;

        if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_ComputePipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.layout = m_ComputePipelineLayout;
        pipelineInfo.stage = compShaderStageInfo;

        if (vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_ComputePipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_Device, compShaderModule, nullptr);
    }

    void VkRayTracingRenderer::CreateShaderStorageBuffers()
    {
        m_SSBOTriangles.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOMaterials.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOAABBs.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOLights.resize(MAX_FRAMES_IN_FLIGHT);


        m_SSBOTrianglesMemory.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOMaterialsMemory.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOAABBsMemory.resize(MAX_FRAMES_IN_FLIGHT);
        m_SSBOLightsMemory.resize(MAX_FRAMES_IN_FLIGHT);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createSSBObuffer<RayTraceScene::triangle>(m_PhysicalDevice, m_Device, m_GraphicsQueue, m_CommandPool, m_SSBOTriangles[i], m_SSBOTrianglesMemory[i], mScene->GetSceneTriangles().data(), mScene->GetSceneTriangles().size());
   /*         VkDeviceSize bufferSize = sizeof(RayTraceScene::material) * (int)mScene->GetSceneMaterial().size();
            VkDeviceSize bufferSize1 = sizeof(RayTraceScene::material) * mScene->GetSceneMaterial().size();
            int s =(int)(mScene->GetSceneMaterial().size());
            size_t ss = mScene->GetSceneMaterial().size();
            int sd = (int)ss;
            size_t ssd = sizeof(RayTraceScene::material);
            size_t ssdd = sizeof(RayTraceScene::material) * mScene->GetSceneMaterial().size();*/

            createSSBObuffer<RayTraceScene::material>(m_PhysicalDevice, m_Device, m_GraphicsQueue, m_CommandPool, m_SSBOMaterials[i], m_SSBOMaterialsMemory[i], mScene->GetSceneMaterial().data(), mScene->GetSceneMaterial().size());
            createSSBObuffer<RayTraceScene::bvhNode>(m_PhysicalDevice, m_Device, m_GraphicsQueue, m_CommandPool, m_SSBOAABBs[i], m_SSBOAABBsMemory[i], mScene->GetSceneBVHNode().data(), mScene->GetSceneBVHNode().size());
            createSSBObuffer<RayTraceScene::light>(m_PhysicalDevice, m_Device, m_GraphicsQueue, m_CommandPool, m_SSBOLights[i], m_SSBOLightsMemory[i], mScene->GetSceneLight().data(), mScene->GetSceneLight().size());
        }
    }

    void VkRayTracingRenderer::CreateFrameBuffers()
    {
        m_FrameBuffers.resize(m_ImageViews.size());
        for (size_t i = 0; i < m_ImageViews.size(); i++) {
            VkImageView attachments[] = {
                m_ImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_SwapChainExtent.width;
            framebufferInfo.height = m_SwapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_FrameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VkRayTracingRenderer::CreateCommandPool()
    {
        QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(m_PhysicalDevice);

        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

        if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VkRayTracingRenderer::CreateComputeTextureImage()
    {
        int texWidth = m_SwapChainExtent.width;
        int texHeight = m_SwapChainExtent.height;

        ImageUtils::createImage(m_PhysicalDevice, m_Device, texWidth, texHeight,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_TargetTexture,
            m_TargetTextureImageMemory);

        ImageUtils::createImage(m_PhysicalDevice, m_Device, texWidth, texHeight,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_AccumulationTexture,
            m_AccumulationTextureImageMemory);

        ImageUtils::transitionImageLayout(m_Device,
            m_GraphicsQueue,
            m_CommandPool,
            m_TargetTexture,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
            );

        ImageUtils::transitionImageLayout(m_Device,
            m_GraphicsQueue,
            m_CommandPool,
            m_AccumulationTexture,
            VK_FORMAT_R8G8B8A8_UNORM,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_GENERAL);

    }

    void VkRayTracingRenderer::CreateComputeTextureImageView()
    {
        m_TargetTextureImageView = ImageUtils::createImageView(m_Device, m_TargetTexture, VK_FORMAT_R8G8B8A8_UNORM);
        m_AccumulationTextureImageView = ImageUtils::createImageView(m_Device, m_AccumulationTexture, VK_FORMAT_R8G8B8A8_UNORM);
    }

    void VkRayTracingRenderer::CreateTextureSampler()
    {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    void VkRayTracingRenderer::CreateUniformBuffers()
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            createBuffer(
                m_PhysicalDevice,
                m_Device,
                bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                m_UniformBuffers[i],
                m_UniformBuffersMemory[i]);
            /*
            The buffer stays mapped to this pointer for the application's whole lifetime.
            This technique is called "persistent mapping" and works on all Vulkan implementations.
            Not having to map the buffer every time we need to update it increases performances,
            as mapping is not free.
            */
            vkMapMemory(m_Device, m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
        }
    }

    void VkRayTracingRenderer::CreateDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 1> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
      
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }

    void VkRayTracingRenderer::CreateComputeDescriptorPool()
    {
        std::array<VkDescriptorPoolSize, 3> poolSizes{};
        poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2;
        poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 4;

        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());;
        poolInfo.pPoolSizes = poolSizes.data();
        poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        if (vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_ComputeDescriptorPool) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor pool!");
        }
    }
    void VkRayTracingRenderer::CreateDescriptorSetLayout()
    {
        std::array<VkDescriptorSetLayoutBinding, 1> bindings;

        bindings[0].binding = 0;
        bindings[0].descriptorCount = 1;
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].pImmutableSamplers = nullptr;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

       
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    void VkRayTracingRenderer::CreateDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_DescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView = m_TargetTextureImageView;
            imageInfo.sampler = m_TextureSampler;

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_DescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pImageInfo = &imageInfo;
                             

            vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VkRayTracingRenderer::CreateComputeDescriptorSets()
    {
        std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_ComputeDescriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_ComputeDescriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        allocInfo.pSetLayouts = layouts.data();

        m_ComputeDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
        if (vkAllocateDescriptorSets(m_Device, &allocInfo, m_ComputeDescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo uniformBufferInfo{};
            uniformBufferInfo.buffer = m_UniformBuffers[i];
            uniformBufferInfo.offset = 0;
            uniformBufferInfo.range = sizeof(UniformBufferObject);

            std::array<VkWriteDescriptorSet, 7> descriptorWrites{};
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &uniformBufferInfo;


            VkDescriptorImageInfo imageInfo{};
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo.imageView = m_TargetTextureImageView;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;

            VkDescriptorImageInfo imageInfo1{};
            imageInfo1.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
            imageInfo1.imageView = m_AccumulationTextureImageView;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            descriptorWrites[2].descriptorCount = 1;
            descriptorWrites[2].pImageInfo = &imageInfo1;

            //m_SSBOTriangles
            VkDescriptorBufferInfo ssboInfo{};
            ssboInfo.buffer = m_SSBOTriangles[i];
            ssboInfo.offset = 0;
            uint32_t triCnt = mScene->GetSceneTriangles().size();
            ssboInfo.range = sizeof(RayTraceScene::triangle)* triCnt;

            descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[3].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[3].dstBinding = 3;
            descriptorWrites[3].dstArrayElement = 0;
            descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[3].descriptorCount = 1;
            descriptorWrites[3].pBufferInfo = &ssboInfo;

            //m_SSBOMaterials
            VkDescriptorBufferInfo ssboInfo1{};
            ssboInfo1.buffer = m_SSBOMaterials[i];
            ssboInfo1.offset = 0;
            uint32_t matCnt = mScene->GetSceneMaterial().size();
            ssboInfo1.range = sizeof(RayTraceScene::material)* matCnt;

            descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[4].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[4].dstBinding = 4;
            descriptorWrites[4].dstArrayElement = 0;
            descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[4].descriptorCount = 1;
            descriptorWrites[4].pBufferInfo = &ssboInfo1;

            //m_SSBOAABBs
            VkDescriptorBufferInfo ssboInfo2{};

            ssboInfo2.buffer = m_SSBOAABBs[i];
            ssboInfo2.offset = 0;
            uint32_t bvhCnt = mScene->GetSceneBVHNode().size();
            ssboInfo2.range = sizeof(RayTraceScene::bvhNode)* bvhCnt;

            descriptorWrites[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[5].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[5].dstBinding = 5;
            descriptorWrites[5].dstArrayElement = 0;
            descriptorWrites[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[5].descriptorCount = 1;
            descriptorWrites[5].pBufferInfo = &ssboInfo2;

            //m_SSBOLights
            VkDescriptorBufferInfo ssboInfo3{};
            ssboInfo3.buffer = m_SSBOLights[i];
            ssboInfo3.offset = 0;
            uint32_t lightCnt = mScene->GetSceneLight().size();

            ssboInfo3.range = sizeof(RayTraceScene::light)* lightCnt;

            descriptorWrites[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[6].dstSet = m_ComputeDescriptorSets[i];
            descriptorWrites[6].dstBinding = 6;
            descriptorWrites[6].dstArrayElement = 0;
            descriptorWrites[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[6].descriptorCount = 1;
            descriptorWrites[6].pBufferInfo = &ssboInfo3;

            vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void VkRayTracingRenderer::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }


    }

    void VkRayTracingRenderer::CreateComputeCommandBuffers()
    {
        m_ComputeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.commandBufferCount = (uint32_t)m_ComputeCommandBuffers.size();
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_ComputeCommandBuffers.size();

        if (vkAllocateCommandBuffers(m_Device, &allocInfo, m_ComputeCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void VkRayTracingRenderer::CreateSyncObjects()
    {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_ComputeFinishSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_ComputeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (
                vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS
                ) {
                throw std::runtime_error("failed to create semaphore!");

            }
            if (
                vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ComputeFinishSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_Device, &fenceInfo, nullptr, &m_ComputeInFlightFences[i]) != VK_SUCCESS
                ) {
                throw std::runtime_error("failed to create semaphore!");

            }
        }

    }

    VkShaderModule VkRayTracingRenderer::CreateShaderModule(const std::vector<char>& code)
    {
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            GE_CORE_ERROR("failed to create shader module!");
        }

        return shaderModule;
    }
    void VkRayTracingRenderer::RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow()), &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m_Device);
        CleanUpSwapChain();

        CreateSwapChain();
        CreateImageViews();
        CreateFrameBuffers();
    }

    void VkRayTracingRenderer::CleanUp()
    {
        for (auto imageView : m_ImageViews)
        {
            vkDestroyImageView(m_Device, imageView, nullptr);
        }

        for (auto framebuffer : m_FrameBuffers) {
            vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
        }
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_RenderFinishSemaphores[i], nullptr);
            vkDestroySemaphore(m_Device, m_ComputeFinishSemaphores[i], nullptr);
            vkDestroyFence(m_Device, m_InFlightFences[i], nullptr);
            vkDestroyFence(m_Device, m_ComputeInFlightFences[i], nullptr);

        }

        vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device, m_VertexBufferMemory, nullptr);

        //vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
        //vkFreeMemory(m_Device, m_IndexBufferMemory, nullptr);

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyBuffer(m_Device, m_SSBOTriangles[i], nullptr);
            vkDestroyBuffer(m_Device, m_SSBOMaterials[i], nullptr);
            vkDestroyBuffer(m_Device, m_SSBOAABBs[i], nullptr);
            vkDestroyBuffer(m_Device, m_SSBOLights[i], nullptr);
            vkFreeMemory(m_Device, m_SSBOTrianglesMemory[i], nullptr);
            vkFreeMemory(m_Device, m_SSBOMaterialsMemory[i], nullptr);
            vkFreeMemory(m_Device, m_SSBOAABBsMemory[i], nullptr);
            vkFreeMemory(m_Device, m_SSBOLightsMemory[i], nullptr);

        }

        vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

        vkDestroyImage(m_Device, m_TargetTexture, nullptr);
        vkDestroyImage(m_Device, m_AccumulationTexture, nullptr);
        vkDestroyImageView(m_Device, m_TargetTextureImageView, nullptr);
        vkDestroyImageView(m_Device, m_AccumulationTextureImageView, nullptr);

        vkDestroySampler(m_Device, m_TextureSampler, nullptr);
        vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
        vkDestroyInstance(m_Instance, nullptr);
    }

    void VkRayTracingRenderer::CleanUpSwapChain()
    {
        for (size_t i = 0; i < m_FrameBuffers.size(); i++)
        {
            vkDestroyFramebuffer(m_Device, m_FrameBuffers[i], nullptr);
        }

        for (size_t i = 0; i < m_ImageViews.size(); i++)
        {
            vkDestroyImageView(m_Device, m_ImageViews[i], nullptr);
        }

        vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    }
    void VkRayTracingRenderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
       /* VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }*/

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_RenderPass;
        renderPassInfo.framebuffer = m_FrameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_SwapChainExtent;

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 1.0f, 0.5f, 1.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

       /* VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
       */

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);


        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_SwapChainExtent.width;
        viewport.height = (float)m_SwapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = m_SwapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
        VkBuffer vertexBuffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);

        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSets[m_CurrentFrame], 0, nullptr);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

        //vkCmdDraw(commandBuffer, 4, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

      /*  if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }*/
    }
    void VkRayTracingRenderer::RecordComputeCommandBuffer(VkCommandBuffer commandBuffer, unsigned int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording compute command buffer!");
        }

        VkImageMemoryBarrier read2Gen = ImageUtils::readOnlyToGeneralBarrier(m_TargetTexture);

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &read2Gen);

        // Bind compute pipeline and dispatch compute command.
        //computeModel->computeCommand(commandBuffers[i], i, m_TargetTexture->width / 32, targetImage->height / 32, 1);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipeline);
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_ComputePipelineLayout, 0, 1, &m_ComputeDescriptorSets[m_CurrentFrame], 0, nullptr);
        vkCmdDispatch(commandBuffer, m_SwapChainExtent.width / 32, m_SwapChainExtent.height / 32, 1);
        // Transfer target and accumulation images to transfer layout and copy one into another.
        VkImageMemoryBarrier gen2TranSrc = ImageUtils::generalToTransferSrcBarrier(m_TargetTexture);

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &gen2TranSrc);

        VkImageMemoryBarrier gen2TranDst = ImageUtils::generalToTransferDstBarrier(m_AccumulationTexture);

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &gen2TranDst);

        VkImageCopy region = ImageUtils::imageCopyRegion(m_SwapChainExtent.width, m_SwapChainExtent.height);
        vkCmdCopyImage(
            commandBuffer,
            m_TargetTexture,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            m_AccumulationTexture,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region);

        VkImageMemoryBarrier tranDst2Gen = ImageUtils::transferDstToGeneralBarrier(m_AccumulationTexture);

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &tranDst2Gen);

        // Convert image layout to READ_ONLY_OPTIMAL before reading from it in fragment shader.
        VkImageMemoryBarrier tranSrc2ReadOnly = ImageUtils::transferSrcToReadOnlyBarrier(m_TargetTexture);

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &tranSrc2ReadOnly);


        RecordCommandBuffer(commandBuffer, imageIndex);
       

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record compute command buffer!");
        }
    }
    uint32_t VkRayTracingRenderer::_FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
}
#endif