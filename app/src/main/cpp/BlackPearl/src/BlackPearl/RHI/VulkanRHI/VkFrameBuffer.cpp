#include "pch.h"
#if GE_API_VULKAN

#include "VkFrameBuffer.h"
namespace BlackPearl {


    Framebuffer::~Framebuffer()
    {
        if (framebuffer && managed)
        {
            vkDestroyFramebuffer(m_Context.device, framebuffer, nullptr);
            //m_Context.device.destroyFramebuffer(framebuffer);
            //framebuffer = nullptr;
        }

        if (renderPass && managed)
        {
            //m_Context.device.destroyRenderPass(renderPass);

            vkDestroyRenderPass(m_Context.device, renderPass, nullptr);
            //renderPass = nullptr;
        }
    }

}
#endif