#pragma once
#include "ProbeRenderer.h"
#include "Renderer/MasterRenderer/BasicRenderer.h"
#include "Math/vector.h"
#include "RHI/RHITexture.h"
namespace BlackPearl {

    struct DDGIData {

    };



    class DDGIRenderer : public ProbeRenderer, public BasicRenderer
    {
    public:
        DDGIRenderer(IDevice* device);
        virtual void Init() override;
        virtual void Render(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
        virtual void RenderProbes(ICommandList* cmdList, IFramebuffer* targetFramebuffer, Scene* scene) override;
   
    private:

        // sw tracing
        void GenerateGDF();

      
    };
}
