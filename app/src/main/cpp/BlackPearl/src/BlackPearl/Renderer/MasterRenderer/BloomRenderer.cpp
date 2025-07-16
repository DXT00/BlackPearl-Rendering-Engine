#include "Renderer/MasterRenderer/BloomRenderer.h"
#include "hlsl/core/bloom_cb.h"
#include "Application.h"
namespace BlackPearl {


	BloomRenderer::BloomRenderer(IDevice* device)
		:BasicRenderer(device)
	{
	}

	void BloomRenderer::Init()
	{


        m_BloomBlurPixelShader = DBG_NEW MaterialShader("assets/shaders/glsl/grab_pass/FinalScreenQuad.glsl");

        BufferDesc constantBufferDesc;
        constantBufferDesc.byteSize = sizeof(BloomConstants);
        constantBufferDesc.isConstantBuffer = true;
        constantBufferDesc.isVolatile = true;
        constantBufferDesc.debugName = "BloomConstantsH";
        m_BloomHBlurCB = m_Device->createBuffer(constantBufferDesc);
        constantBufferDesc.debugName = "BloomConstantsV";
        m_BloomVBlurCB = m_Device->createBuffer(constantBufferDesc);



        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
             RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
             RHIBindingLayoutItem::RT_Sampler(0),
             RHIBindingLayoutItem::RT_Texture_SRV(0)
        };
        m_BloomBlurBindingLayout = m_Device->createBindingLayout(layoutDesc);

        math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();

        m_PerViewData.resize(m_PassCnt);
        for (uint viewIndex = 0; viewIndex < m_PassCnt; viewIndex++)
        {
           // const IView* view = compositeView.GetChildView(ViewType::PLANAR, viewIndex);

          //  RHIIFramebuffer* sampleFramebuffer = m_FramebufferFactory->GetFramebuffer(*view);
            PerViewData& perViewData = m_PerViewData[viewIndex];

            RHIRect viewExtent = RHIRect(windowSize.x >> viewIndex, windowSize.y >> viewIndex);
            int viewportWidth = viewExtent.maxX - viewExtent.minX;
            int viewportHeight = viewExtent.maxY - viewExtent.minY;

            // temporary textures for downscaling
            TextureDesc downscaleTextureDesc;
            downscaleTextureDesc.format = Format::RGBA8_UNORM;
            downscaleTextureDesc.width = (uint32_t)ceil(viewportWidth / 2.f);
            downscaleTextureDesc.height = (uint32_t)ceil(viewportHeight / 2.f);
            downscaleTextureDesc.mipLevelsCnt = 1;
            downscaleTextureDesc.isRenderTarget = true;
            downscaleTextureDesc.debugName = "bloom src mip1";
            downscaleTextureDesc.initialState = ResourceStates::ShaderResource;
            downscaleTextureDesc.keepInitialState = true;
            perViewData.textureDownscale1 = m_Device->createTexture(downscaleTextureDesc);
            perViewData.framebufferDownscale1 = m_Device->createFramebuffer(FramebufferDesc()
                .addColorAttachment(perViewData.textureDownscale1));

            downscaleTextureDesc.debugName = "bloom src mip2";
            downscaleTextureDesc.width = (uint32_t)ceil(downscaleTextureDesc.width / 2.f);
            downscaleTextureDesc.height = (uint32_t)ceil(downscaleTextureDesc.height / 2.f);
            perViewData.textureDownscale2 = m_Device->createTexture(downscaleTextureDesc);
            perViewData.framebufferDownscale2 = m_Device->createFramebuffer(FramebufferDesc()
                .addColorAttachment(perViewData.textureDownscale2));

            // intermediate textures for accumulating blur
            TextureDesc intermediateTextureDesc2;
            intermediateTextureDesc2.format = downscaleTextureDesc.format;
            intermediateTextureDesc2.width = downscaleTextureDesc.width;
            intermediateTextureDesc2.height = downscaleTextureDesc.height;
            intermediateTextureDesc2.mipLevelsCnt = 1;
            intermediateTextureDesc2.isRenderTarget = true;

            intermediateTextureDesc2.debugName = "bloom accumulation pass1";
            intermediateTextureDesc2.initialState = ResourceStates::ShaderResource;
            intermediateTextureDesc2.keepInitialState = true;
            perViewData.texturePass1Blur = m_Device->createTexture(intermediateTextureDesc2);
            perViewData.framebufferPass1Blur = m_Device->createFramebuffer(FramebufferDesc()
                .addColorAttachment(perViewData.texturePass1Blur));

            intermediateTextureDesc2.debugName = "bloom accumulation pass2";
            perViewData.texturePass2Blur = m_Device->createTexture(intermediateTextureDesc2);
            perViewData.framebufferPass2Blur = m_Device->createFramebuffer(FramebufferDesc()
                .addColorAttachment(perViewData.texturePass2Blur));

            GraphicsPipelineDesc psoDesc;
            psoDesc.VS = m_BloomBlurPixelShader->GetVertexShader();
            psoDesc.PS = m_BloomBlurPixelShader->GetPixelShader();
            psoDesc.bindingLayouts = { m_BloomBlurBindingLayout };
            psoDesc.rasterState.cullMode = RasterCullMode::None;
            psoDesc.depthStencilState.depthTestEnable = false;
            psoDesc.depthStencilState.stencilEnable = false;
            perViewData.bloomBlurPso = m_Device->createGraphicsPipeline(psoDesc, perViewData.framebufferPass1Blur);

            BindingSetDesc bindingSetDesc;
            bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(0, m_BloomHBlurCB),
                //BindingSetItem::Sampler(0, m_CommonPasses->m_LinearClampSampler),
                BindingSetItem::Texture_SRV(0, perViewData.textureDownscale2)
            };
            perViewData.bloomBlurBindingSetPass1 = m_Device->createBindingSet(bindingSetDesc, m_BloomBlurBindingLayout);

            bindingSetDesc.bindings = {
                BindingSetItem::ConstantBuffer(0, m_BloomVBlurCB),
              //  BindingSetItem::Sampler(0, m_CommonPasses->m_LinearClampSampler),
                BindingSetItem::Texture_SRV(0, perViewData.texturePass1Blur)
            };
            perViewData.bloomBlurBindingSetPass2 = m_Device->createBindingSet(bindingSetDesc, m_BloomBlurBindingLayout);
        }
	}

    void BloomRenderer::RenderUI(IFramebuffer* framebuffer, IView* View) {
#ifdef USE_IMGUI
        ImGui::Begin("BloomRenderer Settings");
        ImGui::Text("sigmaInPixels: %d ", m_UI.sigmaInPixels);

        ImGui::End();
#endif
    }

	void BloomRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{

        float effectiveSigma = math::clamp(m_UI.sigmaInPixels * 0.25f, 1.f, 100.f);

        commandList->beginMarker("Bloom");

        DrawArguments fullscreenquadargs;
        fullscreenquadargs.instanceCount = 1;
        fullscreenquadargs.vertexCount = 4;


        math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();
        for (uint viewIndex = 0; viewIndex < m_PassCnt; viewIndex++)
        {
            const SceneData* sceneView = Renderer::GetSceneData();



            SceneData* view = DBG_NEW SceneData({sceneView->ProjectionViewMatrix ,sceneView->ViewMatrix,sceneView->ProjectionMatrix,
                sceneView->CameraPosition,sceneView->CameraRotation, sceneView->CameraFront, *scene->GetLightSources() });
            math::vector<int, 2> windowSize = Application::Get().GetWindow().GetCurWindowSize();

			float curWidth = windowSize.x >> viewIndex;
			float curHeight = windowSize.y >> viewIndex;

			view->SetViewport(RHIViewport(windowSize.x >> viewIndex, windowSize.y >> viewIndex));
			view->ViewFrustum = math::frustum(Math::ToFloat4x4(view->ViewMatrix * view->ProjectionMatrix), view->ReverseZ);

			view->zNear = sceneView->zNear;
			view->zFar = sceneView->zFar;
        
           // nvrhi::IFramebuffer* framebuffer = framebufferFactory->GetFramebuffer(*view);
            PerViewData& perViewData = m_PerViewData[viewIndex];

           ViewportState viewportState = view->GetViewportState();
            const RHIRect& scissorRect = viewportState.scissorRects[0];
           // const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();

            // downscale
            {
                commandList->beginMarker("Downscale");

                math::box2 uvSrcRect = math::box2(
                    float2(
                        float(scissorRect.minX) / curWidth,
                        float(scissorRect.minY) / curHeight),
                    float2(
                        float(scissorRect.maxX) / curWidth,
                        float(scissorRect.maxY) / curHeight)
                );

                //// half-scale down

                //BlitParameters blitParams1;
                //blitParams1.targetFramebuffer = perViewData.framebufferDownscale1;
                //blitParams1.sourceTexture = sourceDestTexture;
                //blitParams1.sourceBox = uvSrcRect;
                //m_CommonPasses->BlitTexture(commandList, blitParams1, &m_BindingCache);

                //// half-scale again down to quarter-scale

                //BlitParameters blitParams2;
                //blitParams2.targetFramebuffer = perViewData.framebufferDownscale2;
                //blitParams2.sourceTexture = perViewData.textureDownscale1;
                //m_CommonPasses->BlitTexture(commandList, blitParams2, &m_BindingCache);

                commandList->endMarker(); // "Downscale"
            }

            // apply blur
            {
                commandList->beginMarker("Blur");
                //nvrhi::Viewport viewport;

                //nvrhi::GraphicsState state;
                //state.pipeline = perViewData.bloomBlurPso;
                //viewport = nvrhi::Viewport(float(perViewData.texturePass1Blur->getDesc().width), float(perViewData.texturePass1Blur->getDesc().height));
                //state.viewport.addViewport(viewport);
                //state.viewport.addScissorRect(nvrhi::Rect(viewport));
                //state.framebuffer = perViewData.framebufferPass1Blur;
                //state.bindings = { perViewData.bloomBlurBindingSetPass1 };

                //BloomConstants bloomHorizonal = {};
                //bloomHorizonal.pixstep.x = 1.f / perViewData.texturePass1Blur->getDesc().width;
                //bloomHorizonal.pixstep.y = 0.f;
                //bloomHorizonal.argumentScale = -1.f / (2 * effectiveSigma * effectiveSigma);
                //bloomHorizonal.normalizationScale = 1.f / (sqrtf(2 * PI_f) * effectiveSigma);
                //bloomHorizonal.numSamples = ::round(effectiveSigma * 4.f);
                //BloomConstants bloomVertical = bloomHorizonal;
                //bloomVertical.pixstep.x = 0.f;
                //bloomVertical.pixstep.y = 1.f / perViewData.texturePass1Blur->getDesc().height;
                //commandList->writeBuffer(m_BloomHBlurCB, &bloomHorizonal, sizeof(bloomHorizonal));
                //commandList->writeBuffer(m_BloomVBlurCB, &bloomVertical, sizeof(bloomVertical));

                //commandList->setGraphicsState(state);
                //commandList->draw(fullscreenquadargs); // blur to m_TexturePass1Blur or m_TexturePass3Blur

                //viewport = nvrhi::Viewport(float(perViewData.texturePass2Blur->getDesc().width), float(perViewData.texturePass2Blur->getDesc().height));
                //state.viewport.viewports[0] = viewport;
                //state.viewport.scissorRects[0] = nvrhi::Rect(viewport);
                //state.framebuffer = perViewData.framebufferPass2Blur;
                //state.bindings = { perViewData.bloomBlurBindingSetPass2 };

                //commandList->setGraphicsState(state);
                //commandList->draw(fullscreenquadargs); // blur to m_TexturePass2Blur

                commandList->endMarker(); // "Blur"
            }

            // composite
            {
                commandList->beginMarker("Apply");

                //BlitParameters blitParams3;
                //blitParams3.targetFramebuffer = framebuffer;
                //blitParams3.targetViewport = viewportState.viewports[0];
                //blitParams3.sourceTexture = perViewData.texturePass2Blur;
                //blitParams3.blendState.setBlendEnable(true)
                //    .setSrcBlend(nvrhi::BlendFactor::ConstantColor)
                //    .setDestBlend(nvrhi::BlendFactor::InvConstantColor)
                //    .setSrcBlendAlpha(nvrhi::BlendFactor::Zero)
                //    .setDestBlendAlpha(nvrhi::BlendFactor::One);
                //blitParams3.blendConstantColor = nvrhi::Color(blendFactor);
                //m_CommonPasses->BlitTexture(commandList, blitParams3, &m_BindingCache);

                commandList->endMarker(); // "Apply"
            }
        }

        commandList->endMarker(); // "Bloom"

	}
}

