#pragma once
#include "RHI/RHIDevice.h"
#include "Scene/Scene.h"
namespace BlackPearl {
    class ProbeRenderer
    {
    public:
        virtual void Init() = 0;
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) = 0;
        virtual void RenderProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {};

    };


}
