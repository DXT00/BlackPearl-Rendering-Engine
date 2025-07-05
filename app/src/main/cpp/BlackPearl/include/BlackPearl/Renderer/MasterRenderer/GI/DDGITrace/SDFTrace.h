#pragma once
#include "RayTrace.h"
#include "Renderer/Shader/MaterialShader.h"
#include "Renderer/SDF/GlobalDistanceField.h"
#include "Renderer/Voxel/Voxel.h"

namespace BlackPearl {
    class SDFTrace : public RayTrace
    {
    public:
        SDFTrace() {

        }
        virtual ~SDFTrace(){}


        virtual void Init(IDevice* device) override;
        virtual void Execute(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene) override;

    private:
        IDevice* m_Device;
        MaterialShader* m_SDFGIShader = nullptr;
        BufferHandle m_DDGIRayCB;

        struct SDFTaceBindings {

            BindingLayoutHandle gdfLayout;
            BindingSetHandle    gdfSet;
            BufferHandle        gdfCB;

            BindingLayoutHandle voxelLayout;
            BindingSetHandle    voxelSet;
            BufferHandle        voxelCB;

            BindingLayoutHandle skyboxLayout;
            BindingSetHandle    skyboxSet;

            BindingLayoutHandle ddgiLayout;
            BindingSetHandle    ddgiSet;
            BufferHandle        ddgiCB;
            BufferHandle        ddgiRayCB;


            BindingLayoutHandle outputLayout;
            BindingSetHandle    outputSet;
            TextureHandle       traceRadiance;
            TextureHandle       traceDirectionDistance;
        };

        SDFTaceBindings     m_Bindings;

        

        ComputePipelineHandle m_SDFTracePso = nullptr;


       


        void _InitBindingsSDF();
        void _InitBindingsVoxel();
        void _InitBindingsSkybox();
        void _InitBindingsDDGI();
        void _InitBindingsOutput();

        // find current voxel
        void _FillShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene);

        void _FillDDGI(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene);
        void _FillGDF(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene);
        void _FillVoxel(ICommandList* cmdList, const glm::vec3 camPos);
        void _FillOutputImage(const DDGIPipelineInternal& currentVolume);

    };
}

