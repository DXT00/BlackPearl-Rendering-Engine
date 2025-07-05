#pragma once
#include "RHI/RHICommandList.h"
#include "Renderer/DDGI/IrradianceVolume.h"
#include "Renderer/DDGI/DDGIPipelineInternal.h"
#include "Scene/Scene.h"

namespace BlackPearl {
    class IDevice;
    class RayTrace
    {
    public:
        static RayTrace* CreateRayTracer(DDGITraceType type);
        virtual void Init(IDevice* device) = 0;
        virtual void Execute(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene) = 0;
    };
}


