#include "pch.h"
#include "BasePassRenderer.h"

namespace BlackPearl {

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

                pass->SetPushConstants(commandList, graphicsState, currentDraw);

                commandList->drawIndexed(currentDraw);
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
                pass->SetupInputBuffers(item.buffers, graphicsState);

                lastBuffers = item.buffers;
                stateValid = false;
            }

            if (newMaterial)
            {
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
                args.vertexCount = item.mesh->totalIndices;// numIndices;
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