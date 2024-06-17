#include "pch.h"
#include "CommonRenderPass.h"

namespace BlackPearl {

  

    CommonRenderPasses::CommonRenderPasses(IDevice* device, std::shared_ptr<ShaderFactory> shaderFactory)
    {
    }

    void CommonRenderPasses::BlitTexture(ICommandList* commandList, const BlitParameters& params, BindingCache* bindingCache)
    {
    }

    void CommonRenderPasses::BlitTexture(ICommandList* commandList, IFramebuffer* targetFramebuffer, ITexture* sourceTexture, BindingCache* bindingCache)
    {
    }

}