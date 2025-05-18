//
// Created by DXT00 on 2025/5/15.
//

#pragma once
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "RHI/RHIDevice.h"

namespace BlackPearl{

    class IBLProbeRenderer: public BasicRenderer
    {
    public:
        IBLProbeRenderer(IDevice* device):
            BasicRenderer(device){ }

        void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        static void FillShaderParameters();

        static float s_GICoeffs;

    private:

        float m_TotalTimeIntervalS = 50.0f;//second
        float m_StateIntervalS = m_TotalTimeIntervalS / 3.0f;


        //MaterialShader* m_SSRPassShader = nullptr;

        TextureHandle   m_SkyboxTexture[3];
        BufferHandle    m_SkyCB;

        //TODO:: Skybox Material;
        BindingLayoutHandle m_SkyboxBindingLayout;
        BindingSetHandle    m_SkyboxBindingSet;
        GraphicsPipelineHandle m_SkyboxPso = nullptr;


    };
}