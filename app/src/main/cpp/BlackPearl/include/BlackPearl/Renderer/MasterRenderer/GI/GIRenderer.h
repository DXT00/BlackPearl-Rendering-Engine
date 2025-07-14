#pragma once
#include "RHI/RHIDevice.h"
#include "Scene/Scene.h"
namespace BlackPearl {
    class IView;
    class IFramebuffer;
    class GIRenderer
    {
    public:
        virtual void Init(Scene* scene) = 0;

        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) = 0;
        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {};
        virtual void RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene) = 0;

        virtual void RenderUI(IFramebuffer* framebuffer, IView* View) {}

        virtual void ProbeGather(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {};

    };


}
