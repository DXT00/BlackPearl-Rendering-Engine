//
// Created by DXT00 on 2025/5/15.
//
#pragma once
#include "BasicRenderer.h"
#include "hlsl/core/deferred_lighting_cb.h"
namespace BlackPearl
{
    class PointLight;
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
        void RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);

        void ShowPointLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        
        //void RenderIBLProbes(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);


        void FillLightsParameters(Light* light, DeferredLightingConstants& output);

    private:
        static float CalculateSphereRadius(Object* pointLight);

        void _RenderPointLightMask(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene, PointLight* pointLight);
        void _RenderPointLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene, PointLight* pointLight);

    private:




        /* write pos,normal,color to gBuffer */
        MaterialShader* m_DeferredPointLightShader = nullptr;
        MaterialShader* m_DeferredPointLightStencilShader = nullptr;
        MaterialShader* m_DeferredPointLightDebugShader = nullptr;

        MaterialShader* m_DeferredDirectionLightShader = nullptr;

        ShaderParameters m_ShaderParameters[ShaderType::NUM_COMPILE_SHADER_STAGES];


        BufferHandle        m_LightsCB;
        BindingLayoutHandle m_DeferredShadingBindingLayout;
        BindingSetHandle    m_DeferredShadingBindingSet;

        GraphicsPipelineHandle m_DeferredShadingPointLightPso = nullptr;
        GraphicsPipelineHandle m_DeferredShadingPointLightMaskPso = nullptr;
        GraphicsPipelineHandle m_DeferredShadingPointLightDebugPso = nullptr;

        GraphicsPipelineHandle m_DeferredShadingDirectionLightPso = nullptr;



    };
}