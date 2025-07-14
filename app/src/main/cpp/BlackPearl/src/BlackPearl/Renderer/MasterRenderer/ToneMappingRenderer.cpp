#include "pch.h"
#include "Renderer/MasterRenderer/ToneMappingRenderer.h"
#ifdef GE_API_OPENGL
#include "BlackPearl/RHI/OpenGLRHI/OpenGLDriver/OpenGLFunctions.h"
#endif
#include "Timestep/TimeCounter.h"
#include "BlackPearl/RHI/Common/RHIUtils.h"

namespace BlackPearl {
    ToneMappingRenderer::ToneMappingRenderer(IDevice* device) :
        BasicRenderer(device)
    {

    }
    void ToneMappingRenderer::Init(TextureHandle srcTexture, bool plsCopy) {

        std::vector<std::string> extends;
        std::vector<std::string> macros;


        m_ToneMappingShader = DBG_NEW MaterialShader("assets/shaders/glsl/tonemapping/tonemapping.glsl", &extends, &macros);
        m_ToneMappingCB = m_Device->createBuffer(RHIUtils::CreateStaticConstantBufferDesc(sizeof(ToneMappingConstants), "ToneMappingConstants"));


        if (srcTexture) {

            RHIBindingLayoutDesc layoutDesc;
            layoutDesc.visibility = ShaderType::Pixel;
            layoutDesc.bindings = {
                    RHIBindingLayoutItem::RT_Texture_SRV(0),
                    RHIBindingLayoutItem::RT_VolatileConstantBuffer(8) 
            };
            m_ToneMappingBindingLayout = m_Device->createBindingLayout(layoutDesc);

            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                    BindingSetItem::Texture_SRV(0, srcTexture.Get(), "SrcTexture"),
                    BindingSetItem::ConstantBuffer(8, m_ToneMappingCB),
            };


            m_ToneMappingBindingSet = m_Device->createBindingSet(bindingSetDesc, m_ToneMappingBindingLayout);


        }




    }
    void ToneMappingRenderer::Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) {
        SCOPE_TIME_COUNTER(ToneMapping);




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

        psoDesc.VS = m_ToneMappingShader->GetVertexShader();
        psoDesc.PS = m_ToneMappingShader->GetPixelShader();
        psoDesc.bFromPSOFileCache = false;
        //if (!m_bPlsCopy) {
            psoDesc.bindingLayouts.push_back(m_ToneMappingBindingLayout);
            psoDesc.bindingLayouts.push_back(m_ViewBindinglayout);
        //}



        if (!m_ToneMappingPso) {
            m_ToneMappingPso = m_Device->createGraphicsPipeline(psoDesc, targetFramebuffer);
        }
        graphicsPSO.pipeline = m_ToneMappingPso;
       // if (!m_bPlsCopy) {
            graphicsPSO.bindings.push_back(m_ToneMappingBindingSet);
            graphicsPSO.bindings.push_back(m_ViewBindingset);
      //  }
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
        ToneMappingConstants toneConst{};
        FillShaderParameters(toneConst);
        cmdList->writeBuffer(m_ToneMappingCB, &toneConst, sizeof(ToneMappingConstants));

        Draw(cmdList, drawItem);

        //cmdList->endRenderPass();
      //  cmdList->endMarker();


    }


    void ToneMappingRenderer::FillShaderParameters(ToneMappingConstants& output) {
    
        output.flag = 0;
        output.flag |= PostProcess_Gamma;
        output.flag |= PostProcess_Tonemapping;
        //todo:: other params

    
    }

}
