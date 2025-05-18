//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/GrabPassRenderer.h"
namespace BlackPearl{
    bool GrabPassRenderer::s_HDR = true;

    GrabPassRenderer::GrabPassRenderer(IDevice* device):
    BasicRenderer(device)
    {

    }
    void GrabPassRenderer::Init(){

       // m_FinalScreenShader.reset(DBG_NEW Shader("assets/shaders/glsl/gBufferProbe/FinalScreenQuad.glsl"));


    }
    void GrabPassRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene){

        //glViewport(0, 0, m_TextureWidth, m_TexxtureHeight);

        //glClear(GL_COLOR_BUFFER_BIT);
        //m_FinalScreenShader->Bind();
        //m_FinalScreenShader->SetUniform1i("u_FinalScreenTexture", 0);
        //m_FinalScreenShader->SetUniform1f("u_Settings.hdr", s_HDR);
        //glActiveTexture(GL_TEXTURE0);
        //m_HDRPostProcessTexture->Bind();  // shading pass 生成的texture
        //DrawObject(m_FinalScreenQuad, m_FinalScreenShader);

    }


     void GrabPassRenderer::FillShaderParameters(){}
}