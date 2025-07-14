//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/DeferredShadingRenderer.h"
#include "RHI/Common/RHIUtils.h"
#include "Renderer/SystemTextures.h"
#include "Component/LightComponent/DirectionLight.h"
#include "Component/LightComponent/PointLight.h"
#include "Component/LightComponent/SpotLight.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
#include "RHI/RHIGlobals.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Timestep/TimeCounter.h"
#include "Renderer/GIManager.h"
namespace BlackPearl{
    extern GIManager* g_GIManager;
    DeferredShadingRenderer::DeferredShadingRenderer(IDevice* device)
    : BasicRenderer(device){

    }

    void DeferredShadingRenderer::Init()
    {
        std::vector<std::string> extends;
        std::vector<std::string> macros;
#ifdef GE_PLATFORM_ANDROID
        if  (RenderGraph::SupportPLS())
        {
            extends.push_back("#extension GL_EXT_shader_pixel_local_storage : require");
            extends.push_back("#extension GL_ARM_shader_framebuffer_fetch_depth_stencil : require");
            macros.push_back("#define USE_GLES_PLS 1");

        }
#endif
        macros.push_back("#define DEFERRED_SHADING_PASS 1");

        //TODO:: 不需要多个不同light的shader， 通过宏来决定用哪个函数
        m_DeferredDirectionLightShader  = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_direction_light.glsl", &extends, &macros);
        m_DeferredPointLightShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_point_light.glsl", &extends, &macros);
        m_DeferredPointLightStencilShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_point_light_sphere.glsl", &extends, &macros);
        m_DeferredPointLightDebugShader = DBG_NEW MaterialShader("assets/shaders/glsl/light/pointlight_Debug.glsl", &extends, &macros);


        //Deferred Shading Material
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


        m_ShaderParameters[ShaderType::VertexShader].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::VertexShader].bindingSets.push_back(m_ViewBindingset);
        m_ShaderParameters[ShaderType::Pixel].bindingLayouts.push_back(m_DeferredShadingBindingLayout);
        m_ShaderParameters[ShaderType::Pixel].bindingSets.push_back(m_DeferredShadingBindingSet);

        m_ShaderParameters->PixelShader = m_DeferredDirectionLightShader->GetPixelShader();
        m_ShaderParameters->VertexShader = m_DeferredDirectionLightShader->GetVertexShader();


    }
    void DeferredShadingRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, bool renderGI)
    {
        if (Configuration::bUseDirectLight) {
            cmdList->beginMarker("RenderDirectionLight");
            RenderDirectionLights(cmdList, targetFramebuffer, scene);
            cmdList->endMarker();

            if (Configuration::bShowPointLight) {
                cmdList->beginMarker("RenderPointLight");
                RenderPointLights(cmdList, targetFramebuffer, scene);
                cmdList->endMarker();
            }


        }


        if (Configuration::bUseIndirectLight && g_GIManager->GetGIRenderer() && renderGI) {
            cmdList->beginMarker("RenderIndirectLight");

            RenderIndirectLight(cmdList, targetFramebuffer, scene);
            cmdList->endMarker();

        }
           // RenderIBLProbes(cmdList, targetFramebuffer, scene);



        
    }

    void DeferredShadingRenderer::RenderDirectionLights(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        SCOPE_TIME_COUNTER(DeferredShading);


        LightSources* lightSources = scene->GetLightSources();
        if (lightSources->GetParallelLightNum() > DEFERRED_MAX_DIRECTION_LIGHTS) {
            GE_CORE_ERROR("light number %d exceed limit %d", lightSources->GetParallelLightNum(), DEFERRED_MAX_DIRECTION_LIGHTS);
            return;
        }


        for (size_t i = 0; i < lightSources->GetParallelLightNum(); i++)
        {
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

           // psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
            psoDesc.depthStencilState.disableDepthTest();
            psoDesc.depthStencilState.disableDepthWrite();
            psoDesc.depthStencilState.disableStencil();

            psoDesc.blendState.alphaToCoverageEnable = false;

            for (auto& target: psoDesc.blendState.targets)
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

            psoDesc.VS = m_DeferredDirectionLightShader->GetVertexShader();
            psoDesc.PS = m_DeferredDirectionLightShader->GetPixelShader();
            psoDesc.bFromPSOFileCache = false;
            psoDesc.bindingLayouts.push_back(m_DeferredShadingBindingLayout);
            psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


            if (!m_DeferredShadingDirectionLightPso) {
                m_DeferredShadingDirectionLightPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
            }
            graphicsPSO.pipeline = m_DeferredShadingDirectionLightPso;
            graphicsPSO.bindings.push_back(m_DeferredShadingBindingSet);
            graphicsPSO.bindings.push_back(m_ViewBindingset);
            graphicsPSO.inputLayout = psoDesc.inputLayout;
           
            SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
            GE_ERROR_JUDGE();


            DeferredLightingConstants lightConstants{};
            FillLightsParameters(lightSources->GetParallelLights()[i]->GetComponent<DirectionLight>(), lightConstants);

            cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
            cmdList->setGraphicsState(graphicsPSO);

            Draw(cmdList, drawItem);

        }

       

    }

    void DeferredShadingRenderer::RenderPointLights(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {


        LightSources* lightSources = scene->GetLightSources();
        if (lightSources->GetPointLightNum() > DEFERRED_MAX_POINT_LIGHTS) {
            GE_CORE_ERROR("light number %d exceed limit %d", lightSources->GetPointLightNum(), DEFERRED_MAX_POINT_LIGHTS);
            return;
        }

        //方法2： 只绘制sphere， 需要双面渲染，有overdraw（单面渲染相机到sphere内部，无法绘制）

        /*
            方法1：绘制sphere mask,再绘制sphere， 相机到sphere内部时，depth test fail的情况下依旧渲染！

            使用：glStencilOp(GL_KEEP, GL_INCR, GL_KEEP); depth 失败时， stencil值原来为0，现在加1， 还是写入！
            关键函数：glStencilOp

                void glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);
                参数含义：

                sfail：Stencil test 失败时的操作；

                dpfail：Stencil test 通过但 Depth Test 失败 时的操作；

                dppass：Stencil test 和 Depth Test 都通过时的操作。
         */
        for (size_t i = 0; i < lightSources->GetPointLightNum(); i++)
        {
            PointLight* pointLight = lightSources->GetPointLights()[i]->GetComponent<PointLight>();

//glm::vec3(pointLight->GetAttenuation().maxDistance)
            pointLight->SetPosition(lightSources->GetPointLights()[i]->GetComponent<Transform>()->GetPosition());
            scene->GetPointLightSphere()->GetComponent<Transform>()->SetPosition(lightSources->GetPointLights()[i]->GetComponent<Transform>()->GetPosition());
            scene->GetPointLightSphere()->GetComponent<Transform>()->SetScale(glm::vec3(pointLight->GetRadius()));
            
            
          //  _RenderPointLightMask(cmdList, targetFramebuffer, scene, pointLight);


            _RenderPointLight(cmdList, targetFramebuffer, scene, pointLight);
      

        }
    }
    void DeferredShadingRenderer::_RenderPointLightMask(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, PointLight* pointLight)
    {
        LightSources* lightSources = scene->GetLightSources();
        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);


        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetPointLightSphere())[0];

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = targetFramebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        GraphicsPipelineDesc psoDesc;

        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        /*
        
        
// === 判断摄像机是否在光体积内 ===
bool cameraInside = distance(cameraPos, lightPos) < lightRadius;

if (cameraInside)
{
    // 摄像机在光体积内 → 使用 Z-fail 策略
    glCullFace(GL_BACK); // 剔除背面（只绘制正面）
    glStencilOp(GL_KEEP, GL_INCR, GL_KEEP); // Depth fail 时 +1
}
else
{
    // 摄像机在光体积外 → 使用 Z-pass 策略
    glCullFace(GL_FRONT); // 剔除正面（只绘制背面）
    glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); // Depth pass 时 +1
}

        */


        psoDesc.depthStencilState.enableStencil();
        psoDesc.depthStencilState.setStencilWriteMask(0xff);
        psoDesc.depthStencilState.setStencilReadMask(0xff); 
        psoDesc.depthStencilState.stencilRefValue = 0;
        psoDesc.depthStencilState.frontFaceStencil.setStencilFunc(ComparisonFunc::Always);
        psoDesc.depthStencilState.frontFaceStencil.setPassOp(StencilOp::Keep);
        psoDesc.depthStencilState.frontFaceStencil.setFailOp(StencilOp::Keep);
        psoDesc.depthStencilState.frontFaceStencil.setDepthFailOp(StencilOp::IncrementAndClamp);

        psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        psoDesc.depthStencilState.enableDepthTest();
        psoDesc.depthStencilState.disableDepthWrite();

        psoDesc.blendState.alphaToCoverageEnable = false;
        psoDesc.rasterState.frontCounterClockwise = true;
        psoDesc.rasterState.cullMode = RasterCullMode::Back;
        psoDesc.primType = PrimitiveType::TriangleList;
        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

        psoDesc.VS = m_DeferredPointLightStencilShader->GetVertexShader();
        psoDesc.PS = m_DeferredPointLightStencilShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_DeferredShadingPointLightMaskPso) {
            m_DeferredShadingPointLightMaskPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_DeferredShadingPointLightMaskPso;
        graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;

        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();


        //DeferredLightingConstants lightConstants{};
        //FillLightsParameters(pointLight, lightConstants);

        //cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);
    }

    void DeferredShadingRenderer::_RenderPointLight(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, PointLight* pointLight)
    {
        LightSources* lightSources = scene->GetLightSources();
        SceneData* view = Renderer::GetSceneData();
        GE_ERROR_JUDGE();

        SceneData* preView = Renderer::GetPreSceneData();
        GE_ERROR_JUDGE();

        SetupView(cmdList, view, preView);

      //  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

       // DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetPointLightSphere())[0];
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
        //psoDesc.depthStencilState.enableStencil();
        //psoDesc.depthStencilState.stencilRefValue = 1; //stencil == 1 就写入
        //psoDesc.depthStencilState.frontFaceStencil.setStencilFunc(ComparisonFunc::Equal);
        //psoDesc.depthStencilState.setStencilWriteMask(0x00);  // 不再修改 stencil
        //psoDesc.depthStencilState.setStencilReadMask(0xff);  //


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

        psoDesc.VS = m_DeferredPointLightShader->GetVertexShader();
        psoDesc.PS = m_DeferredPointLightShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        psoDesc.bindingLayouts.push_back(m_DeferredShadingBindingLayout);
        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


        if (!m_DeferredShadingPointLightPso) {
            m_DeferredShadingPointLightPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_DeferredShadingPointLightPso;
        graphicsPSO.bindings.push_back(m_DeferredShadingBindingSet);
        graphicsPSO.bindings.push_back(m_ViewBindingset);
        graphicsPSO.inputLayout = psoDesc.inputLayout;

        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
        GE_ERROR_JUDGE();


        DeferredLightingConstants lightConstants{};
        FillLightsParameters(pointLight, lightConstants);

        cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);
    }

    void DeferredShadingRenderer::RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        g_GIManager->GetGIRenderer()->RenderIndirectLight(commandList, targetFramebuffer, scene);
    }

    void DeferredShadingRenderer::ShowPointLight(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        LightSources* lightSources = scene->GetLightSources();
        if (lightSources->GetPointLightNum() > DEFERRED_MAX_POINT_LIGHTS) {
            GE_CORE_ERROR("light number %d exceed limit %d", lightSources->GetPointLightNum(), DEFERRED_MAX_POINT_LIGHTS);
            return;
        }

        for (size_t i = 0; i < lightSources->GetPointLightNum(); i++) {

            PointLight* pointLight = lightSources->GetPointLights()[i]->GetComponent<PointLight>();

            Object* obj = lightSources->GetPointLights()[i];

            SceneData* view = Renderer::GetSceneData();
            GE_ERROR_JUDGE();

            SceneData* preView = Renderer::GetPreSceneData();
            GE_ERROR_JUDGE();

            SetupView(cmdList, view, preView);


            DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(obj)[0];

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

            psoDesc.VS = m_DeferredPointLightDebugShader->GetVertexShader();
            psoDesc.PS = m_DeferredPointLightDebugShader->GetPixelShader();
            psoDesc.bFromPSOFileCache = false;
            psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


            if (!m_DeferredShadingPointLightPso) {
                m_DeferredShadingPointLightPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
            }
            graphicsPSO.pipeline = m_DeferredShadingPointLightPso;
            graphicsPSO.bindings.push_back(m_ViewBindingset);
            graphicsPSO.inputLayout = psoDesc.inputLayout;

            SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
            GE_ERROR_JUDGE();

            cmdList->setGraphicsState(graphicsPSO);

            Draw(cmdList, drawItem);
        }

    }

    //void DeferredShadingRenderer::RenderIBLProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    //{
    //   
    //    if (scene->GetDiffuseLightProbes().size() > DEFERRED_MAX_LIGHT_PROBES) {
    //        GE_CORE_ERROR("light number %d exceed limit %d", scene->GetDiffuseLightProbes().size(), DEFERRED_MAX_LIGHT_PROBES);
    //        return;
    //    }


    // 
    //        SceneData* view = Renderer::GetSceneData();
    //        GE_ERROR_JUDGE();

    //        SceneData* preView = Renderer::GetPreSceneData();
    //        GE_ERROR_JUDGE();

    //        SetupView(cmdList, view, preView);


    //        DrawItem drawItem = IDrawStrategy::ObjectToDrawItem(scene->GetFullScreenObj())[0];

    //        GraphicsState graphicsPSO;
    //        graphicsPSO.framebuffer = targetFramebuffer;
    //        graphicsPSO.viewport = view->GetViewportState();
    //        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

    //        GraphicsPipelineDesc psoDesc;

    //        psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
    //        psoDesc.depthStencilState.enableDepthTest();
    //        psoDesc.depthStencilState.disableDepthWrite();
    //        psoDesc.depthStencilState.disableStencil();

    //        psoDesc.blendState.alphaToCoverageEnable = false;

    //        for (auto& target : psoDesc.blendState.targets)
    //        {
    //            target.blendEnable = true;
    //            target.blendOp = BlendOp::Add;
    //            target.srcBlend = BlendFactor::One;
    //            target.destBlend = BlendFactor::One;
    //            target.srcBlendAlpha = BlendFactor::One;
    //            target.destBlendAlpha = BlendFactor::One;
    //            target.blendOpAlpha = BlendOp::Add;
    //        }


    //        psoDesc.rasterState.frontCounterClockwise = true;
    //        psoDesc.rasterState.cullMode = RasterCullMode::None;
    //        psoDesc.primType = PrimitiveType::TriangleList;
    //        psoDesc.inputLayout = m_Device->createInputLayout(drawItem.mesh->GetVertexBufferLayout());

    //        psoDesc.VS = m_DeferredIBLShader->GetVertexShader();
    //        psoDesc.PS = m_DeferredIBLShader->GetPixelShader();
    //        psoDesc.bFromPSOFileCache = false;
    //        psoDesc.bindingLayouts.push_back(m_DeferredShadingBindingLayout);
    //        psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);


    //        if (!m_DeferredShadingIBLPso) {
    //            m_DeferredShadingIBLPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
    //        }
    //        graphicsPSO.pipeline = m_DeferredShadingIBLPso;
    //        graphicsPSO.bindings.push_back(m_DeferredShadingBindingSet);
    //        graphicsPSO.bindings.push_back(m_ViewBindingset);
    //        graphicsPSO.inputLayout = psoDesc.inputLayout;
    //      
    //        SetupInputBuffers(cmdList, const_cast<BufferGroup*>(drawItem.buffers), drawItem.transform, graphicsPSO);
    //        GE_ERROR_JUDGE();


    //        DeferredLightingConstants lightConstants{};
    //        FillProbesParameters(scene->GetDiffuseLightProbes(), lightConstants);

    //        cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
    //        cmdList->setGraphicsState(graphicsPSO);

    //        Draw(cmdList, drawItem);


    //}

    void DeferredShadingRenderer::FillLightsParameters(Light* light, DeferredLightingConstants& output)
    {
            output.numLightProbes = 0;
           
            LightConstants lightConst;
         
            light->FillLightConstants(lightConst);
            output.light = lightConst;
       
    }

    //void DeferredShadingRenderer::FillProbesParameters(const std::vector<Object*>& probes, DeferredLightingConstants& output)
    //{
    //    output.numLightProbes = probes.size();
    //    for (size_t i = 0; i < probes.size(); i++)
    //    {
    //        LightProbeConstants probeConst;
    //        probes[i]->GetComponent<LightProbe>()->FillLightProbeConstants(probes[i]->GetComponent<LightProbe>()->GetType(), Math::ToFloat3(probes[i]->GetComponent<Transform>()->GetPosition()), probeConst);
    //        output.lightProbes[i] = probeConst;
    //    }

    //}

    float DeferredShadingRenderer::CalculateSphereRadius(Object* pointLight)
    {
        float s_AttenuationItensity = 1.0;
      auto lightProps = pointLight->GetComponent<PointLight>()->GetLightProps();
        auto attenuation = pointLight->GetComponent<PointLight>()->GetAttenuation();
        math::float3 lightDiffuse = lightProps.diffuse;
        float constant = attenuation.constant;
        float linear = attenuation.linear;
        float quadratic = attenuation.quadratic;



        float maxChannel = std::max(std::max(lightDiffuse.x, lightDiffuse.y), lightDiffuse.z);

        float distance = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - 256.0f / s_AttenuationItensity * maxChannel))) / (2 * quadratic);

        return distance;
    }


}