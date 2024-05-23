#include "pch.h"
#include "RHICommandList.h"


namespace BlackPearl {

	void ICommandList::setResourceStatesForFramebuffer(IFramebuffer* framebuffer)
	{
        const FramebufferDesc& desc = framebuffer->getDesc();

        for (const auto& attachment : desc.colorAttachments)
        {
            setTextureState(attachment.texture, attachment.subresources,
                ResourceStates::RenderTarget);
        }

        if (desc.depthAttachment.valid())
        {
            setTextureState(desc.depthAttachment.texture, desc.depthAttachment.subresources,
                desc.depthAttachment.isReadOnly ? ResourceStates::DepthRead : ResourceStates::DepthWrite);
        }
	}
}