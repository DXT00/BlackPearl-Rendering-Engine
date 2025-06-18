//
// Created by DXT00 on 2025/5/15.
//
#pragma once
#include "BasicRenderer.h"
#include "hlsl/core/deferred_lighting_cb.h"
namespace BlackPearl
{
    enum ShadingType {
        ST_DirectionLight,
        ST_PointLight,
        ST_IBL
    };
    class DeferredShadingRenderer : public BasicRenderer
    {
        public:
        DeferredShadingRenderer(IDevice* device);

        void Init();
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

        void RenderDirectionLights(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        void RenderPointLights(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        void RenderIBLProbes(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        void FillLightsParameters(Light* light, DeferredLightingConstants& output);
        void FillProbesParameters(const std::vector<Object*>& probes, DeferredLightingConstants& output);

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


        /* write pos,normal,color to gBuffer */
        MaterialShader* m_DeferredPointLightShader = nullptr;
        MaterialShader* m_DeferredDirectionLightShader = nullptr;
        MaterialShader* m_DeferredIBLShader = nullptr;

        ShaderParameters m_ShaderParameters[ShaderType::NUM_COMPILE_SHADER_STAGES];


        BufferHandle        m_LightsCB;
        BindingLayoutHandle m_DeferredShadingBindingLayout;
        BindingSetHandle    m_DeferredShadingBindingSet;

        GraphicsPipelineHandle m_DeferredShadingPointLightPso = nullptr;
        GraphicsPipelineHandle m_DeferredShadingDirectionLightPso = nullptr;
        GraphicsPipelineHandle m_DeferredShadingIBLPso = nullptr;



    };
}