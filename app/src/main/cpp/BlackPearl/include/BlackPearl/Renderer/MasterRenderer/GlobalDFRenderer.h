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

        void VisualizeGDF();
    private:
        MaterialShader* m_GDFBakeShader = nullptr;
        //GlobalDistanceField m_GDF;

        BufferHandle m_GDFCB;
        BufferHandle m_SceneObjectsCB;

        BindingLayoutHandle m_GDFBindingLayout;
        std::vector<BindingSetHandle>    m_GDFBindingSets;

        ComputePipelineHandle m_GDFPso = nullptr;


        InstancedOpaqueDrawStrategy* m_DrawStrategy;

    };

}

