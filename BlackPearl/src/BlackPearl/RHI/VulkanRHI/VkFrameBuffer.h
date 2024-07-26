#pragma once
#if GE_API_VULKAN

#include "BlackPearl/RHI/RHIFrameBuffer.h"
#include "VkContext.h"
#include <vulkan/vulkan.h>
namespace BlackPearl {
    class Framebuffer : public RefCounter<IFramebuffer>
    {
    public:
        FramebufferDesc desc;
        FramebufferInfoEx framebufferInfo;

        VkRenderPass renderPass;
        VkFramebuffer framebuffer;

        std::vector<ResourceHandle> resources;

        bool managed = true;

        explicit Framebuffer(const VulkanContext& context)
            : m_Context(context)
        { }

        ~Framebuffer() override;
        const FramebufferDesc& getDesc() const override { return desc; }
        const FramebufferInfoEx& getFramebufferInfo() const override { return framebufferInfo; }

        //for opengl: delete later 
        virtual void Bind() override {};
        virtual void Unbind() override {};
       
       // Object getNativeObject(ObjectType objectType) override;

    private:
        const VulkanContext& m_Context;
    };

}
#endif
