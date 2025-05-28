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
namespace BlackPearl{
    DeferredShadingRenderer::DeferredShadingRenderer(IDevice* device)
    : BasicRenderer(device){

    }

    void DeferredShadingRenderer::Init()
    {
        ShaderDesc desc = ShaderDesc(ShaderType::All);
        desc.debugName = "DeferredShadingShader";
        //TODO:: 不需要多个不同light的shader， 通过宏来决定用哪个函数
        m_DeferredPointLightShader = DBG_NEW MaterialShader("assets/shaders/glsl/deferred_shading/deferred_shading_bsdf_point_light.glsl");


        m_ShaderParameters[ShaderType::VertexShader].bindingLayouts.push_back(m_ViewBindinglayout);
        m_ShaderParameters[ShaderType::VertexShader].bindingSets.push_back(m_ViewBindingset);


       // m_AmbientGIPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferAmbientGIPass.glsl"));
       // m_PointLightPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferPontLightPass.glsl"));


       // m_LightPassFrameBuffer.reset(DBG_NEW FrameBuffer());
       // m_LightPassFrameBuffer->Bind();
       // m_LightPassFrameBuffer->AttachRenderBuffer(m_TextureWidth, m_TexxtureHeight);
       // m_LightPassFrameBuffer->AttachColorTexture(m_HDRPostProcessTexture, 0);
       //// m_LightPassFrameBuffer->AttachColorTexture(m_SSRTestTexture, 1);
       // //m_LightPassFrameBuffer->BindRenderBuffer();
       // m_LightPassFrameBuffer->UnBind();


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
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(8) //           ForwardShadingLightConstants

        };
        m_DeferredShadingBindingLayout = m_Device->createBindingLayout(layoutDesc);
        m_LightsCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(ForwardShadingLightConstants), "ForwardShadingLightConstants"));

        BindingSetDesc bindingSetDesc;
        bindingSetDesc.bindings = {
            BindingSetItem::Texture_SRV(0, SystemTexture::Get().SceneColor, "SceneColor"),
            BindingSetItem::Texture_SRV(1, SystemTexture::Get().GBufferA, "GBufferA"),
            BindingSetItem::Texture_SRV(2, SystemTexture::Get().GBufferB, "GBufferB"),
            BindingSetItem::Texture_SRV(3, SystemTexture::Get().GBufferC, "GBufferC"),
            BindingSetItem::Texture_SRV(4, SystemTexture::Get().GBufferD, "GBufferD"),
            BindingSetItem::Texture_SRV(5, SystemTexture::Get().ShadowCubeMap, "ShadowCubeMap"),
            BindingSetItem::ConstantBuffer(8, m_LightsCB),
        };
        m_DeferredShadingBindingSet = m_Device->createBindingSet(bindingSetDesc, m_DeferredShadingBindingLayout);



    }
    void DeferredShadingRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene)
    {

        ///* When we get here the depth buffer is already populated and the stencil pass
        // depends on it, but it does not write to it.
        // */

        ///* following use default frameBuffer! ������Ȼ���-->ʹ�� sphere ͶӰ����άƽ�棬ֻ��sphereͶӰ���fragment�Ż��� */
        //glDepthMask(GL_FALSE);
        ///* disable DEPTH_TEST �����е� glClear(GL_DEPTH_BUFFER_BIT) ʧЧ */
        //glDisable(GL_DEPTH_TEST);
        ///* blending multiple sphere fragment */
        //glEnable(GL_BLEND);
        //glBlendEquation(GL_FUNC_ADD);
        //glBlendFunc(GL_ONE, GL_ONE);
        //m_GBuffer->UnBind();

        //m_LightPassFrameBuffer->Bind();
        //m_LightPassFrameBuffer->BindRenderBuffer();
        //glViewport(0, 0, m_TextureWidth, m_TexxtureHeight);
        //glClear(GL_COLOR_BUFFER_BIT);

        ///* AmbientGI pass */
        //m_AmbientGIPassShader->Bind();

        //m_AmbientGIPassShader->SetUniform1i("gAmbientGI_AO", 2);
        //glActiveTexture(GL_TEXTURE2);
        //m_GBuffer->GetAmbientGIAOTexture()->Bind();

        //m_AmbientGIPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));
        //m_AmbientGIPassShader->SetUniform1f("u_Settings.GICoeffs", s_GICoeffs);

        //DrawObject(m_GIQuad, m_AmbientGIPassShader);

        cmdList->beginMarker("DeferredShading");
        LightSources* lightSources = scene->GetLightSources();
        /* PointLight pass */
        for (Object* pointLight : lightSources->GetPointLights())
        {
            /*      float radius = CalculateSphereRadius(pointLight);
                  m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
                  m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());

                  m_PointLightPassShader->Bind();

                  m_PointLightPassShader->Bind();
                  m_PointLightPassShader->SetUniform1i("gPosition", 0);
                  m_PointLightPassShader->SetUniform1i("gNormal", 1);
                  m_PointLightPassShader->SetUniform1i("gDiffuse_Roughness", 2);
                  m_PointLightPassShader->SetUniform1i("gSpecular_Mentallic", 3);
                  m_PointLightPassShader->SetUniform1i("gAmbientGI_AO", 4);
                  m_PointLightPassShader->SetUniform1i("gNormalMap", 5);

                  m_PointLightPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

                  glActiveTexture(GL_TEXTURE0);
                  m_GBuffer->GetPositionTexture()->Bind();
                  glActiveTexture(GL_TEXTURE1);
                  m_GBuffer->GetNormalTexture()->Bind();
                  glActiveTexture(GL_TEXTURE2);
                  m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
                  glActiveTexture(GL_TEXTURE3);
                  m_GBuffer->GetSpecularMentallicTexture()->Bind();
                  glActiveTexture(GL_TEXTURE4);
                  m_GBuffer->GetAmbientGIAOTexture()->Bind();
                  glActiveTexture(GL_TEXTURE5);
                  m_GBuffer->GetNormalMapTexture()->Bind();

                  m_PointLightPassShader->SetUniformVec3f("u_PointLight.ambient", pointLight->GetComponent<PointLight>()->GetLightProps().ambient);
                  m_PointLightPassShader->SetUniformVec3f("u_PointLight.diffuse", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
                  m_PointLightPassShader->SetUniformVec3f("u_PointLight.specular", pointLight->GetComponent<PointLight>()->GetLightProps().specular);
                  m_PointLightPassShader->SetUniformVec3f("u_PointLight.position", pointLight->GetComponent<Transform>()->GetPosition());

                  m_PointLightPassShader->SetUniform1f("u_PointLight.constant", pointLight->GetComponent<PointLight>()->GetAttenuation().constant);
                  m_PointLightPassShader->SetUniform1f("u_PointLight.linear", pointLight->GetComponent<PointLight>()->GetAttenuation().linear);
                  m_PointLightPassShader->SetUniform1f("u_PointLight.quadratic", pointLight->GetComponent<PointLight>()->GetAttenuation().quadratic);
                  m_PointLightPassShader->SetUniform1f("u_PointLight.intensity", pointLight->GetComponent<PointLight>()->GetLightProps().intensity);

                  m_PointLightPassShader->SetUniform1f("u_FarPlane", ShadowMapPointLightRenderer::s_FarPlane);
                  m_PointLightPassShader->SetUniform1i("u_ShadowMap", 6);
                  glActiveTexture(GL_TEXTURE6);
                  pointLight->GetComponent<PointLight>()->GetShadowMap()->Bind();

                  m_SurroundSphere->GetComponent<MeshRenderer>()->SetShaders(m_PointLightPassShader);
                  DrawObject(m_SurroundSphere, m_PointLightPassShader);

                  m_LightPassFrameBuffer->UnBind();
              }*/
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


          
            ForwardShadingLightConstants lightConstants{};
            FillShaderParameters(scene->GetLightSources(), lightConstants);

            cmdList->writeBuffer(m_LightsCB, &lightConstants, sizeof(ForwardShadingLightConstants));
            cmdList->setGraphicsState(graphicsPSO);

            Draw(cmdList, drawItem);

            //cmdList->endRenderPass();
            cmdList->endMarker();

        }
    }

    void DeferredShadingRenderer::FillShaderParameters(LightSources* lightSource, ForwardShadingLightConstants& output)
    {
        if (lightSource->GetLightsNum() > FORWARD_MAX_LIGHTS) {
            GE_CORE_ERROR("light number %d exceed limit %d", lightSource->GetLightsNum(), FORWARD_MAX_LIGHTS);
        }
        output.numLights = lightSource->GetLightsNum();
        for (size_t i = 0; i < math::min(FORWARD_MAX_LIGHTS, (int)lightSource->GetLightsNum()); i++)
        {
            LightConstants lightConst;
            Light* light = lightSource->GetLights()[i];
            light->FillLightConstants(lightConst);
            output.lights[i] = lightConst;
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