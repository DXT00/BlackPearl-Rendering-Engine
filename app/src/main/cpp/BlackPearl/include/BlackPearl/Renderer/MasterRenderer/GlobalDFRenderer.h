#pragma once
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "Renderer/SDF/GlobalDistanceField.h"
#include "RHI/RHIPipeline.h"
namespace BlackPearl {

    class GlobalDFRenderer : public BasicRenderer
    {
    public:
        GlobalDFRenderer(IDevice* device);

        void Init(Scene* scene);
        void Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene);
        void FillShaderParameters();

        void ShowGDF(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene);

        TextureHandle GetGDFDebugTexture() const{ return m_GDFDebugBinding.debugOutput; }
    private:

        void _InitGDFDebug();
        MaterialShader* m_GDFBakeShader = nullptr;
        MaterialShader* m_GDFDebugShader = nullptr;

        //GlobalDistanceField m_GDF;

        BufferHandle m_GDFCB;
        BufferHandle m_SceneObjectsCB;

        BindingLayoutHandle m_GDFBindingLayout;
        std::vector<BindingSetHandle>    m_GDFBindingSets;

        ComputePipelineHandle m_GDFPso = nullptr;


        struct GDFDebugBinding {

            BindingLayoutHandle layout;
            BindingSetHandle     set;
            BufferHandle        gdfCb;
            TextureHandle       debugOutput;

        };
        GraphicsPipelineHandle m_GDFDebugPso = nullptr;
        GDFDebugBinding         m_GDFDebugBinding;


        InstancedOpaqueDrawStrategy* m_DrawStrategy;

    };

}

