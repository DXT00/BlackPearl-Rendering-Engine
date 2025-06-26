//
// Created by DXT00 on 2025/5/15.
//

#pragma once
#include "BasicRenderer.h"
#include "RHI/RHIDevice.h"
#include "Renderer/Shader/MaterialShader.h"
namespace BlackPearl {
    class GrabPassRenderer : public BasicRenderer {
    public:
        GrabPassRenderer(IDevice *device);
        /*if plsCopy is true: copy pls data to framebuffer*/
        void Init(TextureHandle grabTexture, bool plsCopy = false);
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        void FillShaderParameters();
        static bool s_HDR;

    private:
        MaterialShader* m_FinalScreenShader;



        BindingLayoutHandle m_GrabPassBindingLayout;
        BindingSetHandle    m_GrabPassBindingSet;

        GraphicsPipelineHandle m_GrabPassPso = nullptr;

        bool m_bPlsCopy = false;
//        /* tone mapping : hdr */
//        BufferHandle m_HDRFrameBuffer;
//        TextureHandle m_HDRPostProcessTexture;
//        Object* m_FinalScreenQuad = nullptr;

    };
}

