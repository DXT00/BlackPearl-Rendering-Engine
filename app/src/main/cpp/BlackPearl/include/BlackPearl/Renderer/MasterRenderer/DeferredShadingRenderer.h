//
// Created by DXT00 on 2025/5/15.
//
#pragma once
#include "BasicRenderer.h"
namespace BlackPearl
{
    class DeferredShadingRenderer : public BasicRenderer
    {
        public:
        DeferredShadingRenderer(IDevice* device);

        void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        static void FillShaderParameters();

    private:
        static float CalculateSphereRadius(Object* pointLight);

    private:
        BufferHandle m_LightPassFrameBuffer;

        std::shared_ptr<MaterialShader> m_AmbientGIPassShader;
        std::shared_ptr<MaterialShader> m_PointLightPassShader;
        std::shared_ptr<MaterialShader> m_DirectionLightPassShader;


        /* AmbientGI pass */
        Object* m_GIQuad = nullptr;

        // todo:: use cluster lighting instead
        Object* m_SurroundSphere = nullptr;
        std::shared_ptr<MaterialShader> m_SphereDeBugShader;


    };
}