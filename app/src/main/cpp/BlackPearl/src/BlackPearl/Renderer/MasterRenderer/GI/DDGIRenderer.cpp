#include "pch.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/DeviceManager.h"
#include "Map/MapManager.h"
#include "LightProbes/LightProbeGrid.h"
#include "RHI/Common/RHIUtils.h"
#include "hlsl/core/ddgi_cb.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/RayTrace.h"
#include "Component/BoundingBoxComponent/BoundingBox.h"
namespace BlackPearl {
    extern MapManager* g_mapManager;

    DDGIRenderer::DDGIRenderer(IDevice* device) :
        BasicRenderer(device)
    {
    }

    void DDGIRenderer::Init(Scene* scene)
    {
        m_ProbeUpdateShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeUpdate.glsl");
        m_Tracer = RayTrace::CreateRayTracer(Configuration::DDIG_TraceType);
        m_Tracer->Init(m_Device);
        _InitVolumesAndPipeline(scene);

    }

    void DDGIRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        RayTraceVolumes(cmdList, targetFramebuffer, scene);
        UpdateVolumeProbe(cmdList, targetFramebuffer, scene);
    }

    void DDGIRenderer::RayTraceVolumes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        for (size_t i = 0; i < scene->GetLightProbeGrid().size(); i++){
       


            m_Tracer->Execute(cmdList, m_Volumes[i], m_Pipelines[i], scene);

        }
    }

    void DDGIRenderer::UpdateVolumeProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
    }

  

    void DDGIRenderer::_InitVolumesAndPipeline(Scene* scene)
    {
        for (size_t i = 0; i < scene->GetLightProbeGrid().size(); i++)
        {
            LightProbeGrid* grid = scene->GetLightProbeGrid()[i];




            DDGIPipelineInternal pipeline;
            // 1-pixel of padding surrounding each probe, 1-pixel padding surrounding entire texture for alignment.
            const int32_t irradianceWidth = (IrradianceOctSize + 2) * grid->ProbeCounts.x * grid->ProbeCounts.y + 2;
            const int32_t irradianceHeight = (IrradianceOctSize + 2) * grid->ProbeCounts.z + 2;
            const int32_t depthWidth = (DepthOctSize + 2) * grid->ProbeCounts.x * grid->ProbeCounts.y + 2;
            const int32_t depthHeight = (DepthOctSize + 2) * grid->ProbeCounts.z + 2;

           
            

            for (int32_t i = 0; i < 2; i++)
            {
                TextureDesc desc;
                desc.minFilter = FilterMode::Linear;
                desc.magFilter = FilterMode::Linear;
                desc.wrap = SamplerAddressMode::ClampToEdge;
                desc.format = Format::RGBA16_FLOAT;
                desc.dimension = TextureDimension::Texture2D;
                desc.width = irradianceWidth;
                desc.height = irradianceHeight;
                
                pipeline.irradiance[i] = m_Device->createTexture(desc);
                
                desc.format = Format::RG16_FLOAT;
                desc.dimension = TextureDimension::Texture2D;
                desc.width = depthWidth;
                desc.height = depthHeight;

                pipeline.depth[i] = m_Device->createTexture(desc);

         
            }


            uint32_t totalProbes = grid->ProbeCounts.x * grid->ProbeCounts.y * grid->ProbeCounts.z;

            TextureDesc desc;
            desc.minFilter = FilterMode::Linear;
            desc.magFilter = FilterMode::Linear;
            desc.wrap = SamplerAddressMode::ClampToEdge;
            desc.format = Format::RGBA16_FLOAT;
            desc.dimension = TextureDimension::Texture2D;
            desc.width = Configuration::DDIG_RaysPerProbe;
            desc.height = totalProbes;
            desc.access = TextureAccess::ReadWrite;
            pipeline.traceRadiance = m_Device->createTexture(desc);
            pipeline.traceDirectionDepth = m_Device->createTexture(desc); //TODO:: only one channel, can combine to radiance texture.w





            IrradianceVolume volume;
            volume.startPos = float4(grid->GridObj->GetComponent<BoundingBox>()->Get().GetMinP(), 1.0f);
            volume.probeDistance = grid->ProbeDistance;
            volume.width = grid->Width;
            volume.height = grid->Height;
            volume.depth = grid->Depth;
            volume.infiniteBounce = Configuration::DDIG_InfiniteBounce;
            volume.raysPerProbe = Configuration::DDIG_RaysPerProbe;
            volume.intensity = Configuration::DDIG_Intensity;
            volume.currentIrrdance = pipeline.irradiance[1- pipeline.pingPong];
            volume.currentDepth = pipeline.depth[1 - pipeline.pingPong];

            m_Volumes.push_back(volume);

            m_Pipelines.push_back(pipeline);
          
        }


       
    }


    void DDGIRenderer::_UpdateProbeIrradiance(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
    }

    void DDGIRenderer::_UpdateProbeDistance(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
    }



    void DDGIRenderer::ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {

    }

   /* void DDGIRenderer::GenerateGDF()
    {
    }*/

   
}
