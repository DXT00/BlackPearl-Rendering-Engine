#pragma once
#include "ProbeRenderer.h"
namespace BlackPearl {
    class SSGIRenderer : public ProbeRenderer
    {
    public:
        SSGIRenderer(IDevice* device);
        virtual void Init() override {};
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override {};
        virtual void RenderProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override {};
    };
}

