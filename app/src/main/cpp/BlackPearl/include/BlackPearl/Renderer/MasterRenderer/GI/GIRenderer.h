#pragma once
#include "RHI/RHIDevice.h"
#include "Scene/Scene.h"
namespace BlackPearl {
    class GIRenderer
    {
    public:
        virtual void Init(Scene* scene) = 0;

        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) = 0;
        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {};

    };


}
