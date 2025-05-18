//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/SSRRenderer.h"
#include "RHI/RHIDevice.h"

namespace BlackPearl{
    float SSRRenderer::s_SSRGICoeffs = 0.2f;
    extern DeviceManager* g_deviceManager;

    SSRRenderer::SSRRenderer(IDevice* device)
            :BasicRenderer(device)
    {

    }

    void SSRRenderer::Init(){
        m_SSRPassShader = DBG_NEW MaterialShader("assets/shaders/glsl/gBufferProbe/gBufferSSRGIPass.glsl");
    }

    void SSRRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
       /* if (enableSSR) {
            m_SSRPassShader->Bind();
            m_SSRPassShader->SetUniform1i("gPosition", 0);
            m_SSRPassShader->SetUniform1i("gNormal", 1);
            m_SSRPassShader->SetUniform1i("gDiffuse_Roughness", 2);
            m_SSRPassShader->SetUniform1i("gSpecular_Mentallic", 3);
            m_SSRPassShader->SetUniform1i("gAmbientGI_AO", 4);
            m_SSRPassShader->SetUniform1i("gNormalMap", 5);
            m_SSRPassShader->SetUniform1i("depthTexture",6);


            m_SSRPassShader->SetUniformVec2f("gScreenSize", glm::vec2(m_TextureWidth, m_TexxtureHeight));
            m_SSRPassShader->SetUniform1f("u_Settings.SSRGICoeffs", s_SSRGICoeffs);

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
            glActiveTexture(GL_TEXTURE6);
            depthTexture->Bind();

            DrawObject(m_GIQuad, m_SSRPassShader);*/


        //}


    }


}
