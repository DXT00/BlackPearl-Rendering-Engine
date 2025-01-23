#include "pch.h"
#include "BasePassRenderer.h"

namespace BlackPearl {
    /* Á÷³Ì·ÂÕÕue: */
    /*
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


    void RenderPassTemplate(ICommandList* cmdList, IFramebuffer* framebuffer, IView* view) {

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
        cmdList->ApplyCachedRenderTargets(graphicsPSO);
        graphicsPSO.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
        graphicsPSO.BlendState = TStaticBlendState<>::GetRHI();
        graphicsPSO.RasterizerState = TStaticRasterizerState<>::GetRHI();
        graphicsPSO.PrimitiveType = PT_TriangleList;
        graphicsPSO.BoundShaderState.VertexDeclarationRHI = GetVertexDeclarationFVector4();
        graphicsPSO.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
        graphicsPSO.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
        SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

        // Update viewport.
        cmdList->setViewport(
            0, 0, 0.f,
            OutTextureRenderTargetResource->GetSizeX(), OutTextureRenderTargetResource->GetSizeY(), 1.f);

        // Update shader uniform parameters.

        SetShaderParametersLegacyVS(RHICmdList, VertexShader, CompiledCameraModel, DisplacementMapResolution);
        SetShaderParametersLegacyPS(RHICmdList, PixelShader, CompiledCameraModel, DisplacementMapResolution);

        // Draw grid.
        uint32_t PrimitiveCount = kGridSubdivisionX * kGridSubdivisionY * 2;
        RHICmdList.DrawPrimitive(0, PrimitiveCount, 1);
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