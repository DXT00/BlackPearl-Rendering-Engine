#include "pch.h"
#include "Renderer/MasterRenderer/GI/DDGIRenderer.h"
#include "Renderer/DeviceManager.h"
#include "Map/MapManager.h"
#include "LightProbes/LightProbeGrid.h"
#include "RHI/Common/RHIUtils.h"
#include "hlsl/core/ddgi_cb.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/RayTrace.h"
#include "Component/BoundingBoxComponent/BoundingBox.h"
#include "Application.h"
namespace BlackPearl {
    extern MapManager* g_mapManager;

    DDGIRenderer::DDGIRenderer(IDevice* device) :
        BasicRenderer(device)
    {
    }

    void DDGIRenderer::Init(Scene* scene)
    {
		std::vector<std::string> macros;
		macros.push_back("#define DEPTHPROBE_UPDATE");
		m_ProbeIrradianceUpdateShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeUpdate.glsl");
		m_ProbeDepthUpdateShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeUpdate.glsl", nullptr, &macros);

		m_Tracer = RayTrace::CreateRayTracer(Configuration::DDIG_TraceType);
        m_Tracer->Init(m_Device);
        _InitVolumesAndPipeline(scene);
        _InitProbeUpdate(scene);
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
		for (size_t i = 0; i < scene->GetLightProbeGrid().size(); i++) {


			auto writeIdx = 1 - m_Pipelines[i].pingPong;
			auto readIdx = m_Pipelines[i].pingPong;

			BindingSetDesc bindingSetDesc;
			bindingSetDesc.bindings = {
				BindingSetItem::Texture_UAV(0, m_Pipelines[i].irradiance[writeIdx], "irradiance_output"),
				BindingSetItem::Texture_UAV(1, m_Pipelines[i].depth[writeIdx], "depth_output"),

				BindingSetItem::Texture_SRV(0, m_Pipelines[i].irradiance[readIdx], "irradiance_input"),
				BindingSetItem::Texture_SRV(1, m_Pipelines[i].depth[readIdx], "depth_input"),
				BindingSetItem::Texture_SRV(2, m_Pipelines[i].traceRadiance, "traceRadiance"),
				BindingSetItem::Texture_SRV(3,  m_Pipelines[i].traceDirectionDepth, "traceDirectionDepth"),
				BindingSetItem::ConstantBuffer(0, m_ProbeUpdateBindings[i].ddgiCB),
				BindingSetItem::ConstantBuffer(1, m_ProbeUpdateBindings[i].frameCB)

			};
			m_ProbeUpdateBindings[i].set = m_Device->createBindingSet(bindingSetDesc, m_ProbeUpdateBindings[i].layout);

			_UpdateProbeIrradiance(cmdList, m_Volumes[i], m_Pipelines[i], m_ProbeUpdateBindings[i], scene);
			_UpdateProbeDistance(cmdList, m_Volumes[i], m_Pipelines[i], m_ProbeUpdateBindings[i], scene);

			m_Pipelines[i].pingPong = 1.0 - m_Pipelines[i].pingPong;
		}

    }

  

    void DDGIRenderer::_InitVolumesAndPipeline(Scene* scene)
    {
        for (size_t i = 0; i < scene->GetLightProbeGrid().size(); i++)
        {
            LightProbeGrid* grid = scene->GetLightProbeGrid()[i];




            DDGIPipelineInternal pipeline;
            // 1-pixel of padding surrounding each probe, 1-pixel padding surrounding entire texture for alignment.
       /*     const int32_t irradianceWidth = (IrradianceOctSize + 2) * grid->ProbeCounts.x * grid->ProbeCounts.y + 2;
            const int32_t irradianceHeight = (IrradianceOctSize + 2) * grid->ProbeCounts.z + 2;
            const int32_t depthWidth = (DepthOctSize + 2) * grid->ProbeCounts.x * grid->ProbeCounts.y + 2;
            const int32_t depthHeight = (DepthOctSize + 2) * grid->ProbeCounts.z + 2;*/

			const int32_t irradianceWidth = (IrradianceOctSize) * grid->ProbeCounts.x * grid->ProbeCounts.y;
			const int32_t irradianceHeight = (IrradianceOctSize) * grid->ProbeCounts.z ;
			const int32_t depthWidth = (DepthOctSize) * grid->ProbeCounts.x * grid->ProbeCounts.y;
			const int32_t depthHeight = (DepthOctSize) * grid->ProbeCounts.z;
            

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

    void DDGIRenderer::_InitProbeUpdate(Scene* scene)
    {
		m_ProbeUpdateBindings.resize(m_Volumes.size());
        for (size_t i = 0; i < m_Volumes.size(); i++)
        {

		
            RHIBindingLayoutDesc layoutDesc;
            layoutDesc.visibility = ShaderType::Compute;
            layoutDesc.bindings = {
                RHIBindingLayoutItem::RT_Texture_UAV(0),
                RHIBindingLayoutItem::RT_Texture_UAV(1),
                RHIBindingLayoutItem::RT_Texture_SRV(0),
                RHIBindingLayoutItem::RT_Texture_SRV(1),
                RHIBindingLayoutItem::RT_Texture_SRV(2),
                RHIBindingLayoutItem::RT_Texture_SRV(3),
                RHIBindingLayoutItem::RT_ConstantBuffer(0),
				RHIBindingLayoutItem::RT_ConstantBuffer(1)


            };
			m_ProbeUpdateBindings[i].layout = m_Device->createBindingLayout(layoutDesc);
			m_ProbeUpdateBindings[i].ddgiCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIConstants), "DDGIConstants"));
			m_ProbeUpdateBindings[i].frameCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIFrameConstants), "DDGIFrameConstants"));
			
			

        }

    }


    void DDGIRenderer::_UpdateProbeIrradiance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeBindings& binidngs,  Scene* scene)
    {
		cmdList->beginMarker("Update_ProbeIrradiance");
		ComputeState computePSO;
		computePSO.pipeline = m_ProbeIrradianceUpdatePso;


		math::uint4 probeCnt = uint4(
			volume.width / volume.probeDistance + 1,
			volume.height / volume.probeDistance + 1,
			volume.depth / volume.probeDistance + 1,
			1);
		float raysPerProbe = volume.raysPerProbe;


		_FillUpdateProbeShaderParameters(cmdList, volume, binidngs);

		computePSO.bindings.push_back(binidngs.set);


		ComputePipelineDesc psoDesc;
		psoDesc.bindingLayouts.push_back(binidngs.layout);
		psoDesc.CS = m_ProbeIrradianceUpdateShader->GetComputeShader();

		if (!m_ProbeIrradianceUpdatePso) {
			m_ProbeIrradianceUpdatePso = m_Device->createComputePipeline(psoDesc);
		}
		computePSO.pipeline = m_ProbeIrradianceUpdatePso;

		cmdList->setComputeState(computePSO);
		uint32_t dispatchX = probeCnt.x * probeCnt.y;
		uint32_t dispatchY =  probeCnt.z;


		cmdList->dispatch(dispatchX, dispatchY, 1);
		cmdList->endMarker();
    }

    void DDGIRenderer::_UpdateProbeDistance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeBindings& binidngs, Scene* scene)
    {
		cmdList->beginMarker("Update_ProbeDistance");
		ComputeState computePSO;
		computePSO.pipeline = m_ProbeDiatanceUpdatePso;


		math::uint4 probeCnt = uint4(
			volume.width / volume.probeDistance + 1,
			volume.height / volume.probeDistance + 1,
			volume.depth / volume.probeDistance + 1,
			1);
		float raysPerProbe = volume.raysPerProbe;

		
		_FillUpdateProbeShaderParameters(cmdList, volume, binidngs);
      
		computePSO.bindings.push_back(binidngs.set);
		

		ComputePipelineDesc psoDesc;
		psoDesc.bindingLayouts.push_back(binidngs.layout);
		psoDesc.CS = m_ProbeDepthUpdateShader->GetComputeShader();

		if (!m_ProbeDiatanceUpdatePso) {
			m_ProbeDiatanceUpdatePso = m_Device->createComputePipeline(psoDesc);
		}
		computePSO.pipeline = m_ProbeDiatanceUpdatePso;

		cmdList->setComputeState(computePSO);
		uint32_t dispatchX = probeCnt.x * probeCnt.y;
		uint32_t dispatchY = probeCnt.z;



		cmdList->dispatch(dispatchX, dispatchY, 1);
		cmdList->endMarker();
    }

	void DDGIRenderer::_FillUpdateProbeShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume, const ProbeBindings& binidngs)
	{
		DDGIConstants ddgiConst{};
		// 1-pixel of padding surrounding each probe, 1-pixel padding surrounding entire texture for alignment.
		const int32_t irradianceWidth = volume.currentIrrdance.Get()->getDesc().width;
		const int32_t irradianceHeight = volume.currentIrrdance.Get()->getDesc().height;
		const int32_t depthWidth = volume.currentDepth.Get()->getDesc().width;
		const int32_t depthHeight = volume.currentDepth.Get()->getDesc().height;

		ddgiConst.startPosition = volume.startPos;
		ddgiConst.probeDistance = volume.probeDistance;
		ddgiConst.probeCounts = int4(
			volume.width / volume.probeDistance + 1,
			volume.height / volume.probeDistance + 1,
			volume.depth / volume.probeDistance + 1,
			1);
		ddgiConst.maxDistance = volume.probeDistance * 1.5f;
		ddgiConst.depthSharpness = volume.depthSharpness;
		ddgiConst.hysteresis = volume.hysteresis;
		ddgiConst.normalBias = volume.normalBias;
		ddgiConst.ddgiGamma = volume.ddgiGamma;


		ddgiConst.irradianceProbeSideLength = IrradianceOctSize;
		ddgiConst.irradianceTextureWidth = irradianceWidth;
		ddgiConst.irradianceTextureHeight = irradianceHeight;

		ddgiConst.depthProbeSideLength = DepthOctSize;
		ddgiConst.depthTextureWidth = depthWidth;
		ddgiConst.depthTextureHeight = depthHeight;
		ddgiConst.raysPerProbe = volume.raysPerProbe;

		cmdList->writeBuffer(binidngs.ddgiCB, &ddgiConst, sizeof(DDGIConstants));

		DDGIFrameConstants frameConst{};
		frameConst.firstFrame = (Application::s_CurrentFrameNum == 0) ? 1 : 0;
		cmdList->writeBuffer(binidngs.frameCB, &frameConst, sizeof(DDGIFrameConstants));

	}



    void DDGIRenderer::ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {

    }

   /* void DDGIRenderer::GenerateGDF()
    {
    }*/

   
}
