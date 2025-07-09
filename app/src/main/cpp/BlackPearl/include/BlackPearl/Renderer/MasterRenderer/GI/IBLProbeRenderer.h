//
// Created by DXT00 on 2025/5/15.
//

#pragma once
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "RHI/RHIDevice.h"
#include "MainCamera/MainCamera.h"
#include "Renderer/MasterRenderer/ForwardShadingRenderer.h"
#include "Renderer/MasterRenderer/SkyboxRenderer.h"
#include "GIRenderer.h"
#include "hlsl/core/deferred_lighting_cb.h"

namespace BlackPearl{

    struct CubeMapKey {
        int width, height;
        int mipCnt;
        int format;

        std::string ToString() {
            return std::to_string(width) + "_"
                + std::to_string(height) + "_"
                + std::to_string(mipCnt) + "_"
                + std::to_string(format);
        }

    };
    class GIManager;

    class IBLProbeRenderer: public BasicRenderer, public GIRenderer
    {
    public:

        void Init(Scene* scene);
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
        virtual void RenderIndirectLight(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene) override;
        virtual void ShowProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;




        void FillShaderParameters();
        void FillProbesParameters(const std::vector<Object*>& probes, DeferredLightingConstants& output);
        void RenderSpecularBRDFLUTMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);
        void UpdateProbeCamera(Object* probe);

        static float s_GICoeffs;

    private:


        void UpdateDiffuseProbesMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* diffuseProbe);
        void UpdateReflectionProbesMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* diffuseProbe);
        /*render environment CubeMap of each probe */
        TextureHandle RenderEnvironmerntCubeMaps(ICommandList* cmdList, Scene* scene,  Object* probes);
        //void RenderDiffuseIrradianceMap(const LightSources* lightSources, std::vector<Object*> objects, Object *probe);
        void RenderSpecularPrefilterMap(ICommandList* cmdList, IFramebuffer* targetFramebuffer, const LightSources* lightSources, Object* probe, TextureHandle environmentMap);

        void RenderSHImage(Object* probe, TextureHandle environmentMap);

        void _RenderProbe(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene, Object* probe);
        void _InitDeferredLighting();
        void _InitProbeDebug();
        void _InitProbeBake();

    private:
        friend class GIManager;
        IBLProbeRenderer(IDevice* device);

        /*draw lighprobes shader*/
        MaterialShader*     m_ProbeDebugShader = nullptr;
        /*shader*/
        MaterialShader*		m_IBLShader = nullptr; //scene renderer
        MaterialShader*		m_IrradianceShader = nullptr; //create diffuse irradianceCubeMap
        MaterialShader*		m_SpecularPrefilterShader = nullptr; //specular prefilter shader
        MaterialShader*		m_SpecularBRDFLutShader = nullptr;  // brdf LUT shader
        MaterialShader*		m_PbrShader = nullptr;
        MaterialShader*		m_NonPbrShader = nullptr;
        MainCamera* m_ProbeCamera;

     
        InstancedOpaqueDrawStrategy* m_DrawStrategy;


        GraphicsPipelineHandle m_BrdfLUTPso = nullptr;


        ForwardShadingRenderer* m_EnvironmentMapRenderer = nullptr;
        SkyboxRenderer* m_EnvironmentMapSkyboxRenderer = nullptr;

        BufferHandle    m_SpecularPrefilterCB;
        GraphicsPipelineHandle m_SpecularPrefilterPso = nullptr;

        //TODO:: Probe Material;
        BindingLayoutHandle m_ProbeBindingLayout;
        BindingSetHandle    m_ProbeBindingSet;
        GraphicsPipelineHandle m_ProbePso = nullptr;
        BufferHandle    m_ProbeCB;

        bool m_IsInitial = false;

        //Deferred IndirectLight shading
        MaterialShader* m_DeferredIBLShader = nullptr;

        BufferHandle        m_LightsCB;
        BindingLayoutHandle m_DeferredShadingBindingLayout;
        BindingSetHandle    m_DeferredShadingBindingSet;
        GraphicsPipelineHandle m_DeferredShadingIBLPso = nullptr;


    };
}