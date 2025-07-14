#include "pch.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Renderer/DeviceManager.h"
#include "RHI/RHIGlobals.h"
#include "Map/Mapmanager.h"

namespace BlackPearl {
    extern MapManager* g_mapManager;

    void RenderGraph::AddPass(BasicRenderer* renderer)
    {
        mRenderPasses.push_back(renderer);
    }
    bool RenderGraph::SupportSinglePass(bool msaaSample)
    {
#ifdef GE_API_VULKAN
        return Configuration::bUseSinglePass; //subpass
#elif defined(GE_API_OPENGL)
#ifdef GE_PLATFORM_ANDROID //android use gles
        if(!Configuration::bUseSinglePass)
            return false;
        if (GSupportsShaderFramebufferFetch || (GSupportsShaderDepthStencilFetch && GSupportsPixelLocalStorage))
            return true;
#elif defined(GE_PLATFORM_WINDOWS)
        return false;// default to mlti pass
#endif
#endif
        return false;

    }
    bool RenderGraph::SupportPLS()
    {
#ifdef GE_API_OPENGL
#ifdef GE_PLATFORM_ANDROID
        if (Configuration::bUseSinglePass && GSupportsPixelLocalStorage && GSupportsShaderDepthStencilFetch)
            return true;
#endif
#endif
        return false;
    }

    //render GI if camera is inside map
    bool RenderGraph::ShouldRender()
    {
        auto camPos = Renderer::GetSceneData()->CameraPosition;
        int areaId = g_mapManager->CalculateAreaId(camPos);

        return areaId >= 0 && g_mapManager->GetArea(areaId);
    }
}