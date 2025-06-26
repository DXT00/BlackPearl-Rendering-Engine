#include "pch.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/DeviceManager.h"
namespace BlackPearl {
    extern DeviceManager* g_deviceManager;

    DDGIRenderer::DDGIRenderer(IDevice* device) :
        BasicRenderer(device)
    {
    }

    void DDGIRenderer::Init()
    {
    }

    void DDGIRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {

    }

    void DDGIRenderer::RenderProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {

    }

    void DDGIRenderer::GenerateGDF()
    {
    }

   
}
