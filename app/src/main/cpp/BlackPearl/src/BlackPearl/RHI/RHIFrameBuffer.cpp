#include "pch.h"
#include "RHIFrameBuffer.h"

namespace BlackPearl {

	FramebufferInfo::FramebufferInfo(const FramebufferDesc& desc)
	{
        for (size_t i = 0; i < desc.colorAttachments.size(); i++)
        {
            const FramebufferAttachment& attachment = desc.colorAttachments[i];
            colorFormats.push_back(attachment.format == Format::UNKNOWN && attachment.texture ? attachment.texture->getDesc().format : attachment.format);
        }

        if (desc.depthAttachment.valid())
        {
            const TextureDesc& textureDesc = desc.depthAttachment.texture->getDesc();
            depthFormat = textureDesc.format;
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
        else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].valid())
        {
            const TextureDesc& textureDesc = desc.colorAttachments[0].texture->getDesc();
            sampleCount = textureDesc.sampleCount;
            sampleQuality = textureDesc.sampleQuality;
        }
	}

    FramebufferInfoEx::FramebufferInfoEx(const FramebufferDesc& desc)
        : FramebufferInfo(desc)
    {
        if (desc.depthAttachment.valid())
        {
            const TextureDesc& textureDesc = desc.depthAttachment.texture->getDesc();
            width = std::max(textureDesc.width >> desc.depthAttachment.subresources.baseMipLevel, 1u);
            height = std::max(textureDesc.height >> desc.depthAttachment.subresources.baseMipLevel, 1u);
        }
        else if (!desc.colorAttachments.empty() && desc.colorAttachments[0].valid())
        {
            const TextureDesc& textureDesc = desc.colorAttachments[0].texture->getDesc();
            width = std::max(textureDesc.width >> desc.colorAttachments[0].subresources.baseMipLevel, 1u);
            height = std::max(textureDesc.height >> desc.colorAttachments[0].subresources.baseMipLevel, 1u);
        }
    }

}
