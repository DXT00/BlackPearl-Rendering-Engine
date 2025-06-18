#include "pch.h"
#include "RHI/RHICommandList.h"

#include "BlackPearl/Renderer/DeviceManager.h"
namespace BlackPearl {
    ImmediateCommandList GImmediateCmdList;
    extern DeviceManager* g_deviceManager;
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


    void ImmediateCommandList::AcquireThreadOwnership()
    {
        if(g_deviceManager)
            g_deviceManager->GetDevice()->acquireThreadOwnership();
    }

    void ImmediateCommandList::ReleaseThreadOwnership()
    {
        if(g_deviceManager)
            g_deviceManager->GetDevice()->releaseThreadOwnership();
    }

    void RHIAcquireThreadOwnership()
    {
        return GImmediateCmdList.AcquireThreadOwnership();
    }

    void RHIReleaseThreadOwnership()
    {
        return GImmediateCmdList.ReleaseThreadOwnership();
    }

    int32_t RHIGetSyncInterval()
    {
        return math::max(Configuration::SyncInterval, 0);
    }


}