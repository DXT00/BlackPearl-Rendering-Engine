//
// Created by DXT00 on 2025/5/15.
//
#include "pch.h"

#include "Renderer/MasterRenderer/IBLProbeRenderer.h"
#include "RHI/RHIDevice.h"

namespace BlackPearl
{
    float IBLProbeRenderer::s_GICoeffs = 0.2f;

   
    void IBLProbeRenderer::Init()
    {
    }
    void IBLProbeRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
    {
    }
    void IBLProbeRenderer::FillShaderParameters()
    {
    }
}