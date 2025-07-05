#pragma once
#include "RayTrace.h"
namespace BlackPearl {
    
    class HWTrace : public RayTrace
    {
    public:
        HWTrace() {

        }
        virtual ~HWTrace() {}


        virtual void Init(IDevice* device) override;
        virtual void Execute(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene) override;
    };
}

