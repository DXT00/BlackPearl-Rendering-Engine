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
    class GIManager;
    class RayTrace;
    class DDGIRenderer : public GIRenderer, public BasicRenderer
    {
    public:
        virtual void Init(Scene* scene) override;
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
        
        virtual void RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene) override;

        virtual void RenderUI(IFramebuffer* framebuffer, IView* View) override;


        virtual void RayTraceVolumes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        virtual void UpdateVolumeProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);


        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
   

        virtual void ProbeGather(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;

    private:
        friend class GIManager;
        DDGIRenderer(IDevice* device);

       

        MaterialShader* m_ProbeIrradianceUpdateShader = nullptr; 
        MaterialShader* m_ProbeDepthUpdateShader = nullptr;
        MaterialShader* m_ProbeGatherShader = nullptr;
        MaterialShader* m_ProbeDebugShader = nullptr;

        std::vector<IrradianceVolume> m_Volumes;
        std::vector<DDGIPipelineInternal> m_Pipelines; // each volume has a pipline

        struct ProbeUpdateBindings {
            BindingLayoutHandle layout;
            BindingSetHandle    set;
            BufferHandle        ddgiCB;
            BufferHandle        frameCB;
            ProbeUpdateBindings() {
                layout = nullptr;
                set = nullptr;
                ddgiCB = nullptr;
                frameCB = nullptr;
            }

        };
        std::vector<ProbeUpdateBindings> m_ProbeUpdateBindings;
        ComputePipelineHandle m_ProbeIrradianceUpdatePso = nullptr;
        ComputePipelineHandle m_ProbeDiatanceUpdatePso = nullptr;

        RayTrace* m_Tracer = nullptr;

        struct ProbeGatherBindings {
            BindingLayoutHandle layout;
            BindingSetHandle    set;
            BufferHandle        ddgiSSBO; 
            BufferHandle        areaCB;
            TextureHandle       outputIndirectLighting;
            ProbeGatherBindings() {
                layout = nullptr;
                set = nullptr;
                ddgiSSBO = nullptr;
                areaCB = nullptr;
            }

        };
        ProbeGatherBindings m_ProbeGatherBindings;
        ComputePipelineHandle m_ProbeGatherPso = nullptr;


        struct ProbeDebugBindings {
            //TODO:: Probe Material;
            BindingLayoutHandle layout;
            BindingSetHandle    set;
            BufferHandle        probeCB;
            BufferHandle        volumeCB;

        };
        ProbeDebugBindings m_ProbeDebugBindings;
        GraphicsPipelineHandle m_ProbeDebugPso = nullptr;
      /*  BindingLayoutHandle m_ProbeBindingLayout;
        BindingSetHandle    m_ProbeBindingSet;
       
        BufferHandle    m_ProbeCB;
        BufferHandle    m_VolumeCB;*/




        //Deferred IndirectLight shading
        MaterialShader* m_DeferredDDGIShader = nullptr;

        BindingLayoutHandle m_DeferredShadingBindingLayout;
        BindingSetHandle    m_DeferredShadingBindingSet;
        GraphicsPipelineHandle m_DeferredShadingPso = nullptr;

        struct UIData {
            uint32_t numVolumeInArea;
            int currentAreaId;
            float hysteresisOfLastFrame;
        };
    private:

        void _InitVolumesAndPipeline(Scene* scene);
        void _InitProbeUpdate(Scene* scene);
        void _InitProbeGather(Scene* scene);
        void _InitDeferredLighting();
        void _InitProbeDebug();

        void _UpdateProbeIrradiance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeUpdateBindings& binidngs, Scene* scene);
        void _UpdateProbeDistance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeUpdateBindings& binidngs, Scene* scene);
        void _RenderProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* probe, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline);

        //Indirect lighint
        void _IndirectShading(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);

        void _FillUpdateProbeShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume,  const ProbeUpdateBindings& binidngs);
        // sw tracing
       // void GenerateGDF();

        uint32_t m_LastWrite = 0;
        UIData m_UI;

    };
}
