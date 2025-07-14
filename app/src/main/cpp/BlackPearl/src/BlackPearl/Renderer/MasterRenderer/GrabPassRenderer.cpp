//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"
#include "Core.h"
#ifdef GE_API_OPENGL
#include "RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
#include "Renderer/MasterRenderer/GrabPassRenderer.h"
#include "RHI/RHIGlobals.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Timestep/TimeCounter.h"
namespace BlackPearl{
    bool GrabPassRenderer::s_HDR = true;

    GrabPassRenderer::GrabPassRenderer(IDevice* device):
    BasicRenderer(device)
    {

    }
    void GrabPassRenderer::Init(TextureHandle grabTexture, bool plsCopy){

        m_bPlsCopy = plsCopy;
        std::vector<std::string> extends;
        std::vector<std::string> macros;
#ifdef GE_PLATFORM_ANDROID
        if  (m_bPlsCopy && RenderGraph::SupportPLS())
        {
            extends.push_back("#extension GL_EXT_shader_pixel_local_storage : require");
            extends.push_back("#extension GL_ARM_shader_framebuffer_fetch_depth_stencil : require");
            macros.push_back("#define USE_GLES_PLS 1");
        }
#endif

        m_FinalScreenShader = DBG_NEW MaterialShader("assets/shaders/glsl/grab_pass/FinalScreenQuad.glsl",&extends, &macros);


        if(!m_bPlsCopy && grabTexture){

            RHIBindingLayoutDesc layoutDesc;
            layoutDesc.visibility = ShaderType::Pixel;
            layoutDesc.bindings = {
                    RHIBindingLayoutItem::RT_Texture_SRV(0),
            };
            m_GrabPassBindingLayout = m_Device->createBindingLayout(layoutDesc);

            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                    BindingSetItem::Texture_SRV(0, grabTexture.Get(), "GrabTexture"),
            };


            m_GrabPassBindingSet = m_Device->createBindingSet(bindingSetDesc, m_GrabPassBindingLayout);


        }




    }
    void GrabPassRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene){
        SCOPE_TIME_COUNTER(GrabPass);






        //Deferred Shading Material
        //cmdList->beginMarker("GrabPass");

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

        psoDesc.VS = m_FinalScreenShader->GetVertexShader();
        psoDesc.PS = m_FinalScreenShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        if(!m_bPlsCopy){
            psoDesc.bindingLayouts.push_back(m_GrabPassBindingLayout);
            psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);
        }



        if (!m_GrabPassPso) {
            m_GrabPassPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_GrabPassPso;
        if(!m_bPlsCopy) {
            graphicsPSO.bindings.push_back(m_GrabPassBindingSet);
            graphicsPSO.bindings.push_back(m_ViewBindingset);
        }
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


        cmdList->setGraphicsState(graphicsPSO);

        Draw(cmdList, drawItem);

        //cmdList->endRenderPass();
      //  cmdList->endMarker();


    }


     void GrabPassRenderer::FillShaderParameters(){}
}