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
#include "Renderer/SystemTextures.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrv.h"
#endif
#include "Renderer/Renderer.h"
namespace BlackPearl {
    extern MapManager* g_mapManager;

    DDGIRenderer::DDGIRenderer(IDevice* device) :
        BasicRenderer(device)
    {
    }

    void DDGIRenderer::Init(Scene* scene)
    {
	
		m_Tracer = RayTrace::CreateRayTracer(Configuration::DDIG_TraceType);
        m_Tracer->Init(m_Device);
        _InitVolumesAndPipeline(scene);
        _InitProbeUpdate(scene);
        _InitProbeGather(scene);
        _InitDeferredLighting();
        _InitProbeDebug();
      
    }

    void DDGIRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        RayTraceVolumes(cmdList, targetFramebuffer, scene);
        UpdateVolumeProbe(cmdList, targetFramebuffer, scene);
    }

    void DDGIRenderer::RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        _ProbeGather(commandList, targetFramebuffer, scene);
        _IndirectShading(commandList, targetFramebuffer, scene);
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
			m_LastWrite = writeIdx;
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


    void DDGIRenderer::RenderUI(IFramebuffer* framebuffer, IView* View) {
        ImGui::Begin("DDGI Settings");
        ImGui::Text("numVolumeInArea: %d ", m_UI.numVolumeInArea);
        ImGui::Text("currentAreaId: %d ", m_UI.currentAreaId);

        ImGui::End();

    }

    void DDGIRenderer::ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        cmdList->beginMarker("DDGI_ShowProbes");


       

        //todo:: use draw indirect!
        for (int i = 0; i < scene->GetLightProbeGrid().size();i++) {

       
            for (auto probe : scene->GetLightProbeGrid()[i]->GridObj->GetChildObjs()) {

   

                _RenderProbe(cmdList, targetFramebuffer, scene, probe, m_Volumes[i], m_Pipelines[i]);

            }

        }
        cmdList->endMarker();

    }
	
    void DDGIRenderer::_ProbeGather(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
        // 获取当前area 的 Volumes
        auto camPos = Renderer::GetSceneData()->CameraPosition;
        int areaId = g_mapManager->CalculateAreaId(camPos);
        m_UI.currentAreaId = areaId;
        std::vector<DDGIConstants> ddgiConsts;
        std::vector<DDGIPipelineInternal> pipelines;
        for (size_t i = 0; i < scene->GetLightProbeGrid().size(); i++) {


            auto volumePos = scene->GetLightProbeGrid()[i]->GridObj->GetComponent<Transform>()->GetPosition();
            int volumeAreaId = g_mapManager->CalculateAreaId(volumePos);

            if (volumeAreaId != areaId)
                continue;

            IrradianceVolume& volume = m_Volumes[i];

            DDGIConstants ddgiConst{};
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

            ddgiConsts.push_back(ddgiConst);
            pipelines.push_back(m_Pipelines[i]);


        }
        cmdList->writeBuffer(m_ProbeGatherBindings.ddgiSSBO, ddgiConsts.data(), sizeof(DDGIConstants) * ddgiConsts.size());

        DDGIAreaConstants areaConst{};
        areaConst.numVolumeInArea = ddgiConsts.size();
        cmdList->writeBuffer(m_ProbeGatherBindings.areaCB, &areaConst, sizeof(DDGIAreaConstants));
        BindingSetDesc bindingSetDesc;
		bindingSetDesc.bindings.push_back(BindingSetItem::Texture_UAV(0, m_ProbeGatherBindings.outputIndirectLighting));
		int slot = 0;
        for (size_t i = 0; i < pipelines.size(); i++)
        {
            uint32_t lastWrite = pipelines[i].pingPong;
            bindingSetDesc.bindings.push_back(BindingSetItem::Texture_SRV(slot++, pipelines[i].irradiance[m_LastWrite]));
            bindingSetDesc.bindings.push_back(BindingSetItem::Texture_SRV(slot++, pipelines[i].depth[m_LastWrite]));

        }
		while (slot < 8) {
			bindingSetDesc.bindings.push_back(BindingSetItem::Texture_SRV(slot++, SystemTexture::Get().blackTexture));
		}
        bindingSetDesc.bindings.push_back(BindingSetItem::Texture_SRV(8, SystemTexture::Get().SceneDepth));
        bindingSetDesc.bindings.push_back(BindingSetItem::Texture_SRV(9, SystemTexture::Get().GBufferA));
        bindingSetDesc.bindings.push_back(BindingSetItem::StructuredBuffer_SRV(2, m_ProbeGatherBindings.ddgiSSBO));
        bindingSetDesc.bindings.push_back(BindingSetItem::ConstantBuffer(3, m_ProbeGatherBindings.areaCB));

          
        m_ProbeGatherBindings.set = m_Device->createBindingSet(bindingSetDesc, m_ProbeGatherBindings.layout);

        m_UI.numVolumeInArea = areaConst.numVolumeInArea;

        cmdList->beginMarker("DDGI_DeferredIndirectLighting_ProbeGather");
        ComputeState computePSO;



        computePSO.bindings.push_back(m_ProbeGatherBindings.set);


        ComputePipelineDesc psoDesc;
        psoDesc.bindingLayouts.push_back(m_ProbeGatherBindings.layout);
        psoDesc.CS = m_ProbeGatherShader->GetComputeShader();

        if (!m_ProbeGatherPso) {
            m_ProbeGatherPso = m_Device->createComputePipeline(psoDesc);
        }
        computePSO.pipeline = m_ProbeGatherPso;

        cmdList->setComputeState(computePSO);
        uint32_t dispatchX = (SystemTexture::Get().SceneDepth.Get()->getDesc().width + 32)/ 32;
        uint32_t dispatchY = (SystemTexture::Get().SceneDepth.Get()->getDesc().height + 32) /32;


        cmdList->dispatch(dispatchX, dispatchY, 1);
        cmdList->endMarker();

	}

    void DDGIRenderer::_IndirectShading(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
   
        cmdList->beginMarker("DDGI_IndirectDeferredShading");
        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);


        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetFullScreenObj())[0];

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = targetFramebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        GraphicsPipelineDesc psoDesc;

        psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        psoDesc.depthStencilState.enableDepthTest();
        psoDesc.depthStencilState.disableDepthWrite();
        psoDesc.depthStencilState.disableStencil();

        psoDesc.blendState.alphaToCoverageEnable = false;

        for (auto& target : psoDesc.blendState.targets)
        {
            target.blendEnable = true;
            target.blendOp = BlendOp::Add;
            target.srcBlend = BlendFactor::One;
            target.destBlend = BlendFactor::One;
            target.srcBlendAlpha = BlendFactor::One;
            target.destBlendAlpha = BlendFactor::One;
            target.blendOpAlpha = BlendOp::Add;
        }


        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::None;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_DeferredDDGIShader->GetVertexShader();
        psoDesc.PS = m_DeferredDDGIShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_DeferredShadingBindingLayout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_DeferredShadingPso) {
            m_DeferredShadingPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_DeferredShadingPso;
        graphicsPSO.bindings.push_back(m_DeferredShadingBindingSet);
        graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;

        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();



        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

        cmdList->endMarker();

    }
    void DDGIRenderer::_InitProbeDebug()
    {
        m_ProbeDebugShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeDebug.glsl");
        //Debug Probe Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(9),
            RHIBindingLayoutItem::RT_Texture_SRV(2)



        };
        m_ProbeDebugBindings.layout = m_Device->createBindingLayout(layoutDesc);
        m_ProbeDebugBindings.probeCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(LightProbeConstants), "LightProbeConstants"));
        m_ProbeDebugBindings.volumeCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIConstants), "DDGIConstants"));

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
        std::vector<std::string> macros;
        macros.push_back("#define DEPTHPROBE_UPDATE");
        m_ProbeIrradianceUpdateShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeUpdate.glsl");
        m_ProbeDepthUpdateShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeUpdate.glsl", nullptr, &macros);




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

	void DDGIRenderer::_InitProbeGather(Scene* scene)
	{
        std::vector<std::string> macros;
        std::vector<std::string> extends;
#ifdef GE_PLATFORM_ANDROID
        if (RenderGraph::SupportPLS())
        {
            extends.push_back("#extension GL_EXT_shader_pixel_local_storage : require");
            extends.push_back("#extension GL_ARM_shader_framebuffer_fetch_depth_stencil : require");
            macros.push_back("#define USE_GLES_PLS 1");

        }
#endif
        macros.push_back("#define DEFERRED_SHADING_PASS 1");
        //extends.push_back("#extension GL_NV_compute_shader_derivatives: enable");
        m_ProbeGatherShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/probeGather.glsl", &extends, &macros);

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_UAV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3),
            RHIBindingLayoutItem::RT_Texture_SRV(4),
            RHIBindingLayoutItem::RT_Texture_SRV(5),
            RHIBindingLayoutItem::RT_Texture_SRV(6),
            RHIBindingLayoutItem::RT_Texture_SRV(7),
            RHIBindingLayoutItem::RT_Texture_SRV(8),
			RHIBindingLayoutItem::RT_Texture_SRV(9),

            RHIBindingLayoutItem::RT_StructuredBuffer_UAV(2),
            RHIBindingLayoutItem::RT_ConstantBuffer(3)
        };


        BufferDesc sceneObjsDesc;
        sceneObjsDesc.byteSize = sizeof(DDGIConstants) * scene->GetLightProbeGrid().size();
        sceneObjsDesc.structStride = sizeof(DDGIConstants);
        sceneObjsDesc.initialState = ResourceStates::UnorderedAccess;
        sceneObjsDesc.keepInitialState = true;
        sceneObjsDesc.canHaveUAVs = true;
        sceneObjsDesc.debugName = "DDGISSBO";
        m_ProbeGatherBindings.ddgiSSBO = m_Device->createBuffer(sceneObjsDesc);

        m_ProbeGatherBindings.areaCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIAreaConstants), "DDGIAreaConstants"));
		m_ProbeGatherBindings.layout = m_Device->createBindingLayout(layoutDesc);

		TextureDesc desc;
		desc.minFilter = FilterMode::Linear;
		desc.magFilter = FilterMode::Linear;
		desc.wrap = SamplerAddressMode::ClampToEdge;
		desc.format = Format::RGBA16_FLOAT;
		desc.dimension = TextureDimension::Texture2D;
		desc.width = SystemTexture::Get().GetBackBuffer().Get()->getDesc().width;
		desc.height = SystemTexture::Get().GetBackBuffer().Get()->getDesc().height;
		desc.access = TextureAccess::ReadWrite;
		m_ProbeGatherBindings.outputIndirectLighting = m_Device->createTexture(desc);
	}

    void DDGIRenderer::_InitDeferredLighting() {


        //Deferred Shading IBL Material
        std::vector<std::string> extends;
        std::vector<std::string> macros;
#ifdef GE_PLATFORM_ANDROID
        if (RenderGraph::SupportPLS())
        {
            extends.push_back("#extension GL_EXT_shader_pixel_local_storage : require");
            extends.push_back("#extension GL_ARM_shader_framebuffer_fetch_depth_stencil : require");
            macros.push_back("#define USE_GLES_PLS 1");

        }
#endif
        macros.push_back("#define DEFERRED_SHADING_PASS 1");
        m_DeferredDDGIShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_ddgi.glsl", &extends, &macros);;
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3),
            RHIBindingLayoutItem::RT_Texture_SRV(4),
            RHIBindingLayoutItem::RT_Texture_SRV(5),
        };
        m_DeferredShadingBindingLayout = m_Device->createBindingLayout(layoutDesc);

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(0, SystemTexture::Get().SceneColor, "SceneColor"),
            BindingSetItem::Texture_SRV(1, SystemTexture::Get().GBufferA, "GBufferA"),
            BindingSetItem::Texture_SRV(2, SystemTexture::Get().GBufferB, "GBufferB"),
            BindingSetItem::Texture_SRV(3, SystemTexture::Get().GBufferC, "GBufferC"),
            BindingSetItem::Texture_SRV(4, SystemTexture::Get().SceneDepth, "SceneDepth"),
            BindingSetItem::Texture_SRV(5, m_ProbeGatherBindings.outputIndirectLighting, "DDGIIndirectLight"),
        };
        m_DeferredShadingBindingSet = m_Device->createBindingSet(bindingSetDesc, m_DeferredShadingBindingLayout);

    }


    void DDGIRenderer::_UpdateProbeIrradiance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeUpdateBindings& binidngs,  Scene* scene)
    {
		cmdList->beginMarker("DDGI_Update_ProbeIrradiance");
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

    void DDGIRenderer::_UpdateProbeDistance(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, const ProbeUpdateBindings& binidngs, Scene* scene)
    {
		cmdList->beginMarker("DDGI_Update_ProbeDistance");


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

	void DDGIRenderer::_FillUpdateProbeShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume, const ProbeUpdateBindings& binidngs)
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



    void DDGIRenderer::_RenderProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* probe, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline)
    {
        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);



        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(probe)[0];

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = targetFramebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        GraphicsPipelineDesc psoDesc;

        psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        psoDesc.depthStencilState.enableDepthTest();
        psoDesc.depthStencilState.disableDepthWrite();
        psoDesc.depthStencilState.disableStencil();

        psoDesc.blendState.alphaToCoverageEnable = false;
        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::None;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_ProbeDebugShader->GetVertexShader();
        psoDesc.PS = m_ProbeDebugShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_ProbeDebugBindings.layout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_ProbeDebugPso) {
            m_ProbeDebugPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(8, m_ProbeDebugBindings.probeCB),
            BindingSetItem::ConstantBuffer(9, m_ProbeDebugBindings.volumeCB),

            BindingSetItem::Texture_SRV(2, pipeline.irradiance[m_LastWrite], "irradiance_input"),

        };
        BindingSetHandle    set = m_Device->createBindingSet(bindingSetDesc, m_ProbeDebugBindings.layout);


        
        DDGIConstants ddgiConst{};
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




        cmdList->writeBuffer(m_ProbeDebugBindings.volumeCB, &ddgiConst, sizeof(DDGIConstants));

        graphicsPSO.pipeline = m_ProbeDebugPso;
        graphicsPSO.bindings.push_back(set);
        graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;
        /*for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingLayouts.size(); ++j) {
            psoDesc.bindingLayouts.push_back(shaderParms[ShaderType::Pixel].bindingLayouts[j]);
        }

        for (int j = 0; j < shaderParms[ShaderType::Pixel].bindingSets.size(); ++j) {
            graphicsPSO.bindings.push_back(shaderParms[ShaderType::Pixel].bindingSets[j]);
        }*/

        /*GE_ERROR_JUDGE();
        SetupMaterial(drawItem.material, drawItem.cullMode, psoDesc, graphicsPSO);*/
        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();



        LightProbeConstants probeConst{};
        //probeConst.probeType = (int)ProbeType::DIFFUSE_PROBE;
        //probeConst.pos = Math::ToFloat3(probe->GetComponent<Transform>()->GetPosition());
        probe->GetComponent<LightProbe>()->FillLightProbeConstants(probe->GetComponent<LightProbe>()->GetType(), Math::ToFloat3(probe->GetComponent<Transform>()->GetPosition()), probeConst);
        cmdList->writeBuffer(m_ProbeDebugBindings.probeCB, &probeConst, sizeof(LightProbeConstants));



        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

    }


   
}
