#include "Renderer/MasterRenderer/BloomRenderer.h"
#include "hlsl/core/bloom_cb.h"

namespace BlackPearl {


	BloomRenderer::BloomRenderer(IDevice* device)
		:BasicRenderer(device)
	{
	}

	void BloomRenderer::Init()
	{
        BufferDesc constantBufferDesc;
        constantBufferDesc.byteSize = sizeof(BloomConstants);
        constantBufferDesc.isConstantBuffer = true;
        constantBufferDesc.isVolatile = true;
        constantBufferDesc.debugName = "BloomConstantsH";
        m_BloomHBlurCB = m_Device->createBuffer(constantBufferDesc);
        constantBufferDesc.debugName = "BloomConstantsV";
        m_BloomVBlurCB = m_Device->createBuffer(constantBufferDesc);

        RHIBindingLayoutDesc layoutDesc;
        layoutDesc.visibility = ShaderType::Pixel;
        layoutDesc.bindings = {
             RHIBindingLayoutItem::RT_VolatileConstantBuffer(0),
             RHIBindingLayoutItem::RT_Sampler(0),
             RHIBindingLayoutItem::RT_Texture_SRV(0)
        };
        m_BloomBlurBindingLayout = m_Device->createBindingLayout(layoutDesc);

	}

	void BloomRenderer::Render(ICommandList* commandList, IFramebuffer* targetFramebuffer, Scene* scene)
	{
	}
}

