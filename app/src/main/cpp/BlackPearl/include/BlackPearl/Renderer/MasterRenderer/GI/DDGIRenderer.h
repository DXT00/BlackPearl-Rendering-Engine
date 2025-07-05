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

        void _InitVolumesAndPipeline(Scene* scene);

        void _UpdateProbeIrradiance(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        void _UpdateProbeDistance(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);

        // sw tracing
       // void GenerateGDF();

    private:

       


        MaterialShader* m_ProbeUpdateShader = nullptr; 

        std::vector<IrradianceVolume> m_Volumes;
        std::vector<DDGIPipelineInternal> m_Pipelines; // each volume has a pipline

        

        RayTrace* m_Tracer = nullptr;




    };
}
