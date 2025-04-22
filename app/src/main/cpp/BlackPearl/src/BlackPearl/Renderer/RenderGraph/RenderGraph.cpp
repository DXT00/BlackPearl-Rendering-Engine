#include "pch.h"
#include "Renderer/RenderGraph/RenderGraph.h"
#include "Renderer/DeviceManager.h"

namespace BlackPearl {
    void RenderGraph::AddPass(BasicRenderer* renderer)
    {
        mRenderPasses.push_back(renderer);
    }
}