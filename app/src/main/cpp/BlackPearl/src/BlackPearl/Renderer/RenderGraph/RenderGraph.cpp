#include "pch.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Renderer/DeviceManager.h"
#include "RHI/RHIGlobals.h"
namespace BlackPearl {
    void RenderGraph::AddPass(BasicRenderer* renderer)
    {
        mRenderPasses.push_back(renderer);
    }
    bool RenderGraph::SupportSinglePass(bool msaaSample)
    {
#ifdef GE_API_VULKAN
        return true; //subpass
#elif defined(GE_API_OPENGL)
#ifdef GE_PLATFORM_ANDROID //android use gles
        if (GSupportsShaderFramebufferFetch || GSupportsShaderDepthStencilFetch || GSupportsPixelLocalStorage)
            return true;
#elif defined(GE_PLATFORM_WINDOWS)
        return Configuration::bUseSinglePass;
#endif
#endif

        return false;

    }
    bool RenderGraph::SupportPLS()
    {
#ifdef GE_API_OPENGL
#ifdef GE_PLATFORM_ANDROID
        if (GSupportsPixelLocalStorage && GSupportsShaderDepthStencilFetch)
            return true;
#endif
#endif
        return false;
    }
}