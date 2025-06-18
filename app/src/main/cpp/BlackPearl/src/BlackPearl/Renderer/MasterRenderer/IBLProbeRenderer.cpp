//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/IBLProbeRenderer.h"
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
namespace BlackPearl
{

    extern ObjectManager* g_objectManager;
    float IBLProbeRenderer::s_GICoeffs = 0.2f;
    IBLProbeRenderer::IBLProbeRenderer(IDevice* device) :
        BasicRenderer(device) {
        m_EnvironmentMapRenderer = DBG_NEW ForwardShadingRenderer(device);
        m_EnvironmentMapSkyboxRenderer = DBG_NEW SkyboxRenderer(device);
    }

   
    void IBLProbeRenderer::Init()
    {

        //m_LightProbeShader = DBG_NEW MaterialShader("assets/shaders/glsl/lightProbes/lightProbe.glsl");
        //m_IBLShader = DBG_NEW MaterialShader("assets/shaders/glsl/lightProbes/iblSHTexture.glsl");
        //m_IrradianceShader = DBG_NEW MaterialShader("assets/shaders/glsl/ibl/irradianceConvolution.glsl");
        m_SpecularBRDFLutShader = DBG_NEW MaterialShader("assets/shaders/glsl/ibl/brdf.glsl");
       // m_PbrShader = DBG_NEW MaterialShader("assets/shaders/glsl/pbr/PbrTexture.glsl");
       // m_NonPbrShader = DBG_NEW MaterialShader("assets/shaders/glsl/IronMan.glsl");
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
                psoDesc.rasterState.cullMode = RasterCullMode::None;
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
        psoDesc.rasterState.cullMode = RasterCullMode::None;
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

    void IBLProbeRenderer::RenderSHImage(Object* probe, TextureHandle environmentMap)
    {
        auto coeffs = SphericalHarmonics::UpdateCoeffs(environmentMap);
        probe->GetComponent<LightProbe>()->SetSHCoeffs(coeffs);
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