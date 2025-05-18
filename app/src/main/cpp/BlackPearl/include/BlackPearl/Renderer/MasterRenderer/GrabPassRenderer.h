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
        void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        static void FillShaderParameters();
        static bool s_HDR;

    private:
        std::shared_ptr<MaterialShader> m_FinalScreenShader;
        /* tone mapping : hdr */
        BufferHandle m_HDRFrameBuffer;
        TextureHandle m_HDRPostProcessTexture;
        Object* m_FinalScreenQuad = nullptr;

    };
}

