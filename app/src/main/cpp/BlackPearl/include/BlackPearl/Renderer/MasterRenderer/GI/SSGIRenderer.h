#pragma once
#include "GIRenderer.h"
namespace BlackPearl {
    class SSGIRenderer : public GIRenderer
    {
    public:
        SSGIRenderer(IDevice* device);
        virtual void Init(Scene* scene) override {};
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override {};
        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override {};
        virtual void RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene) override;

    };
}

