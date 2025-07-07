#include "pch.h"
#include "Renderer/MasterRenderer/GI/DDGITrace/SDFTrace.h"
#include "Renderer/DDGI/IrradianceVolume.h"
#include "Renderer/DDGI/DDGIPipelineInternal.h"
#include "hlsl/core/ddgi_cb.h"
#include "hlsl/core/sdf_cb.h"
#include "hlsl/core/voxel_cb.h"

#include <glm\gtc\quaternion.hpp>
#include "Application.h"
#include "RHI/Common/RHIUtils.h"
#include "Renderer/SystemTextures.h"
#include "Math/vector.h"

namespace BlackPearl {
    extern MapManager* g_mapManager;

    void SDFTrace::Init(IDevice* device)
    {
        m_Device = device;
        m_SDFGIShader = DBG_NEW MaterialShader("assets/shaders/glsl/ddgi/sdfTrace.glsl");
       
        _InitBindingsSDF();
        _InitBindingsVoxel();
        _InitBindingsSkybox();
        _InitBindingsDDGI();
        _InitBindingsOutput();
    

    }

    void SDFTrace::Execute(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene)
    {
        cmdList->beginMarker("SDFGITrace");
        ComputeState computePSO;
        computePSO.pipeline = m_SDFTracePso;


        math::uint4 probeCnt = uint4(
            volume.width / volume.probeDistance + 1,
            volume.height / volume.probeDistance + 1,
            volume.depth / volume.probeDistance + 1,
            1);
        float raysPerProbe = volume.raysPerProbe;

        
        _FillShaderParameters(cmdList, volume, pipeline, scene);

        computePSO.bindings.push_back(m_Bindings.gdfSet);
        computePSO.bindings.push_back(m_Bindings.voxelSet);
        computePSO.bindings.push_back(m_Bindings.skyboxSet);
        computePSO.bindings.push_back(m_Bindings.ddgiSet);
        computePSO.bindings.push_back(m_Bindings.outputSet);

        ComputePipelineDesc psoDesc;
        psoDesc.bindingLayouts.push_back(m_Bindings.gdfLayout);
        psoDesc.bindingLayouts.push_back(m_Bindings.voxelLayout);
        psoDesc.bindingLayouts.push_back(m_Bindings.skyboxLayout);
        psoDesc.bindingLayouts.push_back(m_Bindings.ddgiLayout);
        psoDesc.bindingLayouts.push_back(m_Bindings.outputLayout);

        psoDesc.CS = m_SDFGIShader->GetComputeShader();

        if (!m_SDFTracePso) {
            m_SDFTracePso = m_Device->createComputePipeline(psoDesc);
        }
        computePSO.pipeline = m_SDFTracePso;

        cmdList->setComputeState(computePSO);
        uint32_t dispatchX = std::ceil(raysPerProbe / 16.f);
        uint32_t dispatchY = probeCnt.x * probeCnt.y * probeCnt.z;


        cmdList->dispatch(dispatchX, dispatchY, 1);
        cmdList->endMarker();

    }
    void SDFTrace::_InitBindingsSDF()
    {
        SystemTexture& systemTex = SystemTexture::Get();

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3),
            RHIBindingLayoutItem::RT_ConstantBuffer(0)

        };
        m_Bindings.gdfLayout = m_Device->createBindingLayout(layoutDesc);
        m_Bindings.gdfCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(GlobalSDFTraceConstants), "GlobalSDFTraceConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(0, systemTex.SceneGlobalDF.Clipmaps[0].MipTexture, "clip0"),
            BindingSetItem::Texture_SRV(1, systemTex.SceneGlobalDF.Clipmaps[1].MipTexture, "clip1"),
            BindingSetItem::Texture_SRV(2, systemTex.SceneGlobalDF.Clipmaps[2].MipTexture, "clip2"),
            BindingSetItem::Texture_SRV(3, systemTex.blackTexture, "clip3"),
            BindingSetItem::ConstantBuffer(0, m_Bindings.gdfCB)
        };
        m_Bindings.gdfSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.gdfLayout);
    }

    void SDFTrace::_InitBindingsVoxel()
    {
        SystemTexture& systemTex = SystemTexture::Get();

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(5),
            RHIBindingLayoutItem::RT_ConstantBuffer(1)

        };
        m_Bindings.voxelLayout = m_Device->createBindingLayout(layoutDesc);
        m_Bindings.voxelCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(GlobalSDFTraceConstants), "GlobalSDFTraceConstants"));


        //BindingSetDesc bindingSetDesc;
        //bindingSetDesc.bindings = {
        //    BindingSetItem::Texture_SRV(0, systemTex.SceneGlobalDF.Clipmaps[0].MipTexture, "clip0"),
        //    BindingSetItem::Texture_SRV(1, systemTex.SceneGlobalDF.Clipmaps[1].MipTexture, "clip1"),
        //    BindingSetItem::Texture_SRV(2, systemTex.SceneGlobalDF.Clipmaps[2].MipTexture, "clip2"),
        //    BindingSetItem::Texture_SRV(3, systemTex.blackTexture, "clip3"),
        //    BindingSetItem::ConstantBuffer(4, m_Bindings.gdfCB)
        //};
        //m_Bindings.gdfSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.gdfLayout);
    }

    void SDFTrace::_InitBindingsSkybox()
    {
        SystemTexture& systemTex = SystemTexture::Get();

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(7),
            RHIBindingLayoutItem::RT_Texture_SRV(8),
            RHIBindingLayoutItem::RT_Texture_SRV(9)

        };
        m_Bindings.skyboxLayout = m_Device->createBindingLayout(layoutDesc);

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(7, SystemTexture::Get().SkyboxTexture0, "SkyboxTexture0"),
            BindingSetItem::Texture_SRV(8, SystemTexture::Get().SkyboxTexture1, "SkyboxTexture1"),
            BindingSetItem::Texture_SRV(9, SystemTexture::Get().SkyboxTexture2, "SkyboxTexture2"),
        };
        m_Bindings.skyboxSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.skyboxLayout);
    }

    void SDFTrace::_InitBindingsDDGI()
    {
        SystemTexture& systemTex = SystemTexture::Get();

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
           RHIBindingLayoutItem::RT_ConstantBuffer(2),
           RHIBindingLayoutItem::RT_ConstantBuffer(3)

        };
        m_Bindings.ddgiLayout = m_Device->createBindingLayout(layoutDesc);
        m_Bindings.ddgiCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIConstants), "DDGIConstants"));
        m_Bindings.ddgiRayCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DDGIRayConstants), "DDGIRayConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(2, m_Bindings.ddgiCB),
            BindingSetItem::ConstantBuffer(3, m_Bindings.ddgiRayCB)
        };
        m_Bindings.ddgiSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.ddgiLayout);
    }

    void SDFTrace::_InitBindingsOutput()
    {
        SystemTexture& systemTex = SystemTexture::Get();

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Compute;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_UAV(0),
            RHIBindingLayoutItem::RT_Texture_UAV(1)

        };
        m_Bindings.outputLayout = m_Device->createBindingLayout(layoutDesc);

        
    }


    void SDFTrace::_FillShaderParameters(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene)
    {
        _FillGDF(cmdList, volume, pipeline, scene);
        _FillDDGI(cmdList, volume, pipeline, scene);

        auto camPos = Renderer::GetSceneData()->CameraPosition;
        _FillVoxel(cmdList, camPos);

        _FillOutputImage(pipeline);

    }

    void SDFTrace::_FillDDGI(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene)
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

        cmdList->writeBuffer(m_Bindings.ddgiCB, &ddgiConst, sizeof(DDGIConstants));


        DDGIRayConstants ddgiRayConst{};
        auto vec3 = glm::normalize(glm::vec3(Math::Rand_F1(),
            Math::Rand_F1(),
            Math::Rand_F1()));

        //生成随机旋转角度
        ddgiRayConst.randomOrientation = Math::ToFloat4x4(
            glm::mat4_cast(
                glm::angleAxis(Math::Rand_F1() * float(Math::PI) * 2.0f, vec3)));
        ddgiRayConst.numLights = scene->GetLightSources()->GetLightsNum();
        ddgiRayConst.infiniteBounces = (volume.infiniteBounce && Application::s_CurrentFrameNum != 0) ? 1 : 0;
        ddgiRayConst.intensity = volume.intensity;
        ddgiRayConst.numFrames = Application::s_CurrentFrameNum;

        cmdList->writeBuffer(m_Bindings.ddgiRayCB, &ddgiRayConst, sizeof(DDGIRayConstants));
    }

    void SDFTrace::_FillGDF(ICommandList* cmdList, const IrradianceVolume& volume, const DDGIPipelineInternal& pipeline, Scene* scene)
    {
        SystemTexture& systemTex = SystemTexture::Get();

        GlobalSDFTraceConstants gdfTraceConst{};
        GE_ASSERT(systemTex.SceneGlobalDF.NumClipMapLevels <= MAX_CLIP_NUM);
        for (int i = 0; i < MAX_CLIP_NUM; i++){
            gdfTraceConst.clipPosDistance[i] = math::float4(0.0);

        }
        gdfTraceConst.clipVoxelSize = math::float4(0.0);

        for (int i = 0; i < systemTex.SceneGlobalDF.NumClipMapLevels; i++) {
            gdfTraceConst.clipPosDistance[i] = math::float4(systemTex.SceneGlobalDF.Clipmaps[i].Center, systemTex.SceneGlobalDF.Clipmaps[i].Extent * 0.5f);
            gdfTraceConst.clipVoxelSize[i] = systemTex.SceneGlobalDF.Clipmaps[i].VoxelSize;
        }

        gdfTraceConst.dimension = systemTex.SceneGlobalDF.ClipDim;

        cmdList->writeBuffer(m_Bindings.gdfCB, &gdfTraceConst, sizeof(GlobalSDFTraceConstants));

    }

    void SDFTrace::_FillVoxel(ICommandList* cmdList, const glm::vec3 camPos)
    {
       // GE_ASSERT(Configuration::bUseVoxel);
        uint32_t areaId = g_mapManager->CalculateAreaId(camPos);
        uint32_t voxelId = g_mapManager->GetArea(areaId)->GetVoxelId();
        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
                    BindingSetItem::Texture_SRV(5, SystemTexture::Get().SceneVoxels[voxelId].voxelTexture,"VoxelTexture"),
                    BindingSetItem::ConstantBuffer(1, m_Bindings.voxelCB)
        };


        m_Bindings.voxelSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.voxelLayout);

        VoxelConstants voxelConstants{};
        voxelConstants.areaExtent = g_mapManager->GetArea(areaId)->GetExtend();
        voxelConstants.center = SystemTexture::Get().SceneVoxels[voxelId].center;
        voxelConstants.dimension = SystemTexture::Get().SceneVoxels[voxelId].dimension;

        cmdList->writeBuffer(m_Bindings.voxelCB, &voxelConstants, sizeof(VoxelConstants));



    }
    void SDFTrace::_FillOutputImage(const DDGIPipelineInternal& pipeline)
    {
        m_Bindings.traceRadiance = pipeline.traceRadiance;
        m_Bindings.traceDirectionDistance = pipeline.traceDirectionDepth;


        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_UAV(0, m_Bindings.traceRadiance, "traceRadiance"),
            BindingSetItem::Texture_UAV(1, m_Bindings.traceDirectionDistance, "traceDirectionDistance"),

        };
        m_Bindings.outputSet = m_Device->createBindingSet(bindingSetDesc, m_Bindings.outputLayout);
    }
}

