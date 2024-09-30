#include "pch.h"
#include "RenderGraph.h"
#include "../DeviceManager.h"

namespace BlackPearl {
    void RenderGraph::AddPass(BasicRenderer* renderer)
    {
        mRenderPasses.push_back(renderer);
    }
}