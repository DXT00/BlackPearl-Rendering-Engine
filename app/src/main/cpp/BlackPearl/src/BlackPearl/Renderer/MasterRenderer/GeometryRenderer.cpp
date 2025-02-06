#include "pch.h"
#include "BasePassRenderer.h"
#include "BlackPearl/Renderer/Shader/ShaderFactory.h"
#include "BlackPearl/RHI/PipelineStateCache.h"
namespace BlackPearl {
    extern ShaderFactory* g_shaderFactory;
    /* 流程仿照ue: */
    /*
        //BeginRenderPass会cache renderTarget
        RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("Test_Clear_DrawColoredQuad"));

    	FIntPoint DisplacementMapResolution(OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY());

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

		// Get shaders.
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef< FLensDistortionUVGenerationVS > VertexShader(GlobalShaderMap);
		TShaderMapRef< FLensDistortionUVGenerationPS > PixelShader(GlobalShaderMap);

		// Set the graphic pipeline state.
		FGraphicsPipelineStateInitializer GraphicsPSOInit;
		RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
		GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
		GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
		GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
		GraphicsPSOInit.PrimitiveType = PT_TriangleList;
		GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
		GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
		GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
		SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

		// Update viewport.
		RHICmdList.SetViewport(
			0, 0, 0.f,
			OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);

		// Update shader uniform parameters.

		SetShaderParametersLegacyVS(RHICmdList, VertexShader, CompiledCameraModel, DisplacementMapResolution);
		SetShaderParametersLegacyPS(RHICmdList, PixelShader, CompiledCameraModel, DisplacementMapResolution);

		// Draw grid.
		uint32 PrimitiveCount = kGridSubdivisionX * kGridSubdivisionY * 2;
		RHICmdList.DrawPrimitive(0, PrimitiveCount, 1);
	}
	RHICmdList.EndRenderPass();

	RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));
    
    
    
    
    
    */
    bool SetupMaterial(const Material* material, RasterCullMode cullMode, GraphicsState& state) {
        // auto& context = static_cast<Context&>(abstractContext);

        IBindingSet* materialBindingSet = m_MaterialBindings->GetMaterialBindingSet(material);

        if (!materialBindingSet)
            return false;

        if (material->domain >= MaterialDomain::Count || cullMode > RasterCullMode::None)
        {
            assert(false);
            return false;
        }

        //TODO:: 加pass context
        PipelineKey key;// = context.keyTemplate;
        key.value = material->GetId();
        key.bits.cullMode = cullMode;
        key.bits.domain = material->domain;
        key.bits.frontCounterClockwise = true;
        key.bits.reverseDepth = false;

        GraphicsPipelineHandle& pipeline = m_Pipelines[key.value];
        // GraphicsPipelineHandle pipeline = CreateGraphicsPipeline(key, state.framebuffer);
        if (!pipeline)
        {
            std::lock_guard<std::mutex> lockGuard(m_Mutex);

            if (!pipeline)
                pipeline = CreateGraphicsPipeline(key, state.framebuffer);

            if (!pipeline)
                return false;
        }

        assert(pipeline->getFramebufferInfo() == state.framebuffer->getFramebufferInfo());

        state.pipeline = pipeline;
        //state.bindings = { materialBindingSet, m_ViewBindingSet, context.lightBindingSet };
       // state.bindings = { materialBindingSet, m_ViewBindingSet, m_LightBinding };
        state.bindings = { materialBindingSet, m_ViewBindingSet };


        return true;
    }

    void RenderPassTemplate(ICommandList* cmdList, IFramebuffer* framebuffer, IView* view, IDrawStrategy* drawStrategy)
    {


        for (const auto& item : drawStrategy->GetDrawItems()) {
            if (item.material == nullptr)
                continue;

        }
        std::vector<ShaderMacro> Macros;
       // Macros.push_back(ShaderMacro("COMPILE_SHADER", "1"));
        ShaderHandle vertexShader = g_shaderFactory->CreateShader("hlsl/test/forward_test_vs.hlsl", "main", &Macros, ShaderType::Vertex);
        ShaderHandle pixelShader = g_shaderFactory->CreateShader("hlsl/test/forward_test_vs.hlsl", "main", &Macros, ShaderType::Pixel);

        //资源准备 vertexBuffers, indexBuffer, inputLayout, bindingLayout(uniform resource)
        std::vector<VertexBufferBinding> vertexBuffers;
        IndexBufferBinding indexBuffer;
        const VertexAttributeDesc inputDescs[] =
        {
            GetVertexAttributeDesc(VertexAttribute::Position, "POS", 0),
            GetVertexAttributeDesc(VertexAttribute::PrevPosition, "PREV_POS", 1),
            GetVertexAttributeDesc(VertexAttribute::TexCoord1, "TEXCOORD", 2),
            GetVertexAttributeDesc(VertexAttribute::Normal, "NORMAL", 3),
            //TODO::
           // GetVertexAttributeDesc(VertexAttribute::Tangent, "TANGENT", 4),
            GetVertexAttributeDesc(VertexAttribute::Transform, "TRANSFORM", 4),
        };

        InputLayoutHandle inputLayout = cmdList->getDevice()->createInputLayout(inputDescs, uint32_t(std::size(inputDescs)));

        RHIBindingLayoutDesc viewLayoutDesc;
        viewLayoutDesc.visibility = ShaderType::All;
        viewLayoutDesc.bindings = {
            RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
            RHIBindingLayoutItem::RT_Sampler(1)
        };

        BindingLayoutHandle bindinglayout = cmdList->getDevice()->createBindingLayout(viewLayoutDesc);

        GraphicsState graphicsPSO;
        graphicsPSO.framebuffer = framebuffer;
        graphicsPSO.viewport = view->GetViewportState();
        graphicsPSO.shadingRateState = view->GetVariableRateShadingState();

        // Update viewport.
        //cmdList->setViewport(
        //    0, 0, 0.f,
        //    DisplacementMapResolution.X, DisplacementMapResolution.Y, 1.f);

        //// Get shaders.
        //FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
        //TShaderMapRef< FLensDistortionUVGenerationVS > VertexShader(GlobalShaderMap);
        //TShaderMapRef< FLensDistortionUVGenerationPS > PixelShader(GlobalShaderMap);

        // Set the graphic pipeline state.
        //FGraphicsPipelineStateInitializer GraphicsPSOInit;
       // cmdList->ApplyCachedRenderTargets(graphicsPSO);
        graphicsPSO.pipeline->desc.depthStencilState.setDepthFunc(ComparisonFunc::LessOrEqual);
        graphicsPSO.pipeline->desc.blendState.alphaToCoverageEnable = false;
        graphicsPSO.pipeline->desc.rasterState.frontCounterClockwise = true;
        graphicsPSO.pipeline->desc.rasterState.cullMode = RasterCullMode::Back;
        graphicsPSO.pipeline->desc.primType = PrimitiveType::TriangleList;
        graphicsPSO.pipeline->desc.inputLayout = inputLayout;
       // graphicsPSO.pipeline->desc.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
        graphicsPSO.pipeline->desc.VS = vertexShader;
        graphicsPSO.pipeline->desc.PS = pixelShader;
        graphicsPSO.pipeline->desc.bindingLayouts = { bindinglayout };
        graphicsPSO.indexBuffer = indexBuffer;
        graphicsPSO.vertexBuffers = vertexBuffers;

        //TODO::
       // SetGraphicsPipelineState(cmdList, graphicsPSO, 0);
        cmdList->setGraphicsState(graphicsPSO);

        // pipelineDesc.bindingLayouts = { m_MaterialBindings->GetLayout(), m_ViewBindingLayout, m_LightBindingLayout };


        // Update viewport.
        /*cmdList->setViewport(
            0, 0, 0.f,
            OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);*/

        // Update shader uniform parameters.

       // SetShaderParametersLegacyVS(cmdList, VertexShader, CompiledCameraModel, DisplacementMapResolution);
       // SetShaderParametersLegacyPS(cmdList, PixelShader, CompiledCameraModel, DisplacementMapResolution);

        // Draw grid.
       /* uint32_t PrimitiveCount = kGridSubdivisionX * kGridSubdivisionY * 2;
        cmdList->DrawPrimitive(0, PrimitiveCount, 1);*/

        DrawArguments args;
        cmdList->draw(args);
   /* }
    RHICmdList.EndRenderPass();

    RHICmdList.Transition(FRHITransitionInfo(RenderTargetTexture, ERHIAccess::RTV, ERHIAccess::SRVMask));*/
    }



    void RenderView(ICommandList* commandList, IFramebuffer* framebuffer, IView* view, IView* viewPrev, IDrawStrategy* drawStrategy, GeometryRenderer* pass, bool materialEvents)
    {
        pass->SetupView(commandList, view, viewPrev);

        const Material* lastMaterial = nullptr;
        const BufferGroup* lastBuffers = nullptr;
        RasterCullMode lastCullMode = RasterCullMode::Back;

        bool drawMaterial = true;
        bool stateValid = false;

        const Material* eventMaterial = nullptr;

        GraphicsState graphicsState;
        graphicsState.framebuffer = framebuffer;
        graphicsState.viewport = view->GetViewportState();
        graphicsState.shadingRateState = view->GetVariableRateShadingState();

        DrawArguments currentDraw;
        currentDraw.instanceCount = 0;

        auto flushDraw = [commandList, materialEvents, &graphicsState, &currentDraw, &eventMaterial, &pass](const Material* material)
            {
                if (currentDraw.instanceCount == 0)
                    return;

                if (materialEvents && material != eventMaterial)
                {
                    if (eventMaterial)
                        commandList->endMarker();

                    if (material->name.empty())
                    {
                        eventMaterial = nullptr;
                    }
                    else
                    {
                        commandList->beginMarker(material->name.c_str());
                        eventMaterial = material;
                    }
                }


                if(currentDraw.drawIndex)
                    commandList->drawIndexed(currentDraw);
                else
                    commandList->draw(currentDraw);

                currentDraw.instanceCount = 0;
            };

        for (const auto& item: drawStrategy->GetDrawItems())
        {
            if (item.material == nullptr)
                continue;


            bool newBuffers = item.buffers != lastBuffers;
            bool newMaterial = item.material != lastMaterial || item.cullMode != lastCullMode;

            if (newBuffers || newMaterial)
            {
                flushDraw(lastMaterial);
            }

            if (newBuffers)
            {
                pass->SetupInputBuffers(commandList, const_cast<BufferGroup*>(item.buffers), item.transform, graphicsState);

                lastBuffers = item.buffers;
                stateValid = false;
            }
            //else {
            //    //pass->SetupInputBuffers(commandList, const_cast<BufferGroup*>(lastBuffers), item.transform, graphicsState);

            //    graphicsState.vertexBuffers = commandList->GetLastGraphicsState().vertexBuffers;
            //    graphicsState.indexBuffer = commandList->GetLastGraphicsState().indexBuffer;

            //}

            if (newMaterial)
            {
                if (item.material->isDirty) {
                    MaterialConstants cb = const_cast<Material*>(item.material)->FillMaterialConstants();
                    commandList->writeBuffer(item.material->materialConstants,
                        &cb,
                        sizeof(MaterialConstants));
                    const_cast<Material*>(item.material)->isDirty = false;
                }
                drawMaterial = pass->SetupMaterial(item.material, item.cullMode, graphicsState);

                lastMaterial = item.material;
                lastCullMode = item.cullMode;
                stateValid = false;
            }

            if (drawMaterial)
            {
                if (!stateValid)
                {

                    commandList->setGraphicsState(graphicsState);
                    stateValid = true;
                }

                DrawArguments args;
                if (item.mesh->m_IndicesCount == 0) {
                    args.drawIndex = false;
                    args.vertexCount = item.mesh->m_VerticeCount;
                }
                else {
                    args.drawIndex = true;
                    args.vertexCount = item.mesh->m_IndicesCount;// numIndices;

                }
                args.instanceCount = 1;
                args.startVertexLocation = item.mesh->vertexOffset;// +item.geometry.vertexOffsetInMesh;
                args.startIndexLocation = item.mesh->indexOffset;// +item.geometry.indexOffsetInMesh;
                args.startInstanceLocation = 0;// item.instance.GetInstanceIndex();

                if (currentDraw.instanceCount > 0 &&
                    currentDraw.startIndexLocation == args.startIndexLocation &&
                    currentDraw.startInstanceLocation + currentDraw.instanceCount == args.startInstanceLocation)
                {
                    currentDraw.instanceCount += 1;
                }
                else
                {
                    flushDraw(item.material);

                    currentDraw = args;
                }
            }
        }

        flushDraw(lastMaterial);

        if (materialEvents && eventMaterial)
            commandList->endMarker();
        

           
    }
}