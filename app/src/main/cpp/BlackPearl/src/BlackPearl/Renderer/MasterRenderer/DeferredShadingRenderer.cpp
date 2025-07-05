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

namespace BlackPearl{
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
        m_DeferredPointLightShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_point_light.glsl", &extends, &macros);;
        m_DeferredIBLShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_ibl.glsl", &extends, &macros);;

        


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
    void DeferredShadingRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
        if (Configuration::bUseDirectLight)
            RenderDirectionLights(cmdList, targetFramebuffer, scene);
        if(Configuration::bUseIndirectLight)
            RenderIBLProbes(cmdList, targetFramebuffer, scene);



        
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

            psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
            psoDesc.depthStencilState.enableDepthTest();
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


        for (size_t i = 0; i < lightSources->GetPointLightNum(); i++)
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

            psoDesc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
            psoDesc.depthStencilState.enableDepthTest();
            psoDesc.depthStencilState.disableDepthWrite();
            psoDesc.depthStencilState.disableStencil();

            psoDesc.blendState.alphaToCoverageEnable = false;
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
            FillLightsParameters(lightSources->GetParallelLights()[i]->GetComponent<PointLight>(), lightConstants);

            cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(DeferredLightingConstants));
            cmdList->setGraphicsState(graphicsPSO);

            Draw(cmdList, drawItem);

        }
    }

    void DeferredShadingRenderer::RenderIBLProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
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
            psoDesc.rasterState.cullMode = RasterCullMode::None;
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

    void DeferredShadingRenderer::FillLightsParameters(Light* light, DeferredLightingConstants& output)
    {
            output.numLightProbes = 0;
           
            LightConstants lightConst;
         
            light->FillLightConstants(lightConst);
            output.light = lightConst;
       
    }

    void DeferredShadingRenderer::FillProbesParameters(const std::vector<Object*>& probes, DeferredLightingConstants& output)
    {
        output.numLightProbes = probes.size();
        for (size_t i = 0; i < probes.size(); i++)
        {
            LightProbeConstants probeConst;
            probes[i]->GetComponent<LightProbe>()->FillLightProbeConstants(probes[i]->GetComponent<LightProbe>()->GetType(), Math::ToFloat3(probes[i]->GetComponent<Transform>()->GetPosition()), probeConst);
            output.lightProbes[i] = probeConst;
        }

    }

    float DeferredShadingRenderer::CalculateSphereRadius(Object* pointLight)
    {
       /* auto lightProps = pointLight->GetComponent<PointLight>()->GetLightProps();
        auto attenuation = pointLight->GetComponent<PointLight>()->GetAttenuation();
        math::float3 lightDiffuse = lightProps.diffuse;
        float constant = attenuation.constant;
        float linear = attenuation.linear;
        float quadratic = attenuation.quadratic;



        float maxChannel = std::max(std::max(lightDiffuse.x, lightDiffuse.y), lightDiffuse.z);

        float distance = (-linear + sqrtf(linear * linear - 4 * quadratic * (constant - 256.0f / s_AttenuationItensity * maxChannel))) / (2 * quadratic);

        return distance;*/
        return 0;
    }

}