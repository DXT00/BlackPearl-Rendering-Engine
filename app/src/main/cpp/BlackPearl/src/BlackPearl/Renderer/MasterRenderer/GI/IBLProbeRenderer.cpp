//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/GI/IBLProbeRenderer.h"
#include "RHI/RHIDevice.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#include "LightProbes/SphericalHarmonics.h"
#include "Component/CameraComponent/PerspectiveCamera.h"
#include "ObjectManager/ObjectManager.h"
#include "hlsl/core/prefilter_map_cb.h"
#include "Application.h"
#include "Renderer/SystemTextures.h"

#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLTexture.h"
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLDrv.h"
#endif
#include "Timestep/TimeCounter.h"
#include "RHI/Common/RHIUtils.h"
#include "Renderer/RenderGraph/RenderGraph.h"
namespace BlackPearl
{

    extern ObjectManager* g_objectManager;
    float IBLProbeRenderer::s_GICoeffs = 0.2f;
    IBLProbeRenderer::IBLProbeRenderer(IDevice* device) :
        BasicRenderer(device) {
        m_EnvironmentMapRenderer = DBG_NEW ForwardShadingRenderer(device);
        m_EnvironmentMapSkyboxRenderer = DBG_NEW SkyboxRenderer(device);
    }

   
    void IBLProbeRenderer::Init(Scene* scene)
    {

        _InitProbeBake();

        _InitDeferredLighting();

        _InitProbeDebug();

        m_IsInitial = true;

    }


    void IBLProbeRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        GE_ASSERT(m_IsInitial, "please initial IBLProbesRenderer first! IBLProbesRenderer::init()");

        for (const auto &it : scene->GetDiffuseLightProbes()) {

            Object* probe = it;
            UpdateDiffuseProbesMap(cmdList, targetFramebuffer, scene, it);

        }

        for (const auto& it : scene->GetReflectLightProbes())
        {
            Object* probe = it;
            UpdateReflectionProbesMap(cmdList, targetFramebuffer, scene, it);

        }
    }

    void IBLProbeRenderer::UpdateDiffuseProbesMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* diffuseProbe)
    {
        TextureHandle environmentMap = RenderEnvironmerntCubeMaps(cmdList, scene, diffuseProbe);
        RenderSHImage(diffuseProbe, environmentMap);

    }


    void IBLProbeRenderer::UpdateReflectionProbesMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* reflectionProbe)
    {
        if (reflectionProbe->GetComponent<LightProbe>()->GetDynamicSpecularMap()) {
            TextureHandle environmentMap = RenderEnvironmerntCubeMaps(cmdList, scene, reflectionProbe);
            RenderSpecularPrefilterMap(cmdList, targetFramebuffer, scene->GetLightSources(), reflectionProbe, environmentMap);
        }


    }

    void IBLProbeRenderer::RenderSpecularPrefilterMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, const LightSources* lightSources, Object* probe, TextureHandle environmentMap)
    {
        glm::vec3 center = probe->GetComponent<Transform>()->GetPosition();
        UpdateProbeCamera(probe);
        //probe->UpdateCamera();
        auto camera = m_ProbeCamera;//probe->GetCamera();
        auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

        auto projection = cameraComponent->GetProjectionMatrix();
        std::vector<glm::mat4> ProbeView = {
            glm::lookAt(center, center - camera->Front(),-camera->Up()),
            glm::lookAt(center, center + camera->Front(),-camera->Up()),
            glm::lookAt(center, center + camera->Up(),-camera->Right()),
            glm::lookAt(center, center - camera->Up(),camera->Right()),
            glm::lookAt(center, center - camera->Right(), -camera->Up()),
            glm::lookAt(center, center + camera->Right(), -camera->Up())

        };
        std::vector<glm::mat4> ProbeProjectionViews = {
            projection * ProbeView[0],
            projection * ProbeView[1],
            projection * ProbeView[2],
            projection * ProbeView[3],
            projection * ProbeView[4],
            projection * ProbeView[5]
        };
        auto specularIrradianceMap = probe->GetComponent<LightProbe>()->GetSpecularPrefilterCubeMap();




        //Skybox Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(2),
            RHIBindingLayoutItem::RT_Texture_SRV(1)
        };
        BindingLayoutHandle bindingLayout = m_Device->createBindingLayout(layoutDesc);

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(2, m_SpecularPrefilterCB),
            BindingSetItem::Texture_SRV(1, environmentMap.Get(), "environmentCubeMap")

        };
        BindingSetHandle bindingSet = m_Device->createBindingSet(bindingSetDesc, bindingLayout);



        //m_SpecularPrefilterShader->Bind();
        //GE_ERROR_JUDGE();

        //m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentMap", 0);
        //glActiveTexture(GL_TEXTURE0);
        //GE_ERROR_JUDGE();
        //environmentMap->Bind();
        ////probe->GetHdrEnvironmentCubeMap()->Bind();
        //GE_ERROR_JUDGE();

        //std::shared_ptr<FrameBuffer> frameBuffer = std::make_shared<FrameBuffer>();
        //GE_ERROR_JUDGE();

        //frameBuffer->Bind();
        //frameBuffer->AttachCubeMapColorTexture(0, specularIrradianceMap);
        //frameBuffer->AttachRenderBuffer(specularIrradianceMap->getDesc().width, specularIrradianceMap->getDesc().height);
        //GE_ERROR_JUDGE();

        //frameBuffer->Bind();
        //GE_ERROR_JUDGE();

        unsigned int maxMipMapLevels = 5;
        for (unsigned int mip = 0; mip < maxMipMapLevels; mip++)
        {
            //resize framebuffer according to mipmap-level size;
          /*  unsigned int mipWidth = specularIrradianceMap->getDesc().width * std::pow(0.5, mip);
            unsigned int mipHeight = specularIrradianceMap->getDesc().height * std::pow(0.5, mip);
            frameBuffer->BindRenderBuffer();
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
            glViewport(0, 0, mipWidth, mipHeight);
            GE_ERROR_JUDGE();*/

            float roughness = (float)mip / (float)(maxMipMapLevels - 1);


            PrefilterConstants prefilterConstants{};
            prefilterConstants.roughness = roughness;
            prefilterConstants.environmentCubeMapDim = environmentMap->getDesc().width;
          
            cmdList->writeBuffer(m_SpecularPrefilterCB, &prefilterConstants, sizeof(PrefilterConstants));

            std::vector<int32_t> InArraySlice = { 0,1,2,3,4,5 };
            std::vector<uint8_t> InMipIndex(6, mip);
            TextureHandle depthBuffer = probe->GetComponent<LightProbe>()->GetDepthBuffer();

            FRHIRenderPassInfo RPInfo(
                6,
                specularIrradianceMap,
                ERenderTargetActions::Clear_Store,
                InMipIndex.data(),
                InArraySlice.data(),
                depthBuffer,
                EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil
            );

            cmdList->beginRenderPass(RPInfo, "SpecularPrefilterMap_Pass_mip" + std::to_string(mip));


            //m_SpecularPrefilterShader->Bind();
            //m_SpecularPrefilterShader->SetUniform1f("u_roughness", roughness);
            //m_SpecularPrefilterShader->SetUniform1i("u_EnvironmentCubeMapDim", environmentMap->getDesc().width);

            //GE_ERROR_JUDGE();


            for (unsigned int i = 0; i < 6; i++)
            {

                SceneData* view = DBG_NEW SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection, probe->GetComponent<Transform>()->GetPosition(),{},cameraComponent->Front(),*lightSources });
                //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, static_cast<Texture*>(specularIrradianceMap.Get())->GetRendererID(), mip);
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                //GE_ERROR_JUDGE();

                //probe->GetComponent<MeshRenderer>()->SetShaders(m_SpecularPrefilterShader);
                ////GE_ERROR_JUDGE();
                //m_SpecularPrefilterShader->Bind();
                //DrawObject(probe, m_SpecularPrefilterShader, scene);
                //
                SetupView(cmdList, view, nullptr);

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
                psoDesc.rasterState.cullMode = RasterCullMode::Back;
                psoDesc.primType = PrimitiveType::TriangleList;
                psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

                psoDesc.VS = m_SpecularPrefilterShader->GetVertexShader();
                psoDesc.PS = m_SpecularPrefilterShader->GetPixelShader();
                psoDesc.bFromPSOFileCache = false;
                psoDesc.bindingLayouts.push_back(bindingLayout);
                psoDesc.bindingLayouts.push_back(m_ViewBindinglayout); //todo::


                if (!m_SpecularPrefilterPso) {
                    m_SpecularPrefilterPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
                }
                graphicsPSO.pipeline = m_SpecularPrefilterPso;
                graphicsPSO.bindings.push_back(bindingSet);
                graphicsPSO.bindings.push_back(m_ViewBindingset);  //todo::
                graphicsPSO.inputLayout = psoDesc.inputLayout;

                SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
                GE_ERROR_JUDGE();

                cmdList->setGraphicsState(graphicsPSO);

                Draw(cmdList, drawItem);


                    
                delete view;
                view = nullptr;

            }
        }
        ////	probe->GetHdrEnvironmentCubeMap()->UnBind();
        //environmentMap->UnBind();
        //frameBuffer->UnBind();
        //frameBuffer->CleanUp();
    }

    void IBLProbeRenderer::RenderSpecularBRDFLUTMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
      
      //  cmdList->beginMarker("BRDFLUTPass");

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
        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::Back;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_SpecularBRDFLutShader->GetVertexShader();
        psoDesc.PS = m_SpecularBRDFLutShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
  
        if (!m_BrdfLUTPso) {
            m_BrdfLUTPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_BrdfLUTPso;
 
        graphicsPSO.inputLayout = psoDesc.inputLayout;
     
        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();


        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

    //    cmdList->endMarker();



    }

    void IBLProbeRenderer::ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        SCOPE_TIME_COUNTER(DrawProbes);

        
        for (const auto& it : scene->GetDiffuseLightProbes()) {

            Object* probe = it;
            _RenderProbe(cmdList, targetFramebuffer, scene, it);

        }

        for (const auto& it : scene->GetReflectLightProbes())
        {
            Object* probe = it;
            _RenderProbe(cmdList, targetFramebuffer, scene, it);

        }


       


    }

    void IBLProbeRenderer::RenderIndirectLight(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        if (scene->GetDiffuseLightProbes().size() > DEFERRED_MAX_LIGHT_PROBES) {
            GE_CORE_ERROR("light number %d exceed limit %d", scene->GetDiffuseLightProbes().size(), DEFERRED_MAX_LIGHT_PROBES);
            return;
        }



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
        psoDesc.rasterState.cullMode = RasterCullMode::Back;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_DeferredIBLShader->GetVertexShader();
        psoDesc.PS = m_DeferredIBLShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_DeferredShadingBindingLayout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_DeferredShadingIBLPso) {
            m_DeferredShadingIBLPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_DeferredShadingIBLPso;
        graphicsPSO.bindings.push_back(m_DeferredShadingBindingSet);
        graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;

        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();


        DeferredLightingConstants lightConstants{};
        FillProbesParameters(scene->GetDiffuseLightProbes(), lightConstants);

        cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);
    }

    void IBLProbeRenderer::FillProbesParameters(const std::vector<Object*>& probes, DeferredLightingConstants& output)
    {
        output.numLightProbes = probes.size();
        for (size_t i = 0; i < probes.size(); i++)
        {
            LightProbeConstants probeConst;
            probes[i]->GetComponent<LightProbe>()->FillLightProbeConstants(probes[i]->GetComponent<LightProbe>()->GetType(), Math::ToFloat3(probes[i]->GetComponent<Transform>()->GetPosition()), probeConst);
            output.lightProbes[i] = probeConst;
        }

    }


    void IBLProbeRenderer::RenderSHImage(Object* probe, TextureHandle environmentMap)
    {
        auto coeffs = SphericalHarmonics::UpdateCoeffs(environmentMap);
        probe->GetComponent<LightProbe>()->SetSHCoeffs(coeffs);
    }

    void IBLProbeRenderer::_RenderProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* probe)
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
        psoDesc.rasterState.cullMode = RasterCullMode::Back;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_ProbeDebugShader->GetVertexShader();
        psoDesc.PS = m_ProbeDebugShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_ProbeBindingLayout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_ProbePso) {
            m_ProbePso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_ProbePso;
        graphicsPSO.bindings.push_back(m_ProbeBindingSet);
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



        LightProbeConstants probeConst;

        probe->GetComponent<LightProbe>()->FillLightProbeConstants(probe->GetComponent<LightProbe>()->GetType(), Math::ToFloat3(probe->GetComponent<Transform>()->GetPosition()), probeConst);
        cmdList->writeBuffer(m_ProbeCB, &probeConst, sizeof(LightProbeConstants));
        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

    }

    void IBLProbeRenderer::_InitDeferredLighting()
    {

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
        m_DeferredIBLShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_ibl.glsl", &extends, &macros);;
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_Texture_SRV(0),
            RHIBindingLayoutItem::RT_Texture_SRV(1),
            RHIBindingLayoutItem::RT_Texture_SRV(2),
            RHIBindingLayoutItem::RT_Texture_SRV(3),
            RHIBindingLayoutItem::RT_Texture_SRV(4),
            RHIBindingLayoutItem::RT_Texture_SRV(5),
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8) //           DeferredLightingConstants

        };
        m_DeferredShadingBindingLayout = m_Device->createBindingLayout(layoutDesc);
        m_LightsCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(DeferredLightingConstants), "DeferredLightingConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(0, SystemTexture::Get().SceneColor, "SceneColor"),
            BindingSetItem::Texture_SRV(1, SystemTexture::Get().GBufferA, "GBufferA"),
            BindingSetItem::Texture_SRV(2, SystemTexture::Get().GBufferB, "GBufferB"),
            BindingSetItem::Texture_SRV(3, SystemTexture::Get().GBufferC, "GBufferC"),
            BindingSetItem::Texture_SRV(4, SystemTexture::Get().SceneDepth, "SceneDepth"),
            BindingSetItem::Texture_SRV(5, SystemTexture::Get().ShadowCubeMap, "ShadowCubeMap"),
            BindingSetItem::ConstantBuffer(8, m_LightsCB),
        };
        m_DeferredShadingBindingSet = m_Device->createBindingSet(bindingSetDesc, m_DeferredShadingBindingLayout);


    }

    void IBLProbeRenderer::_InitProbeDebug()
    {
        m_ProbeDebugShader = DBG_NEW MaterialShader("assets/shaders/glsl/lightProbes/lightProbe.glsl");

        //Debug Probe Material
        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8),
        };
        m_ProbeBindingLayout = m_Device->createBindingLayout(layoutDesc);
        m_ProbeCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(LightConstants), "LightConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::ConstantBuffer(8, m_ProbeCB),
        };
        m_ProbeBindingSet = m_Device->createBindingSet(bindingSetDesc, m_ProbeBindingLayout);
    }

    void IBLProbeRenderer::_InitProbeBake()
    {//assets/shaders/glsl/ibl/brdf.glsl");
        m_SpecularPrefilterShader = DBG_NEW MaterialShader("assets/shaders/glsl/ibl/prefilterMap.glsl");


        //多个probe共用一个camera
        m_ProbeCamera = g_objectManager->CreateCamera("ProbeCamera");
        m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetFov(90.0f);
        m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetWidth(Configuration::EnvironmantMapResolution);
        m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetHeight(Configuration::EnvironmantMapResolution);
        m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetZfar(13.0f);

        // m_BrdfLUTQuadObj = brdfLUTQuadObj;
        m_EnvironmentMapRenderer->Init();
        m_EnvironmentMapSkyboxRenderer->Init();

    }

    void IBLProbeRenderer::FillShaderParameters()
    {
    }

    void IBLProbeRenderer::UpdateProbeCamera(Object* probe)
    {
        glm::vec3 objPos = probe->GetComponent<Transform>()->GetPosition();
        glm::vec3 objRot = probe->GetComponent<Transform>()->GetRotation();
        m_ProbeCamera->SetPosition(objPos);
        m_ProbeCamera->SetRotation(objRot);
        m_ProbeCamera->GetObj()->GetComponent<PerspectiveCamera>()->SetZfar(probe->GetComponent<LightProbe>()->GetZfar());

    }

    //TODO:: 改用ddgi， cubemap 效率很低
    TextureHandle IBLProbeRenderer::RenderEnvironmerntCubeMaps(ICommandList* cmdList,  Scene* scene, Object* probe)
    {
        GE_ERROR_JUDGE();
        glm::vec3 center = probe->GetComponent<Transform>()->GetPosition();
        UpdateProbeCamera(probe);
        //probe->UpdateCamera();
        auto camera = m_ProbeCamera;// probe->GetCamera();
        auto cameraComponent = camera->GetObj()->GetComponent<PerspectiveCamera>();

        auto projection = cameraComponent->GetProjectionMatrix();
        //	cameraComponent->SetPosition(probe->GetPosition());

        std::vector<glm::mat4> ProbeView = {
           glm::lookAt(center, center - camera->Front(),-camera->Up()),
           glm::lookAt(center, center + camera->Front(),-camera->Up()),
           glm::lookAt(center, center + camera->Up(),-camera->Right()),//-camera->Front()
           glm::lookAt(center, center - camera->Up(),camera->Right()),//camera->Front()

           glm::lookAt(center, center - camera->Right(), -camera->Up()),
           glm::lookAt(center, center + camera->Right(), -camera->Up()),

        };
        std::vector<glm::mat4> ProbeProjectionViews = {
            projection * ProbeView[0],
            projection * ProbeView[1],
            projection * ProbeView[2],
            projection * ProbeView[3],
            projection * ProbeView[4],
            projection * ProbeView[5]
        };


        //std::shared_ptr<CubeMapTexture> environmentCubeMap = probe->GetHdrEnvironmentCubeMap();
        unsigned int environmentMapResolution = probe->GetComponent<LightProbe>()->GetEnvironmentCubeMapResolution();

        // 如果是 SH probe, 不需要每帧都创建cube map, 烘焙完后可统一release 掉 cubemap
      
        TextureHandle environmentCubeMap = probe->GetComponent<LightProbe>()->GetLdrEnvironmentCubeMap();
        TextureHandle depthBuffer = probe->GetComponent<LightProbe>()->GetDepthBuffer();
 

        glm::vec2 mipMapSize = { environmentCubeMap->getDesc().width, environmentCubeMap->getDesc().height };
       


       
        std::vector<int32_t> InArraySlice = { 0,1,2,3,4,5 };
        std::vector<uint8_t> InMipIndex(6, 0);

        FRHIRenderPassInfo RPInfo(
            6,
            environmentCubeMap.Get(),
            ERenderTargetActions::Clear_Store,
            InMipIndex.data(),
            InArraySlice.data(),
            depthBuffer,
            EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil
        );


       /* FRHIRenderPassInfo RPInfo(
            environmentCubeMap.Get(),
            ERenderTargetActions::Clear_Store,
            depthBuffer,
            EDepthStencilTargetActions::ClearDepthStencil_StoreDepthStencil
        );*/

        cmdList->beginRenderPass(RPInfo, "EnvironmerntCubeMaps_Pass_mip");
        //只画一层mip, diffuse map 再generate mipmap
        for (unsigned int mip = 0; mip < environmentCubeMap->getDesc().mipLevelsCnt; mip++)
        {
           
            std::string name = "mip" + std::to_string(mip);
            cmdList->beginMarker(name.c_str());

       

            for (unsigned int i = 0; i < 6; i++)
            {


                SceneData* view = DBG_NEW SceneData({ ProbeProjectionViews[i] ,ProbeView[i],projection,probe->GetComponent<Transform>()->GetPosition(),{},cameraComponent->Front(), *scene->GetLightSources()});
                math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();
                view->SetViewport(RHIViewport(mipMapSize.x, mipMapSize.y));

                view->bIsSubview = true;
                view->subViewTexTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
                view->subViewMip = mip;
                view->subViewId = i;
                view->subViewTextureId = static_cast<Texture*>(environmentCubeMap.Get())->GetRendererID();

                view->ViewFrustum = math::frustum(Math::ToFloat4x4(view->ViewMatrix * view->ProjectionMatrix), view->ReverseZ);
                if (cameraComponent->GetType() == Camera::CameraType::Perspective) {
                    view->zNear = cameraComponent->GetZnear();
                    view->zFar = cameraComponent->GetZfar();
                }
                m_EnvironmentMapRenderer->SetCustomView(view);
                m_EnvironmentMapRenderer->Render(cmdList, nullptr, scene);

                m_EnvironmentMapSkyboxRenderer->SetCustomView(view);
                m_EnvironmentMapSkyboxRenderer->Render(cmdList, nullptr, scene);

         
                delete view;
                view = nullptr;

            }

            cmdList->endMarker();

            mipMapSize.x /= 2.0f;
            mipMapSize.y /= 2.0f;

        }

        cmdList->endRenderPass();

   

        return environmentCubeMap;
    }
}