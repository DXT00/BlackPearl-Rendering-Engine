#pragma once
#include "GIRenderer.h"
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "Math/vector.h"
#include "RHI/RHITexture.h"
#include "Renderer/DDGI/IrradianceVolume.h"
#include "Renderer/DDGI/DDGIPipelineInternal.h"

namespace BlackPearl {

    struct DDGIData {

    };

    class RayTrace;
    class DDGIRenderer : public GIRenderer, public BasicRenderer
    {
    public:
        DDGIRenderer(IDevice* device);
        virtual void Init(Scene* scene) override;
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
        
        virtual void RayTraceVolumes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        virtual void UpdateVolumeProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);

        
        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
   


    private:

       

        MaterialShader* m_ProbeIrradianceUpdateShader = nullptr; 
        MaterialShader* m_ProbeDepthUpdateShader = nullptr;

        std::vector<IrradianceVolume> m_Volumes;
        std::vector<DDGIPipelineInternal> m_Pipelines; // each volume has a pipline

        struct ProbeBindings {
            BindingLayoutHandle layout;
            BindingSetHandle    set;
            BufferHandle        ddgiCB;
            BufferHandle        frameCB;
            ProbeBindings() {
                layout = nullptr;
                set = nullptr;
                ddgiCB = nullptr;
                frameCB = nullptr;
            }

        };
        std::vector<ProbeBindings> m_ProbeUpdateBindings;
        ComputePipelineHandle m_ProbeIrradianceUpdatePso;
        ComputePipelineHandle m_ProbeDiatanceUpdatePso;

        RayTrace* m_Tracer = nullptr;


    private:

        void _InitVolumesAndPipeline(Scene* scene);
        void _InitProbeUpdate(Scene* scene);

        void _UpdateProbeIrradiance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeBindings& binidngs, Scene* scene);
        void _UpdateProbeDistance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeBindings& binidngs, Scene* scene);

        void _FillUpdateProbeShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume,  const ProbeBindings& binidngs);
        // sw tracing
       // void GenerateGDF();

    };
}
