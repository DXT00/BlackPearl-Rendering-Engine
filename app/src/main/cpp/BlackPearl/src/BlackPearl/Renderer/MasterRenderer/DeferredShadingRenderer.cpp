//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/DeferredShadingRenderer.h"

namespace BlackPearl{
    DeferredShadingRenderer::DeferredShadingRenderer(IDevice* device)
    : BasicRenderer(device){

    }

    void DeferredShadingRenderer::Init()
    {

       // m_AmbientGIPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferAmbientGIPass.glsl"));
       // m_PointLightPassShader.reset(DBG_NEW Shader("assets/shaders/gBufferProbe/gBufferPontLightPass.glsl"));


       // m_LightPassFrameBuffer.reset(DBG_NEW FrameBuffer());
       // m_LightPassFrameBuffer->Bind();
       // m_LightPassFrameBuffer->AttachRenderBuffer(m_TextureWidth, m_TexxtureHeight);
       // m_LightPassFrameBuffer->AttachColorTexture(m_HDRPostProcessTexture, 0);
       //// m_LightPassFrameBuffer->AttachColorTexture(m_SSRTestTexture, 1);
       // //m_LightPassFrameBuffer->BindRenderBuffer();
       // m_LightPassFrameBuffer->UnBind();

    }
    void DeferredShadingRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
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



        ///* PointLight pass */
        //for (Object* pointLight : lightSources->GetPointLights())
        //{
        //    float radius = CalculateSphereRadius(pointLight);
        //    m_SurroundSphere->GetComponent<Transform>()->SetScale({ radius,radius,radius });
        //    m_SurroundSphere->GetComponent<Transform>()->SetPosition(pointLight->GetComponent<Transform>()->GetPosition());

        //    m_PointLightPassShader->Bind();

        //    m_PointLightPassShader->Bind();
        //    m_PointLightPassShader->SetUniform1i("gPosition", 0);
        //    m_PointLightPassShader->SetUniform1i("gNormal", 1);
        //    m_PointLightPassShader->SetUniform1i("gDiffuse_Roughness", 2);
        //    m_PointLightPassShader->SetUniform1i("gSpecular_Mentallic", 3);
        //    m_PointLightPassShader->SetUniform1i("gAmbientGI_AO", 4);
        //    m_PointLightPassShader->SetUniform1i("gNormalMap", 5);

        //    m_PointLightPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));

        //    glActiveTexture(GL_TEXTURE0);
        //    m_GBuffer->GetPositionTexture()->Bind();
        //    glActiveTexture(GL_TEXTURE1);
        //    m_GBuffer->GetNormalTexture()->Bind();
        //    glActiveTexture(GL_TEXTURE2);
        //    m_GBuffer->GetDiffuseRoughnessTexture()->Bind();
        //    glActiveTexture(GL_TEXTURE3);
        //    m_GBuffer->GetSpecularMentallicTexture()->Bind();
        //    glActiveTexture(GL_TEXTURE4);
        //    m_GBuffer->GetAmbientGIAOTexture()->Bind();
        //    glActiveTexture(GL_TEXTURE5);
        //    m_GBuffer->GetNormalMapTexture()->Bind();

        //    m_PointLightPassShader->SetUniformVec3f("u_PointLight.ambient", pointLight->GetComponent<PointLight>()->GetLightProps().ambient);
        //    m_PointLightPassShader->SetUniformVec3f("u_PointLight.diffuse", pointLight->GetComponent<PointLight>()->GetLightProps().diffuse);
        //    m_PointLightPassShader->SetUniformVec3f("u_PointLight.specular", pointLight->GetComponent<PointLight>()->GetLightProps().specular);
        //    m_PointLightPassShader->SetUniformVec3f("u_PointLight.position", pointLight->GetComponent<Transform>()->GetPosition());

        //    m_PointLightPassShader->SetUniform1f("u_PointLight.constant", pointLight->GetComponent<PointLight>()->GetAttenuation().constant);
        //    m_PointLightPassShader->SetUniform1f("u_PointLight.linear", pointLight->GetComponent<PointLight>()->GetAttenuation().linear);
        //    m_PointLightPassShader->SetUniform1f("u_PointLight.quadratic", pointLight->GetComponent<PointLight>()->GetAttenuation().quadratic);
        //    m_PointLightPassShader->SetUniform1f("u_PointLight.intensity", pointLight->GetComponent<PointLight>()->GetLightProps().intensity);

        //    m_PointLightPassShader->SetUniform1f("u_FarPlane", ShadowMapPointLightRenderer::s_FarPlane);
        //    m_PointLightPassShader->SetUniform1i("u_ShadowMap", 6);
        //    glActiveTexture(GL_TEXTURE6);
        //    pointLight->GetComponent<PointLight>()->GetShadowMap()->Bind();

        //    m_SurroundSphere->GetComponent<MeshRenderer>()->SetShaders(m_PointLightPassShader);
        //    DrawObject(m_SurroundSphere, m_PointLightPassShader);

        //    m_LightPassFrameBuffer->UnBind();
        //}



    }

    void DeferredShadingRenderer::FillShaderParameters()
    {
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